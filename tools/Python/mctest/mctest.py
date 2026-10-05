#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import logging
import argparse
import json
import os
join = os.path.join
from os.path import basename, join, isdir, splitext
from os import mkdir
from collections import OrderedDict
import sys
import re
import time
import math
import pathlib
import shutil
import platform
import subprocess
import io
from datetime import datetime

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from mccodelib import utils, mccode_config

def get_processor_info():
    if platform.system() == "Windows":
        return platform.processor()
    elif platform.system() == "Darwin":
        return subprocess.check_output(['/usr/sbin/sysctl', "-n", "machdep.cpu.brand_string"]).strip().decode('utf-8')
    elif platform.system() == "Linux":
        command = r"cat /proc/cpuinfo | grep model\ name | uniq | cut -f2 -d:"
        return subprocess.check_output(command, shell=True).strip().decode('utf-8')

    return ""

def paths_overlap(a: pathlib.Path, b: pathlib.Path) -> bool:
    a = a.resolve()
    b = b.resolve()
    return a == b or a.is_relative_to(b) or b.is_relative_to(a)

#
# Functionality
#

def create_instr_test_objs(sourcefile, localfile, header):
    ''' returns a list containing one initialized test object pr %Example within the instr file '''
    tests = []
    ms = re.findall(r"\%Example:([^\n]*)Detector\:([^\n]*)_I=([0-9.+-e]+)", header)
    if len(ms) > 0:
        testnb = 1
        for m in ms:
            parvals = m[0].strip()
            detector = m[1].strip()
            targetval = float(m[2].strip())
            tests.append(InstrExampleTest(sourcefile, localfile, parvals, detector, targetval, testnb))
            testnb = testnb + 1
    else:
        tests.append(InstrExampleTest(sourcefile, localfile))
    return tests

class InstrExampleTest:
    ''' instruent test house keeping object '''
    def __init__(self, sourcefile, localfile, parvals=None, detector=None, targetval=None, testnb=0):
        self.sourcefile = sourcefile
        self.localfile = localfile
        self.instrname = splitext(basename(sourcefile))[0]
        self.testnb = testnb
        
        self.parvals = parvals
        self.detector = detector
        self.targetval = targetval
        self.testval = None

        self.linted = None
        self.compiled = None
        self.compiletime = None
        self.displayed = None
        self.displaytime = None
        self.didrun = None
        self.runtime = None
        self.errmsg = None

    def get_json_repr(self):
        return {
            "displayname"  : self.get_display_name(),
            "sourcefile"   : self.sourcefile,
            "localfile"    : self.localfile,
            "instrname"    : self.instrname,
            "testnb"       : self.testnb,

            "parvals"      : self.parvals,
            "detector"     : self.detector,
            "targetval"    : self.targetval,
            "testval"      : self.testval,

            "linted"       : self.linted,
            "compiled"     : self.compiled,
            "compiletime"  : self.compiletime,
            "displayed"    : self.displayed,
            "displaytime"  : self.displaytime,
            "didrun"       : self.didrun,
            "runtime"      : self.runtime,
            "errmsg"       : self.errmsg,
        }
    def save(self, infolder):
        text = json.dumps(self.get_json_repr(), indent=2)
        f = open(join(infolder, self.get_display_name()) + ".json", 'w').write(text)
    def load(self,testnb=0):
        jsonfile=pathlib.Path(os.path.join(os.path.dirname(self.localfile),self.get_display_name()+'.json'))
        if jsonfile.is_file():
            f = open(jsonfile, "r", encoding="utf-8")
            obj = json.load(f)
            # # Populate test
            self.displayname=obj['displayname']
            self.sourcefile=obj['sourcefile']
            self.localfile=obj['localfile']
            self.instrname=obj['instrname']
            self.testnb=obj['testnb']
            self.parvals=obj['parvals']
            self.detector=obj['detector']
            self.targetval=obj['targetval']
            self.testval=obj['testval']
            self.linted=obj['linted']
            self.compiled=obj['compiled']
            self.compiletime=obj['compiletime']
            self.displaytime=obj.get('displaytime')
            self.displayed=obj['displayed']
            self.didrun=obj['didrun']
            self.runtime=obj['runtime']
            self.errmsg=obj['errmsg']
            return True
        else:
            return False

    def get_display_name(self):
        if self.testnb > 1:
            return self.instrname + "_%d" % self.testnb
        else:
            return self.instrname

class LineLogger():
    ''' log lines to memory, then save to disk '''
    def __init__(self):
        self.lst = []
    def logline(self, addstr):
        self.lst.append(addstr)
    def save(self, filename):
        text = "\n".join(self.lst) + "\n"
        f = open(filename, 'w').write(text)
    def find(self, searchstr):
        for l in self.lst:
            if re.search(searchstr, l):
                return True
        return False

