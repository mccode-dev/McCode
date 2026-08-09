#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import logging
import argparse
import json
import sys
import os
from os.path import join, dirname, isdir
from os import walk
import subprocess
import shutil
import concurrent.futures
import jinja2

ERROR_PERCENT_THRESSHOLD_ACCEPT = 20

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from mccodelib import utils, mccode_config

def scantree(path):
    """Recursively yield DirEntry objects for given directory."""
    for entry in os.scandir(path):
        if entry.is_dir(follow_symlinks=True):
            yield entry

def get_oldest_dir(directory_name):
    files = []

    for file in scantree(directory_name):
        files.append((file.stat().st_mtime, file.path))

    files.sort(key=lambda x:x[0])
    return files[0][1]

def get_default_diffworkers():
    ''' Number of mcplotdiff-html comparisons to run in parallel by default:
        the number of processors available to this process. Prefers
        os.sched_getaffinity(0) (Linux only) over os.cpu_count(), since the
        former respects cgroup/taskset CPU restrictions (e.g. inside a
        container or batch job allocation) that the latter ignores. '''
    try:
        return len(os.sched_getaffinity(0))
    except AttributeError:
        # os.sched_getaffinity doesn't exist on macOS/Windows
        return os.cpu_count() or 4

def run_normal_mode(testdir, reflabel, nodiff=False, diffmax=300, diffall=True, diffworkers=None):
    ''' load test data and print to html label '''

    if diffworkers is None:
        diffworkers = get_default_diffworkers()

    # jobs collected during row-building, run in parallel afterwards (see
    # plan_diff_link()/plan_coplot_link() below for why this is a
    # two-phase process)
    pending_diff_jobs = []

    # e.g. MCPLOT="mcplot-pyqtgraph" (whatever backend the user has
    # configured as their default plotter) -> mcplot_prefix="mcplot",
    # regardless of which backend that happens to be - the diff/coplot
    # comparison cells always use the html backend specifically, since
    # that's what's viewable directly from the generated report without
    # needing a local GUI session.
    mcplot_prefix = mccode_config.configuration["MCPLOT"].split('-')[0]  # "mcplot" or "mxplot"
    diffplotter = mcplot_prefix + "diff-html"                            # "mcplotdiff-html"
    coplotter = mcplot_prefix.replace('plot', '') + "coplot-html"        # "mccoplot-html"

    def get_col_header(label, meta):
        try:
            return "<br>".join((label + " - " + meta.get("ncount", ""), meta.get("hostname", ""), "CPU: " + meta.get("cpu_type", ""), "GPU: " + meta.get("gpu_type", ""), meta.get("date", "")))
        except:
            return "<br>UNDEFINED"

    def get_header_lst(meta):
        ''' composes an easily-templatable list fom a "_meta" test header object '''
        if meta is not None:
            lst = []
            lst.append(meta["ncount"])
            lst.append(meta["date"])
            lst.append(meta["hostname"])
            lst.append(meta["user"])
            lst.append(meta["cpu_type"])
            lst.append(meta["gpu_type"])
            lst.append(meta["date"])
        return lst

    def get_data_url(cellobj):
        ''' Reconstructs the relative "label/instrname/testnb/" data directory
            for a cellobj, the same way get_cell_tuple() does for its own
            cell - used here to also locate the *reference* column's data
            directory when generating a diff link. '''
        label = cellobj["localfile"].split("/")
        if len(label) == 1:
            label = cellobj["localfile"].split("\\")
        label = label[len(label) - 3]
        return label + "/" + cellobj["instrname"] + "/" + str(cellobj["testnb"]) + "/"

    def plan_diff_link(refcellobj, url, label, row, col_idx):
        ''' Decides whether a diff against the reference column is
            applicable, and if so either:
              - returns the link immediately, if a cached diff already
                exists on disk, or
              - registers a pending job to be run later (see the parallel
                execution pass after all rows have been built, below), and
                optimistically returns the link the job is expected to
                produce. If the job later fails, the corresponding cell in
                `row` is patched to drop the link (see the execution pass).

            This is deliberately NOT where the mcplotdiff-html subprocess is
            actually run: with ~300+ rows x multiple columns, running these
            one at a time while building the table serializes what is an
            embarrassingly parallel, purely I/O+CPU bound batch of
            independent jobs (disjoint output directories, no shared
            state). Collecting them here and running them via a bounded
            thread pool afterwards is a straightforward, safe win. '''
        if nodiff:
            return None
        if refcellobj is None or refcellobj.get("testval") is None:
            # no valid reference data to diff against
            return None

        ref_url = get_data_url(refcellobj)
        test_abs = join(testdir, url)
        ref_abs = join(testdir, ref_url)
        if not (os.path.isfile(join(test_abs, "mccode.sim")) and os.path.isfile(join(ref_abs, "mccode.sim"))):
            # NeXus-only output (mccode.h5) or otherwise nothing mcplotdiff-html can compare
            return None

        outdir_rel = join(url, "diff_vs_%s" % reflabel)
        outdir_abs = join(testdir, outdir_rel)
        index_abs = join(outdir_abs, "index.html")
        link = (outdir_rel + "/index.html").replace(os.sep, '/').replace('//', '/')

        if os.path.isfile(index_abs):
            # already cached from a previous run, nothing to do
            return link

        cmd = '%s "%s" "%s" --nobrowse -A "%s" -B "%s" --output "%s"' % (
            diffplotter, test_abs, ref_abs, label, reflabel, outdir_abs)

        pending_diff_jobs.append({
            'cmd': cmd,
            'index_abs': index_abs,
            'row': row,
            'col_idx': col_idx,
            'url_index': 8,  # position of diffurl in the cell tuple returned by get_cell_tuple()
        })
        return link

    def plan_coplot_link(refcellobj, url, label, row, col_idx):
        ''' Same idea as plan_diff_link(), but for an mcplot-coplot-html
            overlay (a/b plotted on the same axes) rather than a
            difference plot. Independent of, but run through the same
            pending-jobs pool as, the diff link above - both compare the
            same cell against the same reference column, they just show
            the comparison two different ways. '''
        if nodiff:
            return None
        if refcellobj is None or refcellobj.get("testval") is None:
            # no valid reference data to co-plot against
            return None

        ref_url = get_data_url(refcellobj)
        test_abs = join(testdir, url)
        ref_abs = join(testdir, ref_url)
        if not (os.path.isfile(join(test_abs, "mccode.sim")) and os.path.isfile(join(ref_abs, "mccode.sim"))):
            # NeXus-only output (mccode.h5) or otherwise nothing mcplot-coplot-html can compare
            return None

        outdir_rel = join(url, "coplot_vs_%s" % reflabel)
        outdir_abs = join(testdir, outdir_rel)
        index_abs = join(outdir_abs, "index.html")
        link = (outdir_rel + "/index.html").replace(os.sep, '/').replace('//', '/')

        if os.path.isfile(index_abs):
            # already cached from a previous run, nothing to do
            return link

        cmd = '%s "%s" "%s" --nobrowse -A "%s" -B "%s" --output "%s"' % (
            coplotter, test_abs, ref_abs, label, reflabel, outdir_abs)

        pending_diff_jobs.append({
            'cmd': cmd,
            'index_abs': index_abs,
            'row': row,
            'col_idx': col_idx,
            'url_index': 9,  # position of coplotUrl in the cell tuple returned by get_cell_tuple()
        })
        return link

    def get_cell_tuple(cellobj, refval=None, refcellobj=None, row=None, col_idx=None):
        ''' set up and format cell data '''
        state = None
        compiletime = None
        runtime = None
        testval = None
        refp = None
        # Decostruct localfile path to find 'label' corresponding to current cell
        label = cellobj["localfile"].split("/");
        # If localfile was not split by "/", the input was likely generated on Windows:
        if len(label)==1:
            label = cellobj["localfile"].split("\\");

        label=label[len(label)-3];
        # URL for test output data in success (i.e. 1/ or 2/ etc.)
        url =  label + "/" + cellobj["instrname"] +  "/" + str(cellobj["testnb"]) + "/"
        # URL for compiling instrs - without data
        burl = label + "/" + cellobj["instrname"] +  "/"
        # URL for instruments that failed compiling
        curl = label + "/" + cellobj["instrname"] +  "/compile_stdout.txt"
        # Display URL if display succeeded
        durl = label + "/" + cellobj["instrname"] +  "/display/index.html"
        # Display URL if display failed
        fdurl = label + "/" + cellobj["instrname"] +  "/displaylog.txt"

        if cellobj["testnb"] <= 1:
            if not cellobj["displayed"]:
                val=-1
                if cellobj["displaytime"] is not None:
                    val=cellobj["displaytime"]
                display="<strong><font color=\"#FFA500\">DISPLAY(%d s): FAILED! (log)</font></strong>"  % val
                displayurl=fdurl
            else:
                display="<strong>DISPLAY(%d s): OK (link)<strong>" % cellobj["displaytime"]
                displayurl=durl
        else:
            display=""
            displayurl=""
        
        if not cellobj["compiled"]:
            if cellobj["linted"]:
                state = 4
                return (state, "<strong><font color=\"#8B4000\">C-linter output</font></strong>", "", "", "", curl, "", "")
            else:
                state = 4
                return (state, "<strong><font color=\"red\">! Compile error !</font></strong>", "", "", "", curl, "", "")
        elif not cellobj["didrun"]:
            state = 3
            compiletime = "%.2f s" % cellobj["compiletime"]
            if cellobj["testnb"] > 1:
                # if this is a second test of the same instr, it was already compiled, thus 0.001 compiletime is nonsense
                compiletime = ""
            return (state, compiletime, "", "", "", burl, display, displayurl)
        elif cellobj["testval"]==None:
            testval = "missing"
            runtime = "%.2f s" % cellobj["runtime"]
            compiletime = "%.2f s" % cellobj["compiletime"]
            if cellobj["testnb"] > 1:
                compiletime = ""
            state = 2
            return (state, compiletime, runtime, testval, "", url, display, displayurl)
        else:
            testval = "%.2g" % float(cellobj["testval"])
            runtime = "%.2f s" % cellobj["runtime"]
            compiletime = "%.2f s" % cellobj["compiletime"]
            if cellobj["testnb"] > 1:
                compiletime = ""

            # Always use embedded target value
            refval = float(cellobj["targetval"])
            testval = float(cellobj["testval"])
            # Special case, target test value is 0 explicitly:
            if refval==0:
                if testval==0:
                    refp=100
                else:
                    refp=0
            else: # Standard case, target test value is non-zero
                refp = abs(testval/refval*100)
            if abs(refp-100) > ERROR_PERCENT_THRESSHOLD_ACCEPT:
                state = 2
            else:
                state = 1
            if abs(refp)>1000:
                refp = "%2.g" % refp + "%"
            else:
                refp = "%2.f" % refp + "%"

            diffurl = None
            coplotUrl = None
            if diffall or state == 2:
                diffurl = plan_diff_link(refcellobj, url, label, row, col_idx)
                coplotUrl = plan_coplot_link(refcellobj, url, label, row, col_idx)

            return (state, compiletime, runtime, testval, refp, url, display, displayurl, diffurl, coplotUrl)

    def get_empty_cell_tuple(tag=None):
        ''' return a "state_four" black cell, optionally with a tag, this could be "no ref" or "no test" etc. '''
        if tag is not None:
            return (4, tag)
        return (4, )

    def has_test(labels):
        ''' labels : [(t, obj, meta)] '''
        for l in labels:
            if len(l.keys()) > 0:
                return True
        return False

    def iterate_obj_to_populate_rows(iterobj, otherobjs, rows, ncols, use_iterobj_refvalue=True, del_used_from_overobjs=True):
        '''
        Used to construct rows from a dict and a list of dicts with similar keys, either
        from a reference column, or as egalitarian with a lead "iterate" object. Appends to rows.
        
        cols: if higher than 1+len(otherobjs), empty cells are first appended to rows, in order to orient cols correctly)
        '''
        # use default order, default sorting (e.g. list.sort()) wasn't satisfactory
        for key in list(iterobj.keys()): 
            row = []
            rows.append(row)
            # instr
            row.append(key)

            # prepare row list to have the requested amount of cells (cols)
            for i in range(ncols - len(otherobjs) - 1):
                tag = "no test"
                if i == 0:
                    tag = "not on branch"
                row.append(get_empty_cell_tuple(tag))

            # ref col
            row.append(get_cell_tuple(iterobj[key]))

            # remaining cols
            for obj in otherobjs:
                o = obj.get(key, None)
                if o:
                    # use reference/iterobj targetval, or native/None
                    targetval = o.get("targetval", None)
                    if use_iterobj_refvalue:
                        targetval = iterobj[key]["targetval"]
                    # diff cells always compare against the true reference
                    # column (refobj), regardless of which object is
                    # currently driving iteration (iterobj may be a
                    # fallback "lead" column rather than refobj itself, in
                    # the untested multi-column case below)
                    refcellobj = refobj.get(key, None)
                    col_idx = len(row)
                    row.append(get_cell_tuple(o, targetval, refcellobj=refcellobj, row=row, col_idx=col_idx))

                    # delete "used" cell keys
                    if del_used_from_overobjs:
                        del obj[key]
                else:
                    errmsg = iterobj[key]["errmsg"]
                    row.append(get_empty_cell_tuple("not on branch"))

    # load test data
    alllabels = list()
    for root, labels, files in walk(testdir):
        for lab in labels:
            if isdir(lab):
                try:
                    obj = json.loads(open(join(testdir, lab, "testresults_%s.json" % lab)).read())
                    alllabels.append(lab)
                except:
                    pass
    alllabels.sort()
    # get number of data columns
    numcols= len(alllabels)

    refobj = None
    refmeta = None
    testlabels = []
    testobjs = []
    testmetas =  []
    for t in alllabels:
        obj = json.loads(open(join(testdir, t, "testresults_%s.json" % t)).read())
        meta = obj.get("_meta", None)
        if meta:
            del obj["_meta"]
        if reflabel == t:
            refobj = obj
            refmeta = meta
        else:
            testlabels.append(t)
            testobjs.append(obj)
            testmetas.append(meta)

    # create header row
    hrow = []
    hrow.append(get_col_header("%s (ref)" % reflabel, refmeta))
    for i in range(len(testlabels)):
        hrow.append(get_col_header(testlabels[i], testmetas[i]))

    # create rows - 1) all instr tests in reference
    rows = []
    iterate_obj_to_populate_rows(refobj, testobjs, rows, ncols=numcols)

    # WARNING: untested in the non-trivial case
    while has_test(testobjs):
        leadcol = testobjs.pop(0)
        iterate_obj_to_populate_rows(leadcol, testobjs, rows, ncols=numcols, use_iterobj_refvalue=False)

    # Run all collected mcplotdiff-html/mcplot-coplot-html jobs in parallel
    # now that every row has been built (see plan_diff_link()/
    # plan_coplot_link() above). Each job's cell already holds the
    # *expected* link optimistically; if the job fails or times out, patch
    # that specific cell back to drop the link rather than leaving a dead
    # one in the rendered report.
    if pending_diff_jobs:
        logging.info("Running %d mcplotdiff-html/mcplot-coplot-html comparison(s) (up to %d in parallel)..."
                      % (len(pending_diff_jobs), diffworkers))

        def _run_diff_job(job):
            try:
                utils.run_subtool_noread(job['cmd'], cwd=testdir, timeout=diffmax)
            except Exception as e:
                logging.info("diff/coplot job failed: %s" % str(e))
            return job

        done = 0
        with concurrent.futures.ThreadPoolExecutor(max_workers=diffworkers) as executor:
            futures = [executor.submit(_run_diff_job, job) for job in pending_diff_jobs]
            for future in concurrent.futures.as_completed(futures):
                job = future.result()
                if not os.path.isfile(job['index_abs']):
                    # job failed/timed out: drop the optimistically-set
                    # link, but *only* at this job's own url_index (8 for
                    # a diff job, 9 for a coplot job) - a diff job and a
                    # coplot job for the same cell run independently and
                    # can complete in either order, so blanket-truncating
                    # the tuple here (as opposed to patching just the one
                    # slot) would silently wipe out a successful result
                    # from the other job type sharing this same cell.
                    row = job['row']
                    col_idx = job['col_idx']
                    url_index = job['url_index']
                    cell = list(row[col_idx])
                    while len(cell) <= url_index:
                        cell.append(None)
                    cell[url_index] = None
                    row[col_idx] = tuple(cell)
                done += 1
                if done % 20 == 0 or done == len(pending_diff_jobs):
                    logging.info("  ...%d/%d diffs/coplots done" % (done, len(pending_diff_jobs)))

    text = open(join(dirname(__file__), "main.template")).read()
    html = jinja2.Template(text).render(hrow=hrow, rows=rows, header=get_header_lst(refmeta))

    # Platform-independent ofile (ensures correct behaviour of mcviewtest and opens browser also on Windows, 
    # irrespective of 'testdir' given as '.' or no inputs)
    ofile = os.path.join(testdir, "%s_output.html" % os.path.basename(os.path.normpath(testdir)))
    print("writing ofile: %s" % ofile)
    open(ofile, "w").write(html)
    return ofile

