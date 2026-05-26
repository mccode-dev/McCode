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

def run_normal_mode(testdir, reflabel):
    ''' load test data and print to html label '''

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

    def get_cell_tuple(cellobj, refval=None):
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
                display="<strong><font color=\"#FFA500\">DISPLAY: FAILED! (log)</font></strong>"
                displayurl=fdurl
            else:
                display="<strong>DISPLAY: OK (link)<strong>"
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

            return (state, compiletime, runtime, testval, refp, url, display, displayurl)

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
                    row.append(get_cell_tuple(o, targetval))

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

    text = open(join(dirname(__file__), "main.template")).read()
    html = jinja2.Template(text).render(hrow=hrow, rows=rows, header=get_header_lst(refmeta))

    datetime = testdir.split("/")[-1]
    ofile = "%s_output.html" % datetime
    print("writing ofile: %s" % ofile)
    open(ofile, "w").write(html)

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
        run_normal_mode(testdir, reflabel)

    if not args.nobrowse:
        subprocess.Popen('%s %s' % (mccode_config.configuration['BROWSER'], os.path.join(testdir,os.path.basename(testdir) +'_output.html')), shell=True)
        quit()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('testdir', nargs="?", help='test data is drawn from this root folder')
    parser.add_argument('--reflabel', nargs="?", help='Reference label name for comparison between multiple test runs (oldest subfolder in PWD is used if unset)')
    parser.add_argument('--testroot', nargs="?", help='test root folder for test result management')
    parser.add_argument('--verbose', action='store_true', help='output excessive information for debug purposes')
    parser.add_argument('--nobrowse', action='store_true', help='Do not spawn browser on exit')
    args = parser.parse_args()

    main(args)