def _monitorname_filename_match(dfolder, monname):
    '''
    mccode.sim is organized in sections, e.g. data sections are coded by from "begin data" to "end data",
    within which "  component:" and "  filename:" tags may be available.
    
    returns the filename or None
    '''
    look_for_filename = False
    simfile=pathlib.Path(dfolder,"mccode.sim")
    if simfile.is_file():
        lns = open(str(simfile)).read().splitlines()
        for l in lns:
            if re.match(r"  component: %s$" % monname, l):
                # flag this data section
                look_for_filename = True
            if look_for_filename:
                m = re.match(r"\s*filename:\s+(.+)", l)
                if m:
                    filename = m.group(1)
                    if os.path.isfile(join(dfolder,filename)):
                        return filename
                if re.match(r"end data", l):
                    # the filename can for 0D monitors be monname.dat
                    zeroDfilename = join(dfolder, monname + ".dat")
                    if os.path.isfile(zeroDfilename):
                        return zeroDfilename
                    return None
    else:
        return None

def extract_testvals(datafolder, monitorname):
    '''
    Extract monitor I (as well as Ierr and N) from results dir given monitor name.
    
    Returns an error string or a tuple containing (I, I_err, N), or None if datafile does not contain a "values" line.
    '''
    # get any filename in mccode.sim matching monitorname AKA test.detector 
    filename = _monitorname_filename_match(datafolder, monitorname)
    if filename is None:
        return "ERROR: targetval could not be extracted from monitor %s" % (monitorname)

    # extract tested target value from the monitor file
    with open(join(datafolder, filename)) as fp:
        while True:
            l = fp.readline()
            if not l:
                break
            m = re.match(r"# values: ([0-9+-e.]+) ([0-9+-e.]+) ([0-9]+)", l)
            if m :
                I = float(m.group(1))
                I_err = float(m.group(2))
                N = float(m.group(3))
                return (I, I_err, N)
                break

def parse_detector_I_value(resfile_path, detector_name):
    """
    Return (value_float, success_bool, raw_value_str_or_None).
    value_float is the parsed float (or -1.0 on failure).
    success_bool is True when a value was parsed successfully.
    raw_value_str_or_None is the string extracted (before conversion) or None.
    """
    prefix = f"Detector: {detector_name}_I="
    try:
        with open(resfile_path, "r", encoding="utf-8", errors="replace") as f:
            for line in f:
                if line.startswith(prefix):
                    # extract after first '=' then take up to first whitespace
                    # matches the shell pipeline: cut -f2 -d= | cut -f1 -d' '
                    _, _, after_eq = line.partition("=")
                    raw = after_eq.split()[0] if after_eq else ""
                    if raw == "":
                        return -1.0, False, None
                    try:
                        return float(raw), True, raw
                    except ValueError:
                        return -1.0, False, raw
        return -1.0, False, None
    except FileNotFoundError:
        return -1.0, False, None
    except OSError:
        return -1.0, False, None

