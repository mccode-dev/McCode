#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
Generates html docpages from mccode instrument and component files.

A docpage is generated for every instrument and component file, and an 
overview page is written and browsed. Default option Read installed docpage.

Specify a directory to add local results, and a search term for filtered or 
specific file results.
'''
import logging
import argparse
import sys
import os
import re
import subprocess
from os.path import join, basename
import pathlib

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from mccodelib import utils, mccode_config

def get_html_filepath(filepath):
    ''' transform from .anything to .html '''
    h = pathlib.Path(os.path.splitext(filepath)[0] + '.html')
    h = pathlib.Path(str(h).replace(mccode_config.directories['resourcedir'], mccode_config.directories['docdir']))
    return h

class OverviewDocWriter:
    ''' Creates the mcdoc overview html page. '''
    def __init__(self, comp_info_lst, instr_info_lst, comp_info_local_lst, instr_info_local_lst, mccode_libdir, mccode_docdir):
        self.comp_info_lst = comp_info_lst
        self.instr_info_lst = instr_info_lst
        self.comp_info_local_lst = comp_info_local_lst
        self.instr_info_local_lst = instr_info_local_lst
        self.mccode_libdir = mccode_libdir
        self.mccode_docdir = mccode_docdir
        self.text = ''
    
    def create(self):
        ''' action code for create overview page! '''
        i_lst = self.instr_info_lst
        c_loc_lst = self.comp_info_local_lst
        i_loc_lst = self.instr_info_local_lst
        t = self.tab_line
        
        # create comp tables
        #'%TAB_LINES_SOURCES%', '%TAB_LINES_OPTICS%', '%TAB_LINES_SAMPLES%', '%TAB_LINES_MONITORS%', '%TAB_LINES_MISC%', '%TAB_LINES_OBSOLETE%',
        # Sources
        sources_lst = [c for c in self.comp_info_lst if c.category=='sources']
        sources_tab = ''
        for c in sources_lst:
            sources_tab = sources_tab + t % (get_html_filepath(c.filepath), c.name, c.origin, c.author, c.filepath, 'comp', c.short_descr) + '\n'
        # Optics
        optics_lst = [c for c in self.comp_info_lst if c.category=='optics']
        optics_tab = ''
        for c in optics_lst:
            optics_tab = optics_tab + t % (get_html_filepath(c.filepath), c.name, c.origin, c.author, c.filepath, 'comp', c.short_descr) + '\n'
        # Samples
        samples_lst = [c for c in self.comp_info_lst if c.category=='samples']
        samples_tab = ''
        for c in samples_lst:
            samples_tab = samples_tab + t % (get_html_filepath(c.filepath), c.name, c.origin, c.author, c.filepath, 'comp', c.short_descr) + '\n'
        # Detectors
        monitors_lst = [c for c in self.comp_info_lst if c.category=='monitors']
        monitors_tab = ''
        for c in monitors_lst:
            monitors_tab = monitors_tab + t % (get_html_filepath(c.filepath), c.name, c.origin, c.author, c.filepath, 'comp', c.short_descr) + '\n'
        # Misc
        misc_lst = [c for c in self.comp_info_lst if c.category=='misc']
        misc_tab = ''
        for c in misc_lst:
            misc_tab = misc_tab + t % (get_html_filepath(c.filepath), c.name, c.origin, c.author, c.filepath, 'comp', c.short_descr) + '\n'
        # Union
        union_lst = [c for c in self.comp_info_lst if c.category=='union']
        union_tab = ''
        for c in union_lst:
            union_tab = union_tab + t % (get_html_filepath(c.filepath), c.name, c.origin, c.author, c.filepath, 'comp', c.short_descr) + '\n'
        # SASmodels
        sasmodels_lst = [c for c in self.comp_info_lst if c.category=='sasmodels']
        sasmodels_tab = ''
        for c in sasmodels_lst:
            sasmodels_tab = sasmodels_tab + t % (get_html_filepath(c.filepath), c.name, c.origin, c.author, c.filepath, 'comp', c.short_descr) + '\n'
        # Astrox
        astrox_lst = [c for c in self.comp_info_lst if c.category=='astrox']
        astrox_tab = ''
        for c in astrox_lst:
            astrox_tab = astrox_tab + t % (get_html_filepath(c.filepath), c.name, c.origin, c.author, c.filepath, 'comp', c.short_descr) + '\n'
        # Contributed
        contrib_lst = [c for c in self.comp_info_lst if c.category=='contrib']
        contrib_tab = ''
        for c in contrib_lst:
            contrib_tab = contrib_tab + t % (get_html_filepath(c.filepath), c.name, c.origin, c.author, c.filepath, 'comp', c.short_descr) + '\n'
        # Obsolete
        obsolete_lst = [c for c in self.comp_info_lst if c.category=='obsolete']
        obsolete_tab = ''
        for c in obsolete_lst:
            obsolete_tab = obsolete_tab + t % (get_html_filepath(c.filepath), c.name, c.origin, c.author, c.filepath, 'comp', c.short_descr) + '\n'
        
        # create instr examples table
        ex_tab = ''
        for i in i_lst:
            ex_tab = ex_tab + t % (get_html_filepath(i.filepath), i.name, i.origin, i.author, i.filepath, 'instr', i.short_descr) + '\n'

        # create local instrs / comps table
        local_comp_tab = ''
        for c in c_loc_lst:
            local_comp_tab  = local_comp_tab + t % (get_html_filepath(c.filepath), c.name, c.origin, c.author, c.filepath, 'comp', c.short_descr) + '\n'
        
        local_instr_tab = ''
        for i in i_loc_lst:
            local_instr_tab = local_instr_tab + t % (get_html_filepath(i.filepath), i.name, i.origin, i.author, i.filepath, 'instr', i.short_descr) + '\n'

        text = self.html

        # First, check if this is McXtrace (including AstroX) or McStas
        if (mccode_config.get_mccode_prefix() == 'mx'):
            astrox_hdr = ''' | <A href="#astrox">astrox</A>'''
            astrox_table = '''