def run_interactive_mode(testroot):
    ''' a simple utility for deleting useless test directories '''
    for _, dirs, _ in walk(testroot):
        for d in dirs:
            print(d)
            testdir = join(testroot, d)
            for _, tests, _ in walk(join(testroot, d)): break
            for t in tests:
                for _, instrs, _ in walk(join(testroot, d, t)): break
                print("    " + t + " (%d)" % len(instrs))
            ans = input("delete %s (y/n)? " % join(testroot, d))
            if ans == "y":
                try:
                    shutil.rmtree(testdir)
                except Exception as e:
                    print("could not be deleted...", str(e))
        break
    print("maintenance complete")

def main(args):
    if args.verbose:
        logging.basicConfig(level=logging.DEBUG, format="%(message)s")
    else:
        logging.basicConfig(level=logging.INFO, format="%(message)s")

    testroot = args.testroot
    testdir = None
    if args.testdir:
        testdir = args.testdir

    if not args.reflabel:
        print("\nNo reflabel defined, will use oldest subfolder in current workdir")
        print("(cwd=%s)" % os.getcwd())
        reflabel= get_oldest_dir(os.getcwd())
        if reflabel is None:
            print("No subfolders found in current workdir:")
            exit(-1)
        else:
            reflabel=os.path.basename(reflabel)
            print("--> Using reflabel=%s\n" % reflabel)
    else:
        reflabel = args.reflabel

    if not testdir and testroot:
        print("interactive mode")
        run_interactive_mode(testroot)
        exit(-1)
    else:
        if testdir is None:
            print("No testdir defined, will use current dir")
            print("--> Using testdir=%s\n" % os.getcwd())
            testdir=os.getcwd()
        diffmax = 300
        if args.diffmax:
            diffmax = int(args.diffmax[0])
        diffworkers = get_default_diffworkers()
        if args.diffworkers:
            diffworkers = int(args.diffworkers[0])
        diffall = not args.diff_errors_only
        ofile = run_normal_mode(testdir, reflabel, nodiff=args.nodiff, diffmax=diffmax,
                        diffall=diffall, diffworkers=diffworkers)

    if not args.nobrowse:
        subprocess.Popen('%s %s' % (mccode_config.configuration['BROWSER'], ofile), shell=True)
        quit()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('testdir', nargs="?", help='test data is drawn from this root folder')
    parser.add_argument('--reflabel', nargs="?", help='Reference label name for comparison between multiple test runs (oldest subfolder in PWD is used if unset)')
    parser.add_argument('--testroot', nargs="?", help='test root folder for test result management')
    parser.add_argument('--verbose', action='store_true', help='output excessive information for debug purposes')
    parser.add_argument('--nobrowse', action='store_true', help='Do not spawn browser on exit')
    parser.add_argument('--nodiff', action='store_true', help='Do not generate mcplotdiff-html comparison cells against the reference column')
    parser.add_argument('--diff-errors-only', dest='diff_errors_only', action='store_true', help='Only generate diff cells for rows that show a discrepancy against the reference (default: diff every row with valid data)')
    parser.add_argument('--diffmax', nargs=1, help='Maximum time (s) allowed per mcplotdiff-html comparison (default 300s)')
    parser.add_argument('--diffworkers', nargs=1, help='Number of mcplotdiff-html comparisons to run in parallel (default: number of available processors)')
    args = parser.parse_args()

    main(args)