def mccode_test(branchdir, testdir, limitinstrs=None, instrfilter=None, compfilter=None, configdir=None):
    ''' this main test function tests the given mccode installation, optionally with the
    mccode_config.json in configdir (passed to mcrun --override-config) '''
    skipped=False
    global runLocal
    # copy instr files and record info
    if not runLocal:
        searchdir = str(pathlib.Path(branchdir,"examples").resolve())
        logging.info("Finding instruments in: %s" % searchdir)
    else:
        searchdir = str(pathlib.Path(runLocal).resolve())
        logging.info("Adding instruments from subfolders in: %s" % str(pathlib.Path(".").resolve()))
    if instrfilter is not None and compfilter is not None:
        # --instr and --comp together: union, i.e. instruments matching --instr
        # plus instruments using any of the --comp components (single-pass test)
        instrs, _ = utils.get_instr_comp_files(searchdir, recursive=True, instrfilter=instrfilter)
        users, _ = utils.get_instr_comp_files(searchdir, recursive=True, withcomp=compfilter)
        instrs = list(set(instrs) | set(users))
    else:
        instrs, _ = utils.get_instr_comp_files(searchdir, recursive=True, instrfilter=instrfilter, withcomp=compfilter)
    if compfilter is not None:
        logging.info("(Instrument list includes those using component(s) %s )" % compfilter)
    instrs.sort()

    # limt runs if required
    if limitinstrs:
        instrs = instrs[:limitinstrs]

    # max instr name length for pretty-output
    maxnamelen = 0
    for f in instrs:
        l = len(basename(f)) - 5
        if l > maxnamelen:
            maxnamelen = l

    # create test objects and copy instrument files
    logging.info("Copying instruments to: %s" % testdir)
    tests = []
    for f in instrs:
        # copy the test folder for this instrument
        instrname = splitext(basename(f))[0]
        instrdir = join(testdir, instrname)
        
        # Read instr file content to look for tests
        text = open(f, encoding='utf-8').read()
        f_new=str(pathlib.Path(join(instrdir,os.path.basename(f))).as_posix())
        # create a test object for every test defined in the instrument header
        instrtests = create_instr_test_objs(sourcefile=f, localfile=f_new, header=text)
        try:
            shutil.copytree(os.path.dirname(f),instrdir)
            tests = tests + instrtests

            # extract and record %Example info from text
            numtests = len([t for t in instrtests if t.testnb > 0]) 
            if numtests == 0:
                formatstr = "%-" + "%ds: NO TEST" % maxnamelen
                logging.debug(formatstr % instrname)
            elif numtests == 1:
                formatstr = "%-" + "%ds: TEST" % maxnamelen
                logging.debug(formatstr % instrname)
            else:
                formatstr = "%-" + "%ds: TESTS (%d)" % (maxnamelen, numtests)
                logging.debug(formatstr % instrname)
        except:
            print("\nWARNING: Skipped " + instrname + " test - did " + instrdir + " exist already??\n")
            skipped=True
            populated=[]
            for t in instrtests:
                if t.testnb >= 0:
                    if t.load(t.testnb)==True:
                        populated.append(t)
            tests = tests + populated
            pass

    # Issue counters
    num_compilefail=0
    num_runfail=0
    num_valfail=0
    num_noexample=0

    # Over all test success flag
    anyfailed=False

    # compile, record time
    global ncount, no_mpi, mpi, openacc, suffix, nexus, lint, permissive, compilemax, displaymax, runmax, seed, strict, noplots
    logging.info("")
    if not lint:
        logging.info("Compiling instruments [seconds]...")
    else:
        logging.info("c-lint'ing instruments [seconds]...")

    for test in tests:
        if strict and test.testnb==0:
            logging.info("!! No test(s) found in %s !!" % test.instrname)
            anyfailed=True
            num_noexample = num_noexample + 1
            pass
        # if binary exists, set compile time = 0 and continue
        binfile = os.path.splitext(test.localfile)[0] + "." + mccode_config.platform["EXESUFFIX"].lower()
        compilefailed=os.path.splitext(test.localfile)[0] + ".failed"
        # if we linted, continue
        linted=os.path.splitext(test.localfile)[0] + ".linted"
        if os.path.exists(compilefailed):
            test.compiled = False
            test.compiletime = -1
            anyfailed=True
        elif os.path.exists(binfile):
            test.compiled = True
            test.compiletime = 0
        elif os.path.exists(linted):
            test.linted = True
        else:
            if test.testnb > 0 or (not args.skipnontest):
                log = LineLogger()
                t1 = time.time()
                cmd = mccode_config.configuration["MCRUN"]
                if lint:
                    cmd = cmd + " --verbose -C %s > compile_stdout.txt 2>&1" % test.instrname
                else:
                    if nexus:
                        cmd = cmd + " --format=NeXus "
                    if configdir:
                        cmd = cmd + " --override-config=" + configdir
                    if openacc:
                        cmd = cmd + " --openacc "
                    mpiswitch = ''
                    if no_mpi:
                        cmd = cmd + " --no-mpi "
                        mpiswitch = " --no-mpi "
                        mpi=None

                    if mpi:
                        cmd = cmd + " --mpi=1 "
                    cmd = cmd + " --verbose -c -n0 %s > compile_stdout.txt 2>&1" % test.instrname

                utils.run_subtool_noread(cmd, cwd=join(testdir, test.instrname), timeout=compilemax)
                t2 = time.time()
                test.compiled = os.path.exists(binfile)
                test.compiletime = t2 - t1

                # log to terminal
                if test.compiled:
                    formatstr = "%-" + "%ds: " % maxnamelen + \
                      "{:3d}.".format(math.floor(test.compiletime)) + str(test.compiletime-int(test.compiletime)).split('.')[1][:2]
                    logging.info(formatstr % test.get_display_name())
                    # Run mcdisplay (single particle only)
                    t1 = time.time()
                    if test.testnb>0:
                        cmd = mccode_config.configuration["MCDISPLAY"]+'-classic %s --nobrowse %s %s -n0 -d display > displaylog.txt 2>&1' % (mpiswitch, test.instrname+'.instr', test.parvals if test.parvals else '-y')
                    else:
                        cmd = mccode_config.configuration["MCDISPLAY"]+'-classic %s --nobrowse %s -y -n0 -d display > displaylog.txt 2>&1' % (mpiswitch, test.instrname+'.instr')
                    retcode = utils.run_subtool_noread(cmd, cwd=join(testdir, test.instrname), timeout=displaymax)
                    t2 = time.time()
                    if retcode[0]==0:
                        test.displayed = True
                    else:
                        test.displayed = False
                    test.displaytime = t2 - t1
                else:
                    if lint:
                        formatstr = "%-" + "%ds: Linted using using:\n" % maxnamelen
                        logging.info(formatstr % test.instrname + cmd)
                        f = open(linted, "a")
                        f.write(formatstr % test.instrname + cmd)
                        f.close()
                        test.linted = True
                    else:
                        num_compilefail = num_compilefail + 1
                        anyfailed = True
                        formatstr = "%-" + "%ds: COMPILE ERROR using:\n" % maxnamelen
                        logging.info(formatstr % test.instrname + cmd)
                        f = open(compilefailed, "a")
                        f.write(formatstr % test.instrname + cmd)
                        f.close()
            else:
                logging.info("Skipping compile of " + test.instrname)
                skipped=True
        # save (incomplete) test results to disk
        if not skipped:
            test.save(infolder=join(testdir, test.instrname))

    # run, record time
    logging.info("")
    logging.info("Running tests / getting status...")
    runfailed=False
    for test in tests:
        if strict and test.testnb==0:
            runfailed = True
            formatstr = "%-" + "%ds: FAILURE: tool in --strict mode and instrument includes no %%%%Example: line(s)!" % (maxnamelen+1)
            logging.info(formatstr % test.instrname)
            continue
        elif test.linted:
            formatstr = "%-" + "%ds:  Linter only" % (maxnamelen+1)
            logging.info(formatstr % test.instrname)
            continue
        elif not test.compiled:
            formatstr = "%-" + "%ds:   NO COMPILE" % (maxnamelen+1)
            logging.info(formatstr % test.instrname)
            continue
        if test.testnb <= 1:
            displaytime = test.displaytime if test.displaytime is not None else 0
            if test.displayed:
                formatstr = "%-" + "%ds:   Display OK (%ds)" % (maxnamelen+1, displaytime)
                logging.info(formatstr % test.instrname)
            else:
                formatstr = "%-" + "%ds:   Display FAILED (%ds)" % (maxnamelen+1, displaytime)
                logging.info(formatstr % test.instrname)

        # runable tests have testnb > 0
        if test.testnb <= 0:
            formatstr = "%-" + "%ds:   NO TEST" % (maxnamelen+1)
            logging.info(formatstr % test.get_display_name())
            continue

        # run the test, record time and runtime success/fail
        t1 = time.time()
        cmd = mccode_config.configuration["MCRUN"]

        suffix=""
        # An instrument run without any parameters asks for their values
        # interactively, so for a %Example line without parameters, mcrun is
        # told to use the default values (-y can not be combined with
        # parameter values, since the instrument then ignores those):
        parvals = test.parvals if test.parvals else "-y"
        # Did test run already?
        if not os.path.exists(join(testdir, test.instrname, str(test.testnb))):      
            if nexus:
                cmd = cmd + " --format=NeXus "
            if mpi is not None:
                if openacc is True:
                    if configdir:
                        cmd = cmd + " --override-config=" + configdir
                    cmd = cmd + " -s %s %s %s -n%s --openacc --mpi=%s -d%d > run_stdout_%d.txt 2>&1" % (seed, test.instrname, parvals, ncount, mpi, test.testnb, test.testnb)
                else:
                    if configdir:
                        cmd = cmd + " --override-config=" + configdir
                    cmd = cmd + " -s %s %s %s -n%s --mpi=%s -d%d > run_stdout_%d.txt 2>&1" % (seed, test.instrname, parvals, ncount, mpi, test.testnb, test.testnb)
            else:
                if configdir:
                    cmd = cmd + " --no-mpi --override-config=" + configdir
                cmd = cmd + " --no-mpi -s %s %s %s -n%s -d%d > run_stdout_%d.txt 2>&1" % (seed, test.instrname, parvals, ncount, test.testnb, test.testnb)

            retcode = utils.run_subtool_noread(cmd, cwd=join(testdir, test.instrname),timeout=runmax)
            t2 = time.time()
            didwrite = os.path.exists(join(testdir, test.instrname, str(test.testnb), "mccode.sim"))
            didwrite_nexus = os.path.exists(join(testdir, test.instrname, str(test.testnb), "mccode.h5"))

            # retcode is a tuple: (returncode, timed_out)
            test.didrun = retcode[0] == 0 and not retcode[1] and (didwrite or didwrite_nexus)
            test.runtime = t2 - t1
        else:
            suffix=" (cached)"
            didwrite = os.path.exists(join(testdir, test.instrname, str(test.testnb), "mccode.sim"))
            didwrite_nexus = os.path.exists(join(testdir, test.instrname, str(test.testnb), "mccode.h5"))

        # log to terminal
        if not test.didrun:
            formatstr = "%-" + "%ds: RUNTIME ERROR" % (maxnamelen+1)
            logging.info(formatstr % test.get_display_name() + ", " + cmd)
            num_runfail = num_runfail + 1
            anyfailed = True
            runfailed = False
            suffix = ""
            test.testcomplete = True
            if not skipped:
                test.save(infolder=join(testdir, test.instrname))
            continue

        resbase="(No file)"
        # test value extraction
        if not didwrite_nexus:
            extraction = extract_testvals(join(testdir, test.instrname, str(test.testnb)), test.detector)
            if type(extraction) is tuple:
                test.testval = extraction[0]
            else:
                test.testval = -1
                runfailed=True
            resbase ="run_stdout_%d.txt" % (test.testnb)
            resfile = join(testdir,test.instrname,resbase)
        # Look for detector output in run_stdout
        else:
            metalog = LineLogger()
            resbase ="run_stdout_%d.txt" % (test.testnb)
            resfile = join(testdir,test.instrname,resbase)
            val, ok, raw = parse_detector_I_value(resfile, test.detector)
            if ok:
                test.testval = val
            else:
                test.testval=-1
                runfailed=True

        percent=0
        if test.didrun:
            if runfailed:
                num_runfail = num_runfail + 1
                anyfailed=True
                suffix += " + !! RUNTIME FAILURE - see %s !! " % (resbase)
            formatstr = "%-" + "%ds: " % (maxnamelen+1) + \
                "{:3d}.".format(math.floor(test.runtime)) + str(test.runtime-int(test.runtime)).split('.')[1][:2]
            if test.targetval!=0: # Normal situation, non-zero target value
                percent=round(100.0*test.testval/test.targetval)
                if percent<80 or percent>120:
                    suffix += " <--- BIG DISCREPANCY??"
                    num_valfail = num_valfail + 1
                    anyfailed=True
                logging.info(formatstr % test.get_display_name() + "    [val: " + str(test.testval) + " / " + str(test.targetval) + " = " + str(percent) + " %]" + suffix)
            else:                 # Special case, expected test target value is 0
                logging.info(formatstr % test.get_display_name() + "    [val: " + str(test.testval) + " vs " + str(test.targetval) + " (absolute vs 0) ]" + suffix)
                        # if output is not h5, launch plotter on the output data
            if didwrite and not noplots:
                # PDF overview plot
                matplotter  = mccode_config.configuration["MCPLOT"].split('-')[0] + "-matplotlib"
                cmd = matplotter + " %d/ --format=pdf --output %d/01_overview.pdf" %  (test.testnb, test.testnb)
                retcode = utils.run_subtool_noread(cmd, cwd=join(testdir, test.instrname),timeout=runmax)
                plot1 = retcode[0] == 0 and not retcode[1]
                # Interactive html plots
                htmlplotter = mccode_config.configuration["MCPLOT"].split('-')[0] + "-html"
                cmd = htmlplotter + " %d/ --nobrowse --output %d/02_plots.html" %  (test.testnb, test.testnb)
                retcode = utils.run_subtool_noread(cmd, cwd=join(testdir, test.instrname),timeout=runmax)
                plot2 = retcode[0] == 0 and not retcode[1]
                if plot1 and plot2:
                    logging.info(" - Test %d plots generated OK" % test.testnb)
                elif plot1:
                    logging.info(" - Test %d Overview plot OK, HTML plot Failure!" % test.testnb)
                elif plot2:
                    logging.info(" - Test %d HTML plot OK, Overview plot Failure!" % test.testnb)
                else:
                    logging.info(" - Test %d plots generation Failed!" % test.testnb)
        else:
            logging.info((formatstr % test.get_display_name()) + (" !! [TEST INDICATES RUNTIME ERROR - see %s  + suffix ] !!" % (resbase)))
        suffix=""
        # Reset
        runfailed=False
        # save test result to disk
        test.testcomplete = True
        if not skipped:
            test.save(infolder=join(testdir, test.instrname))

    #    cpu type: cat /proc/cpuinfo |grep name |uniq | cut -f2- -d: 
    #    gpu type: nvidia-smi -L | head -1 |cut -f2- -d: |cut -f1 -d\(

    cpu_type = "".join(get_processor_info())

    gpu_type = "none"
    if (platform.system() == "Linux"):
        metalog = LineLogger()
        utils.run_subtool_to_completion(r"nvidia-smi -L | head -1 |cut -f2- -d: |cut -f1 -d\(", stdout_cb=metalog.logline) 
        gpu_type = ",".join(metalog.lst)
        if "failed because" in gpu_type:
            gpu_type = "none"

    metalog = LineLogger()
    utils.run_subtool_to_completion("hostname", stdout_cb=metalog.logline)
    hostnamestr = ",".join(metalog.lst)

    metalog = LineLogger()
    utils.run_subtool_to_completion('echo "$USER"', stdout_cb=metalog.logline)
    username = ",".join(metalog.lst)

    metainfo = OrderedDict()
    metainfo["ncount"] = ncount
    metainfo["mpi"] = mpi
    metainfo["date"] = utils.get_datetimestr()
    metainfo["hostname"] = hostnamestr
    metainfo["user"] = username
    metainfo["cpu_type"] = cpu_type
    metainfo["gpu_type"] = gpu_type

    # displayname must be unique, we can return a dict, which eases comparison between tests
    obj = {}
    for t in tests:
        obj[t.get_display_name()] = t.get_json_repr()
    obj["_meta"] = metainfo
    return (obj, anyfailed, num_compilefail, num_runfail, num_valfail, num_noexample)