<P><A NAME="astrox"></A>
<B><FONT COLOR="#FF0000">AstroX components</FONT></B>
<TABLE BORDER COLS=5 WIDTH="100%" NOSAVE>

%TAB_HEAD%

%TAB_LINES_ASTROX%

</TABLE>
'''
            text = text.replace('%HDR_ASTROX%', astrox_hdr)
            text = text.replace('%TABLE_ASTROX%', astrox_table)
            text = text.replace('%TAB_LINES_ASTROX%', astrox_tab)
        else:
            text = text.replace('%HDR_ASTROX%', '')
            text = text.replace('%TABLE_ASTROX%', '')

        text = text.replace('%MCCODE_LIBDIR%', self.mccode_libdir)
        text = text.replace('%MCCODE_DOCDIR%', self.mccode_docdir)
        text = text.replace('%TAB_HEAD%', self.tab_header)
        text = text.replace('%TAB_LINES_SOURCES%', sources_tab)
        text = text.replace('%TAB_LINES_OPTICS%', optics_tab)
        text = text.replace('%TAB_LINES_SAMPLES%', samples_tab)
        text = text.replace('%TAB_LINES_MONITORS%', monitors_tab)
        text = text.replace('%TAB_LINES_UNION%', union_tab)
        text = text.replace('%TAB_LINES_SASMODELS%', sasmodels_tab)
        text = text.replace('%TAB_LINES_MISC%', misc_tab)
        text = text.replace('%TAB_LINES_CONTRIB%', contrib_tab)
        text = text.replace('%TAB_LINES_OBSOLETE%', obsolete_tab)
        text = text.replace('%TAB_LINES_EXAMPLES%', ex_tab)
        text = text.replace('%TAB_LINES_COMPS_LOCAL%', local_comp_tab)
        text = text.replace('%TAB_LINES_INSTR_LOCAL%', local_instr_tab)
        text = text.replace('%LINK_FILECOLON_DATA%', 'file://%s/data' % self.mccode_libdir)
        text = text.replace('%LINK_FILECOLON_SHARE%', 'file://%s/share' % self.mccode_libdir)
        text = text.replace('%VERSION%', '%s %s' % (mccode_config.configuration["MCCODE"],
                                                    mccode_config.configuration["MCCODE_VERSION"]))

        #some McXtrace specific edits
        if (mccode_config.get_mccode_prefix() == 'mx'):
          text = text.replace('McStas','McXtrace')
          text = text.replace('mcstas','mcxtrace')
          test = text.replace('MCSTAS','MCXTRACE')

        self.text = text
        return self.text
    
    tab_header = '''
<TR>
<TD><B><I>Name</I></B></TD>
<TD WIDTH="10%"><B><I>Origin</I></B></TD>
<TD WIDTH="10%"><B><I>Author(s)</I></B></TD>
<TD><B><I>Source code</I></B></TD>
<TD><B><I>Description</I></B></TD>
</TR>
'''
    tab_line_items = ['%DOCFILE%', '%NAME%', '%ORIGIN%', '%AUTHOR%', '%SOURCEFILE%', '%COMP_OR_INSTR%', '%DOC%']
    tab_line = '''
<TR>
<TD><B><A HREF="%s">%s</A></B></TD>
<TD>%s</TD>
<TD>%s</TD>
<TD><A HREF="%s">%s</A></TD>
<TD>%s</TD>
</TR>
'''
    tags = ['%MCCODE_LIBDIR%',
            '%HDR_ASTROX%'
            '%TAB_HEAD%',
            '%TAB_LINES_SOURCES%',
            '%TAB_LINES_OPTICS%',
            '%TAB_LINES_SAMPLES%',
            '%TAB_LINES_MONITORS%',
            '%TAB_LINES_CONTRIB%',
            '%TAB_LINES_UNION%',
            '%TAB_LINES_SASMODELS%',
            '%TABLE_ASTROX%',
            '%TAB_LINES_MISC%',
            '%TAB_LINES_OBSOLETE%',
            '%TAB_LINES_EXAMPLES%',
            '%TAB_LINES_LOCAL%',
            '%LINK_FILECOLON_DATA%',
            '%LINK_FILECOLON_SHARE%',
            '%VERSION%']
    html = '''
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2//EN">
<HTML>
<HEAD>
   <META NAME="GENERATOR" CONTENT="McDoc">
   <TITLE>McStas : Components/Instruments Library </TITLE>
</HEAD>
<BODY>