#
# Utility
#

def activate_mccode_version(mccoderoot):
    '''
    Modify environment, returns path as it was.
    
    mccoderoot: mccode install directory
    '''
    branchdir = mccoderoot
    os.environ["MCSTAS"] = branchdir
    oldpath = os.environ["PATH"]
    os.environ["PATH"] = "%s/miniconda3/bin:%s/bin:%s" % (branchdir, branchdir, oldpath)
    return oldpath

def deactivate_mccode_version(oldpath):
    ''' clean up path changes, restoring oldpath '''
    del os.environ["MCSTAS"]
    os.environ["PATH"] = oldpath

def create_test_dir(testdir):
    ''' just create testdir or exit '''
    if not os.path.exists(testdir):
        mkdir(testdir)
    if not os.path.exists(testdir):
        logging.info("could not create test folder, exiting...")
        quit()

def create_label_dir(testdir, label):
    if not os.path.exists(testdir):
        mkdir(testdir)
    if not os.path.exists(testdir):
        logging.info("could not create test folder, exiting...")
        quit()
    labeldir = join(testdir, label)
    if not os.path.exists(labeldir):
        mkdir(labeldir)
    return labeldir

def create_datetime_testdir(testroot):
    datetime = utils.get_datetimestr()
    return create_label_dir(testroot + "_" + datetime, "")

#
# Program functions for every main test mode
#

def run_default_test(testdir, mccoderoot, limit, instrfilter, compfilter, suffix):
    ''' tests the default mccode version '''

    # get default/system version number
    logger = LineLogger()
    utils.run_subtool_to_completion("%s --version" % (mccode_config.configuration["MCRUN"]), stdout_cb=logger.logline)
    try:
        version = logger.lst[-1].strip()
    except:
        logging.info("no 'mcstas --version' output, try using --config='some directory' " + mccode_config.configuration["MCCODE"])
        quit(1)

    # create single-run test directory
    labeldir = create_label_dir(testdir, mccode_config.configuration["MCCODE"] + "-" + version + suffix)

    logging.info("Testing: %s" % version)
    logging.info("")

    (results, failed, num_compilefail, num_runfail, num_valfail, num_noexample) = mccode_test(mccoderoot, labeldir, limit, instrfilter, compfilter)

    reportfile = os.path.join(labeldir, "testresults_%s.json" % (mccode_config.configuration["MCCODE"]+"-"+version+suffix))
    open(os.path.join(reportfile), "w").write(json.dumps(results, indent=2))

    logging.debug("")
    logging.debug("Test results written to: %s" % reportfile)
    print("======================================")
    print("Overall test result:")
    if (failed):
        if (not permissive):
            if (not strict):
                print("FAILED! One or more tests errored (%d compile errs / %d runtime errs / %d values off)" % (num_compilefail, num_runfail, num_valfail) )
                exit(-1)
            else:
                print("FAILED! One or more tests errored (%d compile errs / %d runtime errs / %d values off / %d missing %%Example(s))" % (num_compilefail, num_runfail, num_valfail, num_noexample) )
                exit(-1)
        else:
            print("Failures reported but tool was run in permissive mode (%d compile errs / %d runtime errs / %d values off)" % (num_compilefail, num_runfail, num_valfail) )
    else:
        print("SUCCESS")