<P ALIGN=CENTER>
 [ <A href="#sources">sources</A>
 | <A href="#optics">optics</A>
 | <A href="#samples">samples</A>
 | <A href="#monitors">monitors</A>
 | <A href="#union">union</A>
 | <A href="#sasmodels">sasmodels</A>
%HDR_ASTROX%
 | <A href="#misc">misc</A>
 | <A href="#contrib">contrib</A>
 | <A href="#obsolete">obsolete</A>
 | <A href="#examples">examples</A>
 | <A href="#localcomps">local comps</A>
 | <A href="#localinstrs">local instruments</A>
 | <A href="%LINK_FILECOLON_DATA%">data</A>
 | <A href="%LINK_FILECOLON_SHARE%">share</A> ]
</P>
<P ALIGN=CENTER>
[ <a href="file://%MCCODE_DOCDIR%/mcstas-manual.pdf">User Manual</a>
| <a href="file://%MCCODE_DOCDIR%/mcstas-components.pdf">Component Manual</a> ]
| <a href="file://%MCCODE_LIBDIR%/">McCode lib dir</a> ]
</P>

<CENTER><H1>Components and Instruments from the Library for <i>McStas</i></H1></CENTER>

<P><A NAME="sources"></A>
<B><FONT COLOR="#FF0000">Sources</FONT></B>
<TABLE BORDER COLS=5 WIDTH="100%" NOSAVE>

%TAB_HEAD%

%TAB_LINES_SOURCES%

</TABLE>


<P><A NAME="optics"></A>
<B><FONT COLOR="#FF0000">Optics</FONT></B>
<TABLE BORDER COLS=5 WIDTH="100%" NOSAVE>

%TAB_HEAD%

%TAB_LINES_OPTICS%

</TABLE>


<P><A NAME="samples"></A>
<B><FONT COLOR="#FF0000">Samples</FONT></B>
<TABLE BORDER COLS=5 WIDTH="100%" NOSAVE>

%TAB_HEAD%

%TAB_LINES_SAMPLES%

</TABLE>


<P><A NAME="monitors"></A>
<B><FONT COLOR="#FF0000">Detectors and monitors</FONT></B>
<TABLE BORDER COLS=5 WIDTH="100%" NOSAVE>

%TAB_HEAD%

%TAB_LINES_MONITORS%

</TABLE>

<P><A NAME="union"></A>
<B><FONT COLOR="#FF0000">Union components</FONT></B>
<TABLE BORDER COLS=5 WIDTH="100%" NOSAVE>

%TAB_HEAD%

%TAB_LINES_UNION%

</TABLE>

<P><A NAME="sasmodels"></A>
<B><FONT COLOR="#FF0000">SASmodels components</FONT></B>
<TABLE BORDER COLS=5 WIDTH="100%" NOSAVE>

%TAB_HEAD%

%TAB_LINES_SASMODELS%

</TABLE>

%TABLE_ASTROX%

<P><A NAME="misc"></A>
<B><FONT COLOR="#FF0000">Misc</FONT></B>
<TABLE BORDER COLS=5 WIDTH="100%" NOSAVE>

%TAB_HEAD%

%TAB_LINES_MISC%

</TABLE>


<P><A NAME="contrib"></A>
<B><FONT COLOR="#FF0000">Contributed components</FONT></B>
<TABLE BORDER COLS=5 WIDTH="100%" NOSAVE>

%TAB_HEAD%

%TAB_LINES_CONTRIB%

</TABLE>


<P><A NAME="obsolete"></A>
<B><FONT COLOR="#FF0000">Obsolete (avoid usage whenever possible)</FONT></B>
<TABLE BORDER COLS=5 WIDTH="100%" NOSAVE>

%TAB_HEAD%

%TAB_LINES_OBSOLETE%

</TABLE>


<P><A NAME="examples"></A>
<B><FONT COLOR="#FF0000">Instrument Examples</FONT></B>
<TABLE BORDER COLS=5 WIDTH="100%" NOSAVE>

%TAB_HEAD%

%TAB_LINES_EXAMPLES%

</TABLE>


<P><A NAME="localcomps"></A>
<B><FONT COLOR="#FF0000">Local components</FONT></B>
<TABLE BORDER COLS=5 WIDTH="100%" NOSAVE>

%TAB_HEAD%

%TAB_LINES_COMPS_LOCAL%

</TABLE>


<P><A NAME="localinstrs"></A>
<B><FONT COLOR="#FF0000">Local instruments</FONT></B>
<TABLE BORDER COLS=5 WIDTH="100%" NOSAVE>

%TAB_HEAD%

%TAB_LINES_INSTR_LOCAL%

</TABLE>


<HR WIDTH="100%">
<CENTER>
[ <A href="http://www.mcstas.org/"><I>McStas web site</I></A> ]
</CENTER>