def run_config_test(testdir, mccoderoot, limit, configfilter, instrfilter, compfilter, suffix):
    '''
    Test a suite of configs, each a directory holding a mccode_config.json file. Every
    selected config is tested in turn by passing its directory to mcrun via
    --override-config, so the compiler, C-flags, MPI setup etc. of that config are used.
    '''

    def extract_config_mccode_version(configfile):
        ''' (MCCODE_VERSION, label) of a mccode_config.json, or None '''
        label = os.path.basename(os.path.dirname(configfile))
        text = open(configfile, encoding='utf-8').read()
        try:
            version = json.loads(text).get("configuration", {}).get("MCCODE_VERSION")
        except ValueError:
            m = re.search(r'"MCCODE_VERSION"\s*:\s*"([^"]*)"', text)
            version = m.group(1) if m else None
        return (version, label) if version else None

    def get_config_files(configfltr):
        ''' mccode_config.json files to test: configfltr is an absolute path (to a config
        directory or its mccode_config.json), a label (subdirectory of the MCCODE-test
        folder next to this script) or a regex matched against those label names '''
        lookin = join(os.path.dirname(__file__), mccode_config.configuration["MCCODE"] + "-test")
        for cand in (configfltr, join(configfltr, 'mccode_config.json'), join(lookin, configfltr, 'mccode_config.json')):
            if os.path.isabs(cand) and os.path.basename(cand) == 'mccode_config.json' and os.path.isfile(cand):
                return [cand]
        found = []
        for (dirpath, _, files) in os.walk(lookin):
            if 'mccode_config.json' in files and re.search(configfltr, os.path.basename(dirpath)):
                found.append(join(dirpath, 'mccode_config.json'))
        return sorted(found)

    configfiles = get_config_files(configfilter)
    if not configfiles:
        logging.info("No mccode_config.json found for --config=%s" % configfilter)
        quit(1)

    # test labels loop
    anyfailed = False
    for f in configfiles:
        found = extract_config_mccode_version(f)
        if not found:
            logging.info("No MCCODE_VERSION in %s, skipping" % f)
            anyfailed = True
            continue
        [version,label] = found

        oldpath = activate_mccode_version(mccoderoot)
        try:
            logging.info("")
            configdir = os.path.dirname(f)   # passed on to mcrun --override-config
            label = label + suffix           # suffix already ends in _<ncount>_<platform>_<uid>
            logging.info("Testing label: %s" % label)

            # create the proper test dir
            labeldir = create_label_dir(testdir, label)
            results, failed, num_compilefail, num_runfail, num_valfail, num_noexample = mccode_test(mccoderoot, labeldir, limit, instrfilter, compfilter, configdir)

            # write local test result
            reportfile = os.path.join(labeldir, "testresults_%s.json" % (os.path.basename(labeldir)))
            open(os.path.join(reportfile), "w").write(json.dumps(results, indent=2))

            logging.debug("")
            logging.debug("Test results written to: %s" % reportfile)
            counts = "%d compile errs / %d runtime errs / %d values off" % (num_compilefail, num_runfail, num_valfail)
            if strict:
                counts = counts + " / %d missing %%Example(s)" % num_noexample
            print("%s: %s (%s)" % (label, "FAILED" if failed else "SUCCESS", counts))
            anyfailed = anyfailed or failed
        finally:
            deactivate_mccode_version(oldpath)

    print("======================================")
    print("Overall test result:")
    if anyfailed and not permissive:
        print("FAILED! One or more configs errored")
        exit(-1)
    print("Failures reported but tool was run in permissive mode" if anyfailed else "SUCCESS")