<P><BR>
<ADDRESS>
Generated for %VERSION%
</ADDRESS>
</BODY>
</HTML>
'''

class InstrParser:
    ''' parses an instr or comp file, extracting all relevant information into python '''
    def __init__(self, filename):
        self.filename = filename
        self.info = None
        self.has_parsed = False
    
    def stub(self):
        ''' fallback parsing '''
        self.info = utils.InstrCompHeaderInfo()
        self.has_parsed = True
        return self.info
    
    def parse(self):
        ''' parses the given file '''
        f = open(self.filename)
        logging.debug('parsing file "%s"' % self.filename)
        
        header = utils.read_header(f)
        info = utils.parse_header(header)
        info.site = utils.get_instr_site_fromtxt(header)
        dfine = utils.read_define_instr(f)
        info.name, info.params = utils.parse_define_instr(dfine)
        
        self.info = info
        return self.info


class InstrDocWriter:
    ''' create html doc text by means of a instr parser '''
    def __init__(self, info):
        self.info = info
        self.text = ''

    def create(self):
        i = self.info
        t = self.tags
        h = self.html

        #some McXtrace specific edits
        if (mccode_config.get_mccode_prefix() == 'mx'):
          h = h.replace('McStas','McXtrace')
          h = h.replace('mcstas','mcxtrace')
          h = h.replace('MCSTAS','MCXTRACE')

        h = h.replace(t[0], i.name)
        h = h.replace(t[1], i.name)
        # If parsing failed to find a short description, use 'name'
        if re.match(r'^\s*\n*\r*$', i.short_descr):
            i.short_descr=i.name + ' instrument';
        h = h.replace(t[2], i.short_descr)
        h = h.replace(t[3], i.site)
        h = h.replace(t[4], i.author)
        h = h.replace(t[5], i.origin)
        h = h.replace(t[6], i.date)
        h = h.replace(t[7], i.description)

        h = h.replace(t[8], self.par_header)
        doc_rows = ''

        for p in i.params:
            unit = [pd[1] for pd in i.params_docs if p[1] == pd[0]]
            unit = unit[0] if len(unit) > 0 else ''
            defval = p[2] if p[2] != None else ''
            doc = [pd[2] for pd in i.params_docs if p[1] == pd[0]] # TODO: rewrite to speed up 
            doc = doc[0] if len(doc) > 0 else ''
            if defval == '':
                doc_rows = doc_rows + '\n' + self.par_str_boldface % (p[1], unit, doc, defval)
            else:
                doc_rows = doc_rows + '\n' + self.par_str % (p[1], unit, doc, defval)
        h = h.replace(t[9], doc_rows)

        h = h.replace(t[10], i.filepath)
        h = h.replace(t[11], os.path.basename(i.filepath))
        
        # TODO: implement links writing
        lstr = ''
        for l in i.links:
            lstr = lstr + self.lnk_str % l + '\n'
        h = h.replace(t[12], lstr)
        
        h = h.replace(t[13], '%s %s' % (mccode_config.configuration["MCCODE"],
                                        mccode_config.configuration["MCCODE_VERSION"]))

        self.text = h
        return self.text

    tags = ['%TITLE%',
            '%INSTRNAME%',
            '%SHORT_DESCRIPTION%',
            '%SITE%',
            '%AUTHOR%',
            '%ORIGIN%',
            '%DATE%',
            '%DESCRIPTION%',
            '%T_HEAD%',
            '%T_ROWS%',
            '%INSTRFILE%',
            '%INSTRFILE_BASE%',
            '%LINKS%',
            '%VERSION%']
    par_str = "<TR> <TD>%s</TD><TD>%s</TD><TD>%s</TD><TD ALIGN=RIGHT>%s</TD></TR>"
    par_str_boldface = "<TR> <TD><strong>%s</strong></TD><TD>%s</TD><TD>%s</TD><TD ALIGN=RIGHT>%s</TD></TR>"
    par_header = par_str % ('<strong>Name</strong>', '<strong>Unit</strong>', '<strong>Description</strong>', '<strong>Default</strong>')
    lnk_str = "<LI>%s"

    html = '''
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2//EN">
<HTML><HEAD>
<TITLE>McStas: %TITLE%</TITLE>
<LINK REV="made" HREF="mailto:pkwi@fysik.dtu.dk">
</HEAD>

<BODY>

<script src="https://tex.mccode.org/jquery/latest/jquery.min.js"></script>
<script src="https://tex.mccode.org/jquery/latex/jquery.jslatex.packed.js"></script>
<script>
$(function () {
    $(".latex").latex();
});
</script>

<P ALIGN=CENTER>
 [ <A href="#id">Identification</A>
 | <A href="#desc">Description</A>
 | <A href="#ipar">Input parameters</A>
 | <A href="#links">Links</A> ]
</P>

<H1>The <CODE>%INSTRNAME%</CODE> Instrument</H1>

%SHORT_DESCRIPTION%

<H2><A NAME=id></A>Identification</H2>

<UL>
  <LI> <B>Site: </B>%SITE%
  <LI> <B>Author: </B>%AUTHOR%
  <LI> <B>Origin: </B>%ORIGIN%
  <LI> <B>Date: </B>%DATE%
</UL>
<H2><A NAME=desc></A>Description</H2>

<PRE>
%DESCRIPTION%
</PRE>

<H2><A NAME=ipar></A>Input parameters</H2>
Parameters in <B>boldface</B> are required;
the others are optional.

<TABLE BORDER=1>
%T_HEAD%
%T_ROWS%
</TABLE>

<H2><A NAME=links></A>Links</H2>

<UL>
  <LI> <A HREF="%INSTRFILE%">Source code</A> for <CODE>%INSTRFILE_BASE%</CODE>.
  %LINKS%
</UL>
<HR>
<P ALIGN=CENTER>
 [ <A href="#id">Identification</A>
 | <A href="#desc">Description</A>
 | <A href="#ipar">Input parameters</A>
 | <A href="#links">Links</A> ]
</P>

<ADDRESS>
Generated for %VERSION%
</ADDRESS>
</BODY></HTML>
'''


class CompParser(InstrParser):
    def parse(self):
        ''' override '''
        f = open(self.filename)
        logging.debug('parsing file "%s"' % self.filename)
        
        header = utils.read_header(f)
        info = utils.parse_header(header)
        info.site = utils.get_instr_site_fromtxt(header)
        
        dfine = utils.read_define_comp(f)
        name, setpar, defpar, outpar = utils.parse_define_comp(dfine)
        
        info.name = name
        info.category = utils.get_comp_category(self.filename)
        
        # basically just for debug use
        info.params = setpar + defpar + outpar
        
        # these are used by CompDocWriter
        info.setparams = setpar
        info.defparams = defpar
        info.outparams = outpar
        
        self.info = info
        return self.info

class CompDocWriter:
    ''' create html doc text by means of a instr parser '''
    def __init__(self, info):
        self.info = info
        self.text = ''
    
    def create(self):
        i = self.info
        t = self.tags
        h = self.html
        #some McXtrace specific edits
        if (mccode_config.get_mccode_prefix() == 'mx'):
          h = h.replace('McStas','McXtrace')
          h = h.replace('mcstas','mcxtrace')
          h = h.replace('MCSTAS','MCXTRACE')

        h = h.replace(t[0], i.name)
        h = h.replace(t[1], i.name)
        # If parsing failed to find a short description, use 'name'
        if re.match(r'^\s*\n*\r*$', i.short_descr):
            i.short_descr=i.name + ' component';
        h = h.replace(t[2], i.short_descr)
        h = h.replace(t[3], i.site)
        h = h.replace(t[4], i.author)
        h = h.replace(t[5], i.origin)
        h = h.replace(t[6], i.date)
        h = h.replace(t[7], i.description)

        h = h.replace(t[8], self.par_header)

        parstr=""
        first=1

        doc_rows_in = ''
        for p in i.setparams + i.defparams:
            if (not first):
                parstr = parstr + ', "' + p[1] + '"'
            else:
                first=0
                parstr = parstr + '"' + p[1] + '"'
            unit = [pd[1] for pd in i.params_docs if p[1] == pd[0]]
            unit = unit[0] if len(unit) > 0 else ''
            defval = p[2] if p[2] != None else ''
            doc = [pd[2] for pd in i.params_docs if p[1] == pd[0]]
            doc = doc[0] if len(doc) > 0 else ''
            if defval == '':
                doc_rows_in = doc_rows_in + '\n' + self.par_str_boldface % (p[1], unit, doc, defval, '<input type="text" value="" id="' + p[1] + '">')
            else:
                doc_rows_in = doc_rows_in + '\n' + self.par_str % (p[1], unit, doc, defval, '<input type="text" value="" id="' + p[1] + '">')
        h = h.replace(t[9], doc_rows_in)
        h = h.replace(t[14],parstr)

        h = h.replace(t[10], i.filepath)
        h = h.replace(t[11], os.path.basename(i.filepath))

        # TODO: implement links writing
        lstr = ''
        for l in i.links:
            lstr = lstr + self.lnk_str % l + '\n'
        h = h.replace(t[12], lstr)

        h = h.replace(t[13], '%s %s' % (mccode_config.configuration["MCCODE"],
                                        mccode_config.configuration["MCCODE_VERSION"]))

        self.text = h
        return self.text

    tags = ['%TITLE%',
            '%COMPNAME%',
            '%SHORT_DESCRIPTION%',
            '%SITE%',
            '%AUTHOR%',
            '%ORIGIN%',
            '%DATE%',
            '%DESCRIPTION%',
            '%T_HEAD%',
            '%T_ROWS_IN%',
            '%COMPFILE%',
            '%COMPFILE_BASE%',
            '%LINKS%',
            '%VERSION%',
            '%PARLIST%']
    par_str = "<TR> <TD>%s</TD><TD>%s</TD><TD>%s</TD><TD ALIGN=RIGHT>%s</TD><TD ALIGN=RIGHT>%s</TD></TR>"
    par_str_boldface = "<TR> <TD><strong>%s</strong></TD><TD>%s</TD><TD>%s</TD><TD ALIGN=RIGHT>%s</TD><TD ALIGN=RIGHT>%s</TD></TR>"
    par_header = par_str % ('<strong>Name</strong>', '<strong>Unit</strong>', '<strong>Description</strong>', '<strong>Default</strong>', '<input type="text" value="' + "CompInstanceName" + '" id="instance">')
    lnk_str = "<LI>%s"
    
    
    html = r'''
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2//EN">
<HTML><HEAD>
<TITLE>McStas: %TITLE%</TITLE>
<LINK REV="made" HREF="mailto:pkwi@fysik.dtu.dk">
</HEAD>

<script>