ncount = None
mpi = None
openacc = None
suffix = None
nexus = None
lint = None
permissive = None
runLocal = None
runmax = None
compilemax = None
displaymax = None
noplots = None

def main(args):
    configfilter = args.config      # test only config matching this label (default: as installed)

    # modifying options
    verbose = args.verbose          # display more info during runs
    testdir = args.testdir          # use non-default test output location
    mccoderoot = args.mccoderoot    # use non-default mccode system install location
    limit = args.limit              # only test the first [limit] instruments (useful for debugging purposes)
    instrfilter = args.instr        # test only matching instrs
    compfilter = args.comp          # test only instrs including comp
    suffix=""

    # set modifications first
    if verbose:
        logging.basicConfig(level=logging.DEBUG, format="%(message)s")
    else:
        logging.basicConfig(level=logging.INFO, format="%(message)s")

    if not testdir:
        testdir='.'

    logging.info("Output of test will be placed in: %s" % testdir)

    if not mccoderoot:
        # Figure out "mccoderoot" location from calling local mc/mcxrunxs
        if shutil.which(mccode_config.configuration["MCRUN"]) is not None:
            if (verbose):
                logging.info("Probing " + mccode_config.configuration["MCRUN"] + " --showcfg=resourcedir for 'mccoderoot'")
            metalog = LineLogger()
            try:
                utils.run_subtool_to_completion(mccode_config.configuration["MCRUN"] + " --showcfg=resourcedir", stdout_cb=metalog.logline)
                mccoderoot=metalog.lst[0]
            except:
                logging.info("Probe using mcrun/mxrun --showcfg=resourcedir failed. Next attempt using env var...")
        # Probe environment variable
        MCCODE = mccode_config.configuration["MCCODE"].upper()
        if os.environ[MCCODE] is not None:
            if (verbose):
                logging.info("Probing " + MCCODE + " env var for 'mccoderoot'")
            mccoderoot=os.environ[MCCODE]
        # Fallback attempt
        if not mccoderoot:
            logging.info("Using fallback value /usr/share/mcstas for 'mccoderoot'")
            mccoderoot = "/usr/share/mcstas/"
    if not os.path.exists(mccoderoot):
        logging.info("mccoderoot does not exist")
        quit(1)
    logging.debug("Using mccode root:       %s" % mccoderoot)
    if limit:
        try:
            limit = int(args.limit[0])
        except:
            logging.info("--limit must be a number")
            quit(1)
    logging.debug("")

    global ncount, no_mpi, mpi, skipnontest, openacc, nexus, lint, permissive, runLocal, compilemax, displaymax, runmax, seed, strict, noplots
    ncount = "1e6"
    no_mpi = False
    if args.ncount:
        ncount = args.ncount[0]
    elif args.n:
        ncount = args.n[0]

    seed = "1000"
    if args.seed:
        seed = args.seed[0]
    elif args.s:
        seed = args.s[0]

    if seed == "0" or seed == "NULL":
        # Emulate McCode 'epoch' seed, however applied to all active tests / sim runs...
        seed = round((datetime.now() - datetime(1970, 1, 1)).total_seconds())

    if args.local:
        runLocal = args.local

    # Check for collision between testdir and mccoderoot / runlocal
    if args.local:
        if paths_overlap(pathlib.Path(args.local),pathlib.Path(testdir)):
            logging.info("Local input path %s and output path %s overlap. This is not allowed!" % (args.local, testdir))
            quit(1)
        # Check for mccode.sim in local dir and raise a warning if found:
        matches = list(pathlib.Path(args.local).rglob("mccode.sim"))
        if len(matches):
            logging.info("Local input path %s contains mccode.sim data (extra input files?) This may lead to errors/warnings..." % args.local)

    else:
        if paths_overlap(pathlib.Path(mccoderoot),pathlib.Path(testdir)):
            logging.info("MCCODE root dir %s and output path %s overlap. This is not allowed!" % (mccoderoot, testdir))
            quit(1)
        # Check for mccode.sim in mccoderoot and raise a warning if found:
        matches = list(pathlib.Path(mccoderoot).rglob("mccode.sim"))
        if len(matches):
            logging.info("MCCODE root dir %s contains mccode.sim data (extra input files?) This may lead to errors/warnings..." % mccoderoot)

    if instrfilter:
        isuffix=instrfilter.replace(',', '_')
        suffix = '_' + isuffix

    if compfilter:
        compfilter=str(compfilter[0])
        suffix = suffix + '_' + compfilter.replace(',', '_')

    # filters may be regexes: keep the label directory name filesystem-safe
    suffix = re.sub(r'[^\w.+-]', '_', suffix)

    if args.suffix:
        if (len(suffix)<30):
            suffix = suffix + '_' + args.suffix[0]
        else:
            suffix = '_' + args.suffix[0]

    if not args.uid:
        uid = "_" + utils.get_datetimestr()
    else:
        uid = "_" + args.uid[0]

    suffix=suffix + "_" + ncount + "_" + platform.system() + uid
    if runLocal:
        suffix = suffix + '_LOCAL'

    logging.info("ncount is: %s" % ncount)

    if args.no_mpi:
        args.mpi=None
        no_mpi = True
        logging.info("Disable MPI compilation")

    if args.mpi:
        no_mpi = False
        mpi = args.mpi[0]
        logging.info("mpi count is: %s" % mpi)
        suffix = '_mpi_x_' + str(mpi) + suffix
    if args.openacc:
        openacc = True
        logging.info("openacc is enabled")
        suffix = '_openacc' + suffix
    if args.nexus:
        nexus = True
        suffix = '_NeXus' + suffix
        logging.info("NeXus compilation / output format is enabled")
    if args.lint:
        lint = True
        suffix = '_lint' + suffix
        logging.info("c-linting enabled")
    if args.runmax:
        runmax=int(args.runmax[0])
    else:
        runmax=3600
    if args.compilemax:
        compilemax=int(args.compilemax[0])
    else:
        compilemax=1800
    if lint:
        compilemax=100*compilemax
    if args.displaymax:
        displaymax=int(args.displaymax[0])
    else:
        displaymax=60

    if args.strict and args.permissive:
        logging.error("ERROR: Permissive mode and strict mode can not be combined!")
        exit(-1)

    if args.permissive:
        permissive = True
        logging.info("Permissive mode, tool will not report failure on failed instruments")

    strict = False
    if args.strict:
        strict = True
        logging.info("Strict mode, tool will report failure for instruments without %Example")

    noplots = args.noplots
    if noplots:
        logging.info("No plots of the test output will be generated")

    if not configfilter:
        run_default_test(testdir, mccoderoot, limit, instrfilter, compfilter, suffix)
    else:
        run_config_test(testdir, mccoderoot, limit, configfilter, instrfilter, compfilter, suffix)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--ncount', nargs=1, help='ncount sent to %s' % (mccode_config.configuration["MCRUN"]) )
    parser.add_argument('-n', nargs=1, help='ncount sent to %s' % (mccode_config.configuration["MCRUN"]) )
    parser.add_argument('--seed', nargs=1, help='seed sent to %s (default 1000 - use 0/NULL to "randomize")' % (mccode_config.configuration["MCRUN"]) )
    parser.add_argument('-s', nargs=1, help='seed sent to %s (default 1000 - use 0/NULL to "randomize")' % (mccode_config.configuration["MCRUN"]) )
    parser.add_argument('--mpi', nargs=1, help='mpi nodecount sent to %s' % (mccode_config.configuration["MCRUN"]) )
    parser.add_argument('--no-mpi', action='store_true', help='MPI compilation disabled via %s --no-mpi' % (mccode_config.configuration["MCRUN"]) )
    parser.add_argument('--openacc', action='store_true', help='openacc flag sent to %s' % (mccode_config.configuration["MCRUN"]))
    parser.add_argument('--config', nargs="?", help='test this specific config only - label name (regex) or absolute path')
    parser.add_argument('--instr', nargs="?", help='test only intruments matching this filter (py regex). Comma-separated list allowed for multiple filters. Combined with --comp, instruments matching either are tested.')
    parser.add_argument('--comp', nargs=1, help='test only intruments utilising COMP (whole-word match). Comma-separated list allowed. Useful for testing the instrument suite after component changes.')
    parser.add_argument('--mccoderoot', nargs='?', help='manually select root search folder for mccode installations')
    parser.add_argument('--testdir', nargs='?', help='output test results directly in this dir (default CWD). Used testdir and --local path can not overlap!')
    parser.add_argument('--limit', nargs=1, help='test only the first [LIMIT] instrs')
    parser.add_argument('--verbose', action='store_true', help='output a test/notest instrument status header before each test')
    parser.add_argument('--skipnontest', action='store_true', help='Skip compilation of instruments without a test')
    parser.add_argument('--suffix', nargs=1, help='Add suffix to test directory name, e.g. 3.x-dev_suffix')
    parser.add_argument('--uid', nargs=1, help='Unique identifier for suffix, e.g. CI worker id (if unset a timestamp is used)')
    parser.add_argument('--nexus', action='store_true', help='Compile for / use NeXus output format everywhere')
    parser.add_argument('--lint', action='store_true', help='Just run the c-linter')
    parser.add_argument('--compilemax', nargs=1, help='Maximum time (s) allowed pr. compilation (default 1800s)(if run with --lint muliplied x100)')
    parser.add_argument('--runmax', nargs=1, help='Maximum time (s) allowed pr. test Example run (default 3600s)')
    parser.add_argument('--displaymax', nargs=1, help='Maximum time allowed pr. test Example DISPLAY run (default 60s)')
    parser.add_argument('--permissive', action='store_true', help='Use zero return-value even if some tests fail. Useful for full test con systems that are only partially functional. Can not be combined with --strict.')
    parser.add_argument('--strict', action='store_true', help='Let instruments without %%Example line(s) instantly fail. Can not be combined with --permissive.')
    parser.add_argument('--noplots', action='store_true', help='Do not generate plots (01_overview.pdf and 02_plots.html) of the test output. Useful e.g. in CI, where the plots are not looked at, and can take long for instruments with many monitors.')
    parser.add_argument('--local', help='Instruments to test are NOT picked up from MCCODE installation, instead from --local=DIR. Local path and --testdir can not overlap!')
    args = parser.parse_args()

    try:
        main(args)
    except KeyboardInterrupt:
        print()