function getval(tag) {
  var ret = document.getElementById(tag);
  var val = "";
  if (ret) {
    ret.select();
    ret.setSelectionRange(0, 99999); // For mobile devices
    val=ret.value;
  }
  return val;
}

function parstr(tag,init) {
   var val = getval(tag);
   var str = "";
   if (val) {
     if (init==0) {
        str = str + ",\n";
     }
     str = str + "  " + tag + " = " + val;
   }
   return str;
}

function addpos() {
   var xpos = getval("xpos");
   var ypos = getval("ypos");
   var zpos = getval("zpos");
   var REF = getval("REF");

   var ATpos = "\) \n AT \(";
   ATpos = ATpos + xpos + ", ";
   ATpos = ATpos + ypos + ", ";
   ATpos = ATpos + zpos;
   ATpos = ATpos + "\) RELATIVE " ; 
   ATpos = ATpos + " " + REF ;
   return ATpos;
}

function addrot() {
   var xrot = getval("xrot");
   var yrot = getval("yrot");
   var zrot = getval("zrot");
   var REF2 = getval("REF2");

   var ATrot = "";

   if (xrot && yrot && zrot) { 
     ATrot = "\n ROTATED \(";
     ATrot = ATrot + xrot + ", ";
     ATrot = ATrot + yrot + ", ";
     ATrot = ATrot + zrot;
     ATrot = ATrot + "\) RELATIVE " ; 
     ATrot = ATrot + " " + REF2 ;
   }
   return ATrot;
}

function compdef(type) {
  var instance = getval("instance");
  var compstr = "COMPONENT " + instance + " = " + type +"\(\n";
  return compstr;
}

function comp() {
  // Get the text fields

  var Text = compdef("%COMPNAME%");

  var Add;

  var init = 1;

  const pars = [%PARLIST%];

  let len = pars.length;

  for (var i=0; i<len; i++) {
    Add = parstr(pars[i],init);
    if (Add) {
      init=0;
      Text = Text + Add;",\n ";
    }
  }

  Text = Text + addpos();
  Text = Text + addrot();

  // Copy the text inside the text field
  navigator.clipboard.writeText(Text);

  // Alert the copied text
  alert("Now on your clipboard: \n\n" + Text);
}
</script>

<script src="https://tex.mccode.org/jquery/latest/jquery.min.js"></script>
<script src="https://tex.mccode.org/jquery/latex/jquery.jslatex.packed.js"></script>
<script>
$(function () {
    $(".latex").latex();
});
</script>

<BODY>

<P ALIGN=CENTER>
 [ <A href="#id">Identification</A>
 | <A href="#desc">Description</A>
 | <A href="#ipar">Input parameters</A>
 | <A href="#links">Links</A> ]
</P>

<H1>The <CODE>%COMPNAME%</CODE> Component</H1>

%SHORT_DESCRIPTION%

<H2><A NAME=id></A>Identification</H2>

<UL>
  <LI> <B>Site: </B>%SITE%
  <LI> <B>Author: </B>%AUTHOR%
  <LI> <B>Origin: </B>%ORIGIN%
  <LI> <B>Date: </B>%DATE%
</UL>
<H2><A NAME=desc></A>Description</H2>

<PRE>
%DESCRIPTION%
</PRE>

<H2><A NAME=ipar></A>Input parameters</H2>
Parameters in <B>boldface</B> are required;
the others are optional.

<TABLE BORDER=1>
%T_HEAD%
%T_ROWS_IN%
</TABLE>

<TABLE BORDER=0 ALIGN=CENTER>
  <TR>
  <TH align="right">AT (</TH>
  <td><input type="text" value="0" id="xpos" maxlength="4" size="4">, </td>
  <td><input type="text" value="0" id="ypos" maxlength="4" size="4">, </td>
  <td><input type="text" value="0" id="zpos" maxlength="4" size="4"></td>
  <TH>) RELATIVE</TH>
  <td><input type="text" value="PREVIOUS" id="REF"></td>
  <td rowspan="2">
    <button style="height:50px" onclick="comp()"><strong>Generate<br>to clipboard</strong></button>
  </td>
  </TR>
  <TR>
  <TH align="right">ROTATED (</TH>
  <td><input type="text" value="" id="xrot" maxlength="4" size="4">, </td>
  <td><input type="text" value="" id="yrot" maxlength="4" size="4">, </td>
  <td><input type="text" value="" id="zrot" maxlength="4" size="4"></td>
  <TH>) RELATIVE</TH>
  <td><input type="text" value="PREVIOUS" id="REF2"></td></TR>
</TABLE>

<H2><A NAME=links></A>Links</H2>

<UL>
  <LI> <A HREF="%COMPFILE%">Source code</A> for <CODE>%COMPFILE_BASE%</CODE>.
  %LINKS%
</UL>
<HR>
<P ALIGN=CENTER>
 [ <A href="#id">Identification</A>
 | <A href="#desc">Description</A>
 | <A href="#ipar">Input parameters</A>
 | <A href="#links">Links</A> ]
</P>

<ADDRESS>
Generated on %VERSION%
</ADDRESS>
</BODY></HTML>
'''


def write_file(filename, text, failsilent=False):
    try:
        dirname = os.path.dirname(filename)
        
        if not os.path.exists(dirname):
            os.makedirs(dirname)
        f = open(filename, 'w')
        f.write(text)
        f.close()
    except Exception as e:
        if failsilent:
            pass
        else:
            raise e

def parse_and_filter(indir, namefilter=None, recursive=False, printlog=False):
    ''' read and parse headers and definitions of component and instrument files '''
    indir=pathlib.Path(indir).resolve()
    instr_files, comp_files = utils.get_instr_comp_files(indir, recursive)
    print("parsing root folder:", indir)
    comp_info_lst = []

    comp_files=sorted(comp_files)
    instr_files=sorted(instr_files)

    for f in comp_files:
        try:
            if printlog:
                print("parsing comp... %s" % f)
            info = CompParser(f).parse()
            info.filepath = os.path.abspath(f)
            comp_info_lst.append(info)
        except:
            print("failed parsing file: %s" % f)
            comp_info_lst.append(CompParser(f).stub())
    print("comp files: %s" % str(len(comp_files)))

    instr_info_lst = []
    for f in instr_files:
        try:
            if printlog:
                print("parsing instr... %s" % f)
            info = InstrParser(f).parse()
            info.filepath = os.path.abspath(f)
            instr_info_lst.append(info)
        except:
            print("failed parsing file: %s" % f)
            instr_info_lst.append(InstrParser(f).stub())
    print("instr files: %s" % str(len(instr_files)))

    if namefilter != None:
        comp_info_lst = [c for c in comp_info_lst if re.search(namefilter.lower(), c.name.lower())]
        instr_info_lst = [c for c in instr_info_lst if re.search(namefilter.lower(), c.name.lower())]
        comp_files = [f for f in comp_files if re.search(namefilter.lower(), os.path.splitext(os.path.basename(f))[0].lower())]
        instr_files = [f for f in instr_files if re.search(namefilter.lower(), os.path.splitext(os.path.basename(f))[0].lower())]

    return comp_info_lst, instr_info_lst, comp_files, instr_files

def write_doc_files_or_continue(comp_infos, instr_infos, comp_files, instr_files, printlog=False):
    ''' Writes component and instrument docs files '''
    for i in range(len(comp_infos)):
        try:
            p = comp_infos[i]
            try:
                f = comp_files[i]
            except:
                f = comp_infos[i].filepath
            doc = CompDocWriter(p)
            text = doc.create()
            h = pathlib.Path(os.path.splitext(f)[0] + '.html')
            h = pathlib.Path(str(h).replace(mccode_config.directories['resourcedir'], mccode_config.directories['docdir']))
            if printlog:
                print("writing doc file... %s" % h)
            write_file(h, text, failsilent=True)
        except:
            print("Could not work on comp " + str(i) + " of " + str(len(comp_infos)) + ": " + comp_infos[i].name)
            pass

    for i in range(len(instr_infos)):
        try:
            p = instr_infos[i]
            try:
                f = instr_infos[i].filepath
            except:
                f = instr_files[i]
            doc = InstrDocWriter(p)
            text = doc.create()
            h = pathlib.Path(os.path.splitext(f)[0] + '.html')
            h = pathlib.Path(str(h).replace(mccode_config.directories['resourcedir'], mccode_config.directories['docdir']))
            if printlog:
                print("writing doc file... %s" % h)
            write_file(h, text, failsilent=True)
        except:
            print("Could not work on instr " + str(i) + " of " + str(len(instr_infos)) + ": " + instr_infos[i].name)
            print(instr_infos[i].filepath)
            pass

def main(args):
    logging.basicConfig(level=logging.INFO)

    usedir = mccode_config.configuration["MCCODE_LIB_DIR"]
    docdir = mccode_config.directories["docdir"]

    if args.dir==None and args.install==False and args.searchterm==None and args.manual==False and args.comps==False and args.web==False:
        ''' browse system docs and exit '''
        if not os.path.isfile(os.path.join(docdir,mccode_config.get_mccode_prefix()+'doc.html')):
            try:
                sub=subprocess.Popen('%s%s -i' % (mccode_config.get_mccode_prefix(), 'doc'), shell=True)
                sub.wait()
            except:
                print("Could not write main mcdoc page, you may need admin permissions!")
                quit()

        subprocess.Popen('%s %s' % (mccode_config.configuration['BROWSER'], os.path.join(docdir,mccode_config.get_mccode_prefix()+'doc.html')), shell=True)
        quit()

    elif args.manual == True:
        ''' open manual and exit '''
        subprocess.Popen('%s %s' % (mccode_config.configuration['BROWSER'], os.path.join(docdir,mccode_config.configuration['MCCODE']+'-manual.pdf')), shell=True)
        quit()

    elif args.comps == True:
        ''' open component manual and exit '''
        subprocess.Popen('%s %s' % (mccode_config.configuration['BROWSER'], os.path.join(docdir,mccode_config.configuration['MCCODE']+'-components.pdf')), shell=True)
        quit()

    elif args.web == True:
        ''' open website and exit '''
        subprocess.Popen('%s %s' % (mccode_config.configuration['BROWSER'], 'https://www.'+mccode_config.configuration['MCCODE']+'.org'), shell=True)
        quit()

    elif args.install == True:
        ''' write system doc files '''
        if args.searchterm:
            print("will not write filtered system docs, exiting...")
            quit()
        print("writing mccode distribution docs...")
        if args.dir:
            usedir = args.dir
            print("using custom dir: %s" % usedir)

        comp_infos, instr_infos, comp_files, instr_files = parse_and_filter(usedir, recursive=True, printlog=args.verbose)
        write_doc_files_or_continue(comp_infos, instr_infos, comp_files, instr_files, args.verbose)

        masterdoc = OverviewDocWriter(comp_infos, instr_infos, [], [], mccode_config.configuration['MCCODE_LIB_DIR'], mccode_config.directories['docdir'])
        text = masterdoc.create()

        mcdoc_html_filepath = os.path.join(docdir,mccode_config.get_mccode_prefix()+'doc.html')
        try:
            write_file(mcdoc_html_filepath, text)
            print("master doc file: %s" % mcdoc_html_filepath)
        except Exception as e:
            print('ERROR writing master doc file: %s', e)

    elif args.dir != None or args.searchterm != None:
        ''' filtered and/or local results '''
        flter = '.*'
        usedir = mccode_config.configuration['MCCODE_LIB_DIR']
        rec = True
        if args.searchterm:
            flter = args.searchterm

        # single, specific file
        if args.searchterm is not None and re.search(r'\.', args.searchterm):
            usedir2 = '.'
            if args.dir is not None:
                usedir2 = args.dir
            f = os.path.join(usedir2, args.searchterm)
            
            # find matcing filenames
            instr_files, comp_files = utils.get_instr_comp_files(mccode_config.configuration['MCCODE_LIB_DIR'], True)
            comp_files = [f for f in comp_files if os.path.basename(f) == args.searchterm]
            instr_files = [f for f in instr_files if os.path.basename(f) == args.searchterm]

            # accumulate results
            results = []
            if os.path.isfile(f):
                results.append(f)
            results.extend(instr_files)
            results.extend(comp_files)
            if len(results) == 1:
                f = results[0]

                instr = re.search(r'[\w0-9]+\.instr', args.searchterm)
                comp = re.search(r'[\w0-9]+\.comp', args.searchterm)
                docdir = mccode_config.directories["docdir"]

                if instr:
                    f_base = os.path.splitext(os.path.basename(f))[0]
                    f_class = os.path.basename(os.path.dirname(os.path.dirname(f)))
                    f_html = os.path.join(docdir,'examples',f_class,f_base,f_base + ".html")
                    info = InstrParser(f).parse()
                    info.filepath = os.path.abspath(f)
                    write_doc_files_or_continue([], [info], [], [f])
                    subprocess.Popen('%s %s' % (mccode_config.configuration['BROWSER'], f_html), shell=True)
                elif comp:
                    f_base = os.path.splitext(os.path.basename(f))[0] + ".html"
                    f_class = os.path.basename(os.path.dirname(f))
                    f_html = os.path.join(docdir,f_class,f_base)
                    info = CompParser(f).parse()
                    info.filepath = os.path.abspath(f)
                    write_doc_files_or_continue([info], [], [f], [])
                    subprocess.Popen('%s %s' % (mccode_config.configuration['BROWSER'], f_html), shell=True)
                quit()
            # there were multiple matches - fall back to general search term mode
            else:
                flter = os.path.splitext(os.path.basename(args.searchterm))[0]

        # system
        comp_infos, instr_infos, comp_files, instr_files = parse_and_filter(usedir, flter, recursive=True)
        write_doc_files_or_continue(comp_infos, instr_infos, comp_files, instr_files)

        # local
        comp_infos_local = []
        instr_infos_local = []
        if args.dir != None:
            usedir = args.dir
            comp_infos_local, instr_infos_local, comp_files, instr_files = parse_and_filter(args.dir, flter, recursive=False)
            write_doc_files_or_continue(comp_infos_local, instr_infos_local, comp_files, instr_files)

        if len(comp_infos_local) + len(instr_infos_local) + len(comp_infos) + len(instr_infos) == 0:
            print("no matches found")
            quit()

        masterdoc = OverviewDocWriter(comp_infos, instr_infos, comp_infos_local, instr_infos_local, usedir, mccode_config.directories['docdir'])
        text = masterdoc.create()

        mcdoc_html_filepath = os.path.join('.', mccode_config.get_mccode_prefix()+'doc.html')
        if args.verbose:
            print('writing local overview doc file... %s' % mcdoc_html_filepath)
        write_file(mcdoc_html_filepath, text)

        subprocess.Popen('%s %s' % (mccode_config.configuration['BROWSER'], os.path.join('.',mccode_config.get_mccode_prefix()+'doc.html')), shell=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('searchterm', nargs='?', help='search filter or .instr/.comp file')
    parser.add_argument('--install', '-i', action='store_true', help='generate installation master doc page')
    parser.add_argument('--dir', '-d', help='add search results from this directory')
    parser.add_argument('--manual','-m', action='store_true', help='open the system manual')
    parser.add_argument('--comps','-c', action='store_true', help='open the component manual')
    parser.add_argument('--web','-w', action='store_true', help='open the '+mccode_config.configuration['MCCODE']+' website')
    parser.add_argument('--verbose','-v', action='store_true', help='prints a parsing log during execution')
    
    
    args = parser.parse_args()
    
    try:
        main(args)
    except Exception as e:
        print(str(e))

