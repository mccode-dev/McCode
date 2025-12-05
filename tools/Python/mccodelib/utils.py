'''
Analysis tools for mcstas component files and instrument files.
'''
import re
import os
from os.path import splitext, join
import subprocess
from datetime import datetime

'''
Component parser used by initial versions of mcgui. (More recent implementations exist.)
'''
class ComponentParInfo(object):
    ''' Component parameter info, used as ComponentParser.pars '''
    def __init__(self, par_info=None):
        if par_info:
            self.par_name = par_info.par_name
            self.type = par_info.type
            self.default_value = par_info.default_value
            self.doc_and_unit = par_info.doc_and_unit
    par_name = ''       # parameter par_name
    type = ''           # can be "string" or "int", but is mostly empty
    default_value = ''
    doc_and_unit = ''   # doc string and unit (no linebreaks)


class ComponentParser(object):
    ''' Component file parser. '''
    def __init__(self, comp_file):
        if comp_file == '':
            raise Exception('ComponentParser: "comp_file" may not be an empty.')
        
        self.file = comp_file
        self.name = None
        self.info = None
        self.description = None     # info section all in one string  (multiple lines)
        self.pars = []              # list of McComponentParInfo
        self.mcdisplay = None       # optional MCDISPLAY section
        
        self.__hasParsed = False
    
    def parse(self):
        ''' main parse component for info function '''
        
        if self.__hasParsed:
            return
        
        # load component from file
        text = open(self.file).read()
        if text == '':
            raise Exception('parse: component file is empty.')
        
        # get component info, description and par docs from header section
        self.info, self.description, header_P_section = self.__parseComponentHeader(text)
        
        # get component par_name from main section
        self.name = self.__parseComponentName(text)
        
        # get component pars from main section and match with par docs from header section
        self.pars = self.__parseComponentPars(text)
        self.__matchDocStringsToPars(self.pars, header_P_section)

        self.__hasParsed = True
    
    def parseDisplaySection(self):
        ''' optional: call to parse MCDISPLAY section and save in member "mcdisplay" '''
        
        # load component from file
        text = open(self.file).read()
        if text == '':
            raise Exception('parse: component file is empty.')
        
        pat = r'MCDISPLAY\s*\%\{([-+.\w\s=<>,/*{}\"\'()\.&$=?;:*|]*)\%\}'
        sr = re.search(pat, text)
        if sr:
            self.mcdisplay = sr.group()
        else:
            self.mcdisplay = 'missing MCDISPLAY string'
    
    @staticmethod
    def __parseComponentHeader(text):
        # identify the start and end positions of the I, D and P sections
        pos_I = text.find('%I')
        pos_D = text.find('%D')
        pos_P = text.find('%P')
        pos_E = text.find('%E')
        
        # validate match for %I, %D, %P and %E positions
        if pos_I == -1:
            raise Exception('parseComponentHeader: Missing %I tag.')
        if pos_D == -1:
            raise Exception('parseComponentHeader: Missing %D tag.')
        if pos_P == -1:
            raise Exception('parseComponentHeader: Missing %P tag.')
        if pos_E == -1:
            raise Exception('parseComponentHeader: Missing %E tag.')
        
        # extract strings for I, D and P sections
        text_I = ComponentParser.__removeStarsFromLines(text[pos_I+1: pos_D])
        text_D = ComponentParser.__removeStarsFromLines(text[pos_D+1: pos_P])
        text_P = ComponentParser.__removeStarsFromLines(text[pos_P+1: pos_E])
        
        return text_I, text_D, text_P
        
    @staticmethod
    def __parseComponentName(text):
        re_out = re.search(r'DEFINE\s+COMPONENT\s+(\w*)', text)
        if re_out:
            return re_out.group(1)
    
    @staticmethod
    def __parseComponentPars(text):
        ''' parse definition parameters and setting parameters from the main component section
        '''
        # get definition parameters
        result_1 = []
        re_out = re.search(r'DEFINITION\s+PARAMETERS\s*\([-+.\w\s=,/*{}\"]+\)', text)
        if re_out:
            result_sub, substituted_text = ComponentParser.__substituteCurlyPars(re_out.group(0))
            result_1 = ComponentParser.__parseParLine(substituted_text)
            # restitute curly pars default values
            for r in result_1:
                for s in result_sub:
                    if r.par_name == s.par_name:
                        r.default_value = s.default_value
        
        # get setting parameters
        result_2 = []
        re_out = re.search(r'SETTING\s+PARAMETERS\s*\([-+.\w\s=,/*{}\"]+\)', text)
        if re_out:
            result_sub, substituted_text = ComponentParser.__substituteCurlyPars(re_out.group(0))
            result_2 = ComponentParser.__parseParLine(substituted_text)
            # restitute curly pars default values
            for r in result_2:
                for s in result_sub:
                    if r.par_name == s.par_name:
                        r.default_value = s.default_value
        
        return result_1 + result_2
    
    @staticmethod
    def __substituteCurlyPars(text):
        # Extract stuff like def_par={1.0, 2.0, 3.0} which contains commas inside a curly bracket.
        # Returns curly bracket (name, default value), and text with such values replaced by placeholder string
        
        curly = re.findall(r'(\w+)=(\{[-+.\w\s,/*\"]*\})', text)
        result = []
        substituted_text = str(text)
        
        # get par_info into the par info par_info structure: 
        for p in curly:
            par_info = ComponentParInfo()
            par_info.par_name = p[0]
            par_info.default_value = p[1]
            result.append(par_info)
            
            # reduce
            substituted_text = substituted_text.replace(par_info.par_name + '=' + par_info.default_value, par_info.par_name + '=' + par_info.par_name + '_substituted')
        
        return result, substituted_text
    
    @staticmethod 
    def __parseParLine(text):
        # assuming no curly brackets - these must have been processed and removed from text
        # always return something 
        result = []
        allowed = r'[-+.\w\s=,/*{}\"]'
        
        # get the text in parenthesis
        out1 = re.search(r'\(([-+.\w\s=,/*{}\"]+)\)', text)
        if not out1:
            return result
            # TODO: error handling
        
        # get comma separated sections
        comma_sep = re.findall(r'([-+.\w\s=/*\"]+),*', out1.group(1))
        
        # get par_info into the par info par_info structure: 
        for p in comma_sep:
            par_info = ComponentParInfo()
            
            out = re.search(r'(\w+)\s+([\w\-]+)\s*=\s*([-+.\w/*\"\s]+)', p)
            if out:
                # type par_name = def_val
                par_info.type = out.group(1)
                par_info.par_name = out.group(2)
                par_info.default_value = out.group(3)
            else:
                # par_name = def_val
                out = re.search(r'([\w\-]+)\s*=\s*([-+.\w/*\"\s]+)', p)
                if out:
                    par_info.par_name = out.group(1)
                    par_info.default_value = out.group(2)
                else:
                    # par_name
                    par_info.par_name = p.strip()
            
            result.append(par_info)
        
        return result
    
    @staticmethod
    def __matchDocStringsToPars(pars, header_P_section):
        # sets docstring values on objects in the "pars" list of McComponentParInfo 
        parnames = [p.par_name for p in pars]
        lines = header_P_section.splitlines()
        lastpar = None

        for line in lines:
            # Try to match "param: description"
            out = re.search(r'^(.*)([\[\w\s\]])(.*)$', line)
            candidate = None
            description = None

            if out:
                candidate = out.group(1).rstrip()
                description = out.group(2).strip()
            else:
                # No colon: split on first whitespace only
                parts = line.split(None, 1)
                if len(parts) == 2:
                    candidate, description = parts[0], parts[1].strip()
                elif len(parts) == 1:
                    candidate = parts[0]
                description = ' '

            if candidate and candidate in parnames:
            # candidate is a known par name
                j = parnames.index(candidate)
                pars[j].doc_and_unit += (description + ' ').strip()
                lastpar = pars[j]
            elif lastpar and description is not None:
                # continuation or appended description
                lastpar.doc_and_unit += ' ' + description.strip()
            elif lastpar and ComponentParser.__stringIsEmptyLine(line):
                # empty line, close lastpar appending
                lastpar = None
            elif lastpar:
                # continuation line, append to lastpar
                lastpar.doc_and_unit += ' ' + line.strip()
    
    @staticmethod
    def __stringIsEmptyLine(s):
        out = re.match(r'[\s\t\r\n]*', s)

        empty = False
        if out:
                empty = out.group() == s
        return empty
    
    @staticmethod
    def __removeStarsFromLines(text):
        result = ''        
        text_lns = list(iter(text.splitlines()))
        for l in text_lns:
            # lstrip '*' if present, otherwise just remove left whitespace
            if l.startswith('*'):
                l = l.lstrip('*')
            else:
                l = l.lstrip()
            l = l.strip()
            result += '\n'
            result += l
        return result.strip('\n')
    
    @staticmethod
    def __parseInstrumentDef(text):
        re_out = re.search(r'DEFINE\s+INSTRUMENT\s+([a-zA-Z0-9_]*)\s*\(([-+.a-zA-Z0-9_ \t\n\r=,/*{}\"]*)\)', text)
        if re_out:
            return re_out.group(1)


'''

Utility functions related to mccode file handling.

'''

'''
Utility functions for parsing instrument and component files.
'''
def read_header(file):
    '''
    Reads lines of a slash-star commented section, from file-object. 
    Returns acc. read lines as text.
    '''
    lines = []
    while True:
        try:
            l = file.readline()
            lines.append(l)
        except:
            break
        if not re.match(r'[ ]*\*', l):
            if not re.match(r'[ ]*\/\*', l):
                break
        elif re.search(r'[ ]*\*\*\*\*', l):
            break
        else:
            # line is still in header but not prefixed by * - we tolerate...
            pass

    return ''.join(lines)

class InstrCompHeaderInfo:
    field_cols = ['name', 'author', 'date', 'origin', 'site', 'short_description', 'description', 'test']
    lst_cols = ['params', 'params_docs', 'links']
    def __init__(self):
        self.name = ''
        self.filepath = ''
        # instr params
        self.params = []
        # comp params
        self.setparams = []
        self.defparams = []
        self.outparams = []
        # comp category
        self.category = ''
        # doc info
        self.author = ''
        self.date = ''
        self.origin = ''
        self.site = ''
        self.short_descr = ''
        self.description = ''
        self.test = ''
        self.params_docs = []
        self.links = []
    @staticmethod
    def __len__():
        return len(InstrCompHeaderInfo.field_cols) + len(InstrCompHeaderInfo.lst_cols)
    @staticmethod
    def colname(idx):
        if idx >= 0 and idx <= 7:
            return InstrCompHeaderInfo.field_cols[idx]
        elif idx >= 8 and idx <= 10:
            return InstrCompHeaderInfo.lst_cols[idx-8]
        else:
            raise Exception("InstrCompHeaderInfo.colname: invalid index")
    def __getitem__(self, idx):
        if idx == 0: return self.name
        elif idx == 1: return self.author
        elif idx == 2: return self.date
        elif idx == 3: return self.origin
        elif idx == 4: return self.site
        elif idx == 5: return self.short_descr
        elif idx == 6: return self.description
        elif idx == 7: return self.test
        elif idx == 8: return self.params
        elif idx == 9: return self.params_docs
        elif idx == 10: return self.links
        else:
            raise Exception("InstrCompHeaderInfo.__getitem__: idx must be in range(%s)." % str(len(self)))
    def __setitem__(self, idx, value):
        if idx == 0: self.name = value
        elif idx == 1: self.author = value
        elif idx == 2: self.date = value
        elif idx == 3: self.origin = value
        elif idx == 4: self.site = value
        elif idx == 5: self.short_descr = value
        elif idx == 6: self.description = value
        elif idx == 7: self.test = value
        elif idx == 8: self.params = value
        elif idx == 9: self.params_docs = value
        elif idx == 10: self.links = value
        else:
            raise Exception("InstrCompHeaderInfo.__setitem__: idx must be in range(%s)." % str(len(self)))
    def __str__(self):
        lst = [self.name, self.author, self.date, self.origin, self.site, self.short_descr, self.description, self.test]
        lst2 = [' '.join(d) for d in self.params_docs]
        lst3 = [' '.join([str(c) for c in p]) for p in self.params]
        lst4 = [l for l in self.links]
        return '\n'.join(lst) + '\n\n- params docs:\n' + '\n'.join(lst2) + '\n\n- params:\n' + '\n'.join(lst3) + '\n\n- links:\n' + '\n'.join(lst4)

def parse_header(text):
    ''' Parses the header of an instrument or component file '''
    # get rid of stars and empty padding lines
    lines = text.splitlines()
    new_lines = []
    for i in range(len(lines)):
        l = lines[i]
        if l.startswith('*'):
            new_lines.append(l.strip('*').strip()) # strip spaces then left stars
        else:
            new_lines.append(l.strip().strip())
    text = '\n'.join(new_lines)
    
    # get tag indices, and deal with cases of missing tags
    tag_I=0
    tag_D=1
    tag_E=2
    tag_P=3
    tag_L=4
    lst = [text.find('%I'), text.find('%D'), text.find('%Ex'), text.find('%P'), text.find('%L')]
    # missing %Example tag
    if lst[tag_E] == -1:
        lst[tag_E] = lst[tag_P]
    # existing %I tag with missing %D tag handled like this
    if lst[tag_I] > lst[tag_D] and lst[tag_E] > lst[tag_D]: 
        lst[tag_D] = lst[tag_E]
    # if %E is actually %End:
    if lst[tag_E] > lst[tag_P] and lst[tag_P] != -1:
        lst[tag_E] = lst[tag_P]
    for i in range(len(lst)-1):
        if lst[i] > lst[i+1]:
            lst[i+1] = lst[i]
    if lst[tag_L] == lst[tag_P]:
        lst[tag_L] = len(text)

    # cut header into some sections
    bites = [text[lst[i]:lst[i+1]].strip() for i in range(len(lst)-1)]
    bites.append(text[lst[tag_L]:])
    info = InstrCompHeaderInfo()
    
    # get author, date, origin, revision
    m1 = re.search(r'Written by:([^\n]*)\n', bites[tag_I])
    if not m1:
      m1 = re.search(r'Author:([^\n]*)\n', bites[tag_I])
    if m1: info.author = m1.group(1).strip()
    
    m2 = re.search(r'Date:([^\n]*)\n', bites[tag_I])
    if m2: info.date = m2.group(1).strip()
    
    m3 = re.search(r'Origin:([^\n]*)\n', bites[tag_I])
    if m3: info.origin = m3.group(1).strip()
    
    m4 = re.search(r'Version:([^\n]*)\n', bites[tag_I])
    if m4: info.version = m4.group(1).strip()
    
    m5 = re.search(r'\%INSTRUMENT_SITE:[^\n]*\n(.*)', bites[tag_I], flags=re.DOTALL)
    if not m5:
      m5 = re.search(r'Origin:[^\n]*\n(.*)', bites[tag_I], flags=re.DOTALL)
    
    if m5:
      # ignore some comments
      descrlines = []
      # remove all "*:" lines
      for l in m5.group(1).strip().splitlines():
        # PW 2025: Suppressed the : detection, but strip off %INSTRUMENT_SITE and Modified by 
        if not re.match(r'%INSTRUMENT_SITE:', l, flags=re.DOTALL) and not re.match(r'Modified by:', l, flags=re.DOTALL):
          descrlines.append(l)
      info.short_descr = '\n'.join(descrlines).strip()
    
    # description
    descr = bites[tag_D]
    if re.match(r'\%Description', descr):
        descr = descr.replace('%Description', '').strip()
    elif re.match(r'\%D', descr):
        descr = descr.replace('%D', '').strip()
    info.description = descr
    
    # test / example
    tst = bites[tag_E]
    info.test = tst
    
    # params
    par_doc = None
    for l in bites[tag_P].splitlines():
        # regex is tolerant for mising ':' in  param: [unit] description
        m = re.match(r'(\w+)[: \t]*\[([ \w\/\(\)\\\~\-.,\":\%\^\|\{\};\*]*)\][ \t]*(.*)', l)
        par_doc = (None, None, None)
        if m:
            par_doc = (m.group(1), m.group(2), m.group(3).strip())
            info.params_docs.append(par_doc)
        else:
            m = re.match(r'(\w+):[ \t]*(.*)', l)
            if m:
                par_doc = (m.group(1), "", m.group(2).strip())
                info.params_docs.append(par_doc)
    
    # links
    for l in bites[tag_L].splitlines():
        if re.match(r'\s*%', l) or l.strip() == '' or re.match(r'\/', l):
            continue
        info.links.append(l)
    
    return info

def read_define_instr(file):
    '''
    Reads lines from file obj until DEFINE INSTRUMENT, then reads lines until ")".
    Parses this statement and returns the result in organized form.
    '''
    lines = []
    for l in file:
        if not re.match(r'DEFINE[ \t]+INSTRUMENT[ \t]+', l):
            continue
        else:
            lines.append(re.sub(r'//.*', '', l.strip()))
            break
    
    if len(lines) > 0 and not re.search(r'\)', lines[-1]):
        for l in file:
            lines.append(re.sub(r'//.*', '', l.strip()))
            if re.search(r'\)', l):
                break
    if not lines[-1] == ')':
        lines.append(')')
    return ' '.join(lines)

def read_define_comp(file):
    ''' appends all read lines to return list '''
    end_conds = ('SHARE', 'DECLARE', 'INITIALIZE', 'TRACE')
    
    lines = []
    
    # get to DEFINE COMPONENT statement
    for l in file:
        lines.append(l.strip())
        if not re.match(r'DEFINE[ \t]+COMPONENT[ \t]+', l):
            continue
        else:
            break
    
    for l in file:
        m = re.match(r'[ ]*(\w+)', l)
        lines.append(l.strip())
        if m and m.group(1) in end_conds:
            break
    
    # look for closing 
    if not re.search(r'\)', lines[-1]):
        for l in file:
            lines.append(l.strip())
            if re.search(r'\)', l):
                break
    
    return '\n'.join(lines)

def get_comp_category(filepath):
    ''' extract first folder name from file path '''
    head = os.path.split(filepath)[0]
    firstdir = os.path.split(head)[1]
    # handle special case - sub folder of "contrib" folder
    if firstdir not in ('sources', 'optics', 'samples', 'monitors', 'misc', 'contrib', 'union','sasmodels','astrox','obsolete'):
        head2 = os.path.split(head)[0]
        seconddir = os.path.split(head2)[1]
        return seconddir
    return firstdir

def parse_define_comp(text):
    text = text.replace('\n', ' ')
    text = text.replace(' = ', '=')
    text = text.replace(' =', '=')
    text = text.replace('= ', '=')
    
    name = re.search(r'DEFINE[ \t]+COMPONENT[ \t]+(\w+)', text).group(1)
    m = re.search(r'DEFINITION[ \t]+PARAMETERS[ \t]*\(([\w\,\"\s\n\t\r\.\+\-=\{\}]*)\)', text)
    defpar = []
    if m:
        defpar = parse_params(m.group(1))
    m = re.search(r'SETTING[ \t]+PARAMETERS[ \t]*\(([\w\,\"\s\n\t\r\.\+\-=\{\}]*)\)', text)
    setpar = []
    if m:
        setpar = parse_params(m.group(1))
    m = re.search(r'OUTPUT[ \t]+PARAMETERS[ \t]*\(([\w\,\"\s\n\t\r\.\+\-=]*)\)', text)
    outpar = []
    if m:
        outpar = parse_params(m.group(1))
    
    return (name, defpar, setpar, outpar)

def clean_instr_def(defline):
    ''' takes a typical output of read_define_instr() and extracts the "params string" for use with parse_params '''
    m = re.search(r'\(([^(^)]*)\)', defline)
    try:
        return m.group(1)
    except:
        return None

def parse_params(params_line):
    ''' creates a list of 3-tuples (type, name, devault_value)) from a "params string" '''
    params = []

    def par_rec(substr, lst):
        try:
            # handle parameter sections including curly bracket default values (case 2)
            m1 = re.match(r'([^,{]+),(.*)', substr) # not comma or curl, then a comma
            m2 = re.match(r'([^,{]+\{[^}]*\}\s*),(.*)', substr) # not comma or curl, then not a right curl, then a right curlt, then a comma
    
            if m1:
                lst.append(m1.group(1)) 
                substr = m1.group(2)
            elif m2:
                lst.append(m2.group(1))
                substr = m2.group(2)
            else:
                # the end
                return lst
    
            # continue recursion
            return par_rec(substr, lst)
        except Exception as e:
            print("error", e)

    # split the line into parts corresponding to each parameter definition
    params_line = params_line.lstrip('(').rstrip(')') # secure brackets stripped
    if '{' in params_line:
        # NOTE: this may exceed python max recursion depth in some cases, e.g. guide_four_sides_10_shells
        # however, this version is required for parsing {a,b,c} style default values
        # TODO: reimplement into a while-based iteration rather than a recursion
        parts = par_rec(params_line, [])
    else:
        parts = [s.strip() for s in params_line.split(',')]

    # parse each parameter
    for part in parts:
        tpe = None
        dval = None
        name = None
        if re.search(r'^ ', part):
            part = part[1:]
        if re.match(r'double ', part):
            part = part.replace('double ', '').strip()
        if re.match(r'string ', part):
            tpe = 'string'
            part = part.replace('string ', '').strip()
        if re.match(r'vector ', part):
            tpe = 'vector'
            part = part.replace('vector ', '').strip()
        if re.match(r'int ', part):
            tpe = 'int'
            part = part.replace('int ', '').strip()
        if re.search(r'"', part):
            m=re.match(r'(.*)\s*=\s*"(.*)"', part)
            dval = '"' + m.group(2) + '"'
            name = m.group(1).strip()
        elif re.search(r'=', part):
            m = re.match(r'(.*)\s*=\s*(.*)', part)
            dval = m.group(2).strip()
            name = m.group(1).strip()
        else:
            name = part.strip()
        if name not in (None, "", ):
            params.append((tpe, name, dval))
    return params

def parse_define_instr(text):
    '''
    Parses a DEFINE INSTRUMENT statement from an instrument file.
    Not robust to "junk" in the input string.
    '''
    try:
        m = re.match(r'DEFINE[ \t]+INSTRUMENT[ \t]+(\w+)\s*\(([\w\,\"\s\n\t\r\.\+:;\-=/]*)\)+', text)
        name = m.group(1)
        params = m.group(2).replace('\n', '').strip()
    except:
        return '', []
    return name, parse_params(params)

def read_declare(file):
    raise Exception('Read_declare: not yet implemented.')

def read_initialize(file):
    raise Exception('Read_initialize: not yet implemented.')

def read_trace(file):
    raise Exception('Read_trace: not yet implemented.')

def read_finally(file):
    raise Exception('Read_finally: not yet implemented.')

def get_instr_site_fromtxt(text):
    m = re.search(r'\%INSTRUMENT_SITE:[ \t]*(\w+)[ \t]*\n?', text)
    if m:
        return m.group(1)
    else:
        #raise Exception('Tag "%INSTRUMENT_SITE" not found.')
        return ''

def get_instr_site(instr_file):
    ''' extracts and returns the rest of the line, from the text file instr_file, containing "%INSTRUMENT_SITE:" '''
    try:
        f = open(instr_file, 'rb')
        text = f.read().decode()
        f.close()
    except:
        text = '';
        print("WARNING: \n  Attempt to parse instrument header faliled for: \n %s" % instr_file)
    
    site = '("%INSTRUMENT_SITE:" tag not found)'
    
    start = text.find('%INSTRUMENT_SITE:')
    if start > -1:
        end = text.find('\n', start)
        site = text[start+17:end].strip()
        
    return site

def get_instr_comp_files(mydir, recursive=True, instrfilter=None, compfilter=None):
    ''' returns list of filename with path of all .instr and .comp recursively from dir "mydir"

    181211: added recursive, defaults to True to preserve backwards compatibility
    191114: added instrfilter and compfilter, which filters results based on filename (before the dot)
    '''
    instrreg = None
    compreg = None
    if compfilter:
        compreg = re.compile(compfilter)

    files_instr = [] 
    files_comp = []

    for (dirpath, _, files) in os.walk(mydir):
        for f in files:
            # get instr files
            if splitext(f)[1] == '.instr':
                if instrfilter is not None:
                    filters=instrfilter.split(",")
                    numfilters=len(filters)
                    for filter in filters:
                        instrreg = re.compile(filter)
                        if instrreg.search(join(dirpath,f), re.IGNORECASE):
                            files_instr.append(join(dirpath, f))
                else:
                    files_instr.append(join(dirpath, f))

            # get comp files
            if os.path.splitext(f)[1] == '.comp':
                if compfilter is not None:
                    if compreg.search(f):
                        files_comp.append(join(dirpath, f))
                else:
                    files_comp.append(join(dirpath, f))
        if not recursive:
            break
    
    return files_instr, files_comp

def save_instrfile(instr, text):
    ''' 
    Creates and/or saves instrument file, makes sure the file extension is .instr.
    Returns file par_name, or empty string if no file was saved/created.
    '''
    if instr == '':
        return ''
    
    if str(instr).find('.') >= 0:
        if os.path.splitext(str(instr))[1] != '.instr':
            instr = instr + '.instr'
    else:
        instr = instr + '.instr'
    
    # TODO: add try-finally and error handling
    f = open(str(instr), 'w', newline='\n')
    f.write(text)
    f.close()
    
    return instr

def get_file_text_direct(file):
    ''' Opens a file for reading binary, reads it, decodes the results and returns the text. Fixes an occational issue on MacOSX. '''
    f = open(file, 'rb')
    text = f.read().decode()
    f.close()
    return text

def get_file_contents(filepath):
    ''' returns file contents if file exists '''
    if os.path.exists(str(filepath)):
        return get_file_text_direct(filepath)
    else:
        return ''

def run_subtool_noread(cmd, cwd=None):
    ''' run subtool to completion in a excessive pipe-output robust way (millions of lines) '''
    if not cwd:
        cwd = os.getcwd()
    try:
        process = subprocess.Popen(cmd,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE,
                                   stdin=subprocess.PIPE,
                                   shell=True,
                                   universal_newlines=True,
                                   cwd=cwd)
        process.communicate()
        return process.returncode
    except Exception as e:
        ''' unicode read error safe-guard '''
        print("run_subtool_noread (cmd=%s) error: %s" % (cmd, str(e)))
        return -1

def run_subtool_to_completion(cmd, cwd=None, stdout_cb=None, stderr_cb=None):
    '''
    Synchronous run subprocess.popen with pipe buffer reads, 
    one-string command 'cmd' and running in directory 'cwd'.
    '''
    def call_if_not_none(fct, *args):
        ''' shorthand utility for calling a function if it is defined, and otherwise ignoring it '''
        if fct:
            fct(*args)
    if not cwd:
        cwd = os.getcwd()

    try:
        # open the process with all bells & whistles
        process = subprocess.Popen(cmd,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE,
                                   stdin=subprocess.PIPE,
                                   shell=True,
                                   universal_newlines=True,
                                   cwd=cwd)
        # flush until EOF
        for stdoutdata in process.stdout:
            call_if_not_none(stdout_cb, stdoutdata.rstrip('\n'))
        for stderrdata in process.stderr:
            call_if_not_none(stderr_cb, stderrdata.rstrip('\n'))

        return process.returncode
    except Exception as e:
        ''' unicode read error safe-guard '''
        print("run_subtool_to_completion: An error occurred, %s" % str(e))
        return -1

def start_subtool_then_return(cmd, cwd=None):
    '''
    Asynchronous run subprocess.popen, using
    one-string command 'cmd' in directory 'cwd'.
    '''
    def call_if_not_none(fct, *args):
        ''' shorthand utility for calling a function if it is defined, and otherwise ignoring it '''
        if fct:
            fct(*args)
    if not cwd:
        cwd = os.getcwd()

    # open the process with all bells & whistles
    process = subprocess.Popen(cmd,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,
                               stdin=subprocess.PIPE,
                               shell=True,
                               universal_newlines=True,
                               cwd=cwd)

    return process.returncode

def dumpfile_pqtg(scene, filenamebase='mcplot', format='png'):
    ''' save as png file. Pdf is not supported, althouhg svg kind-of is '''
    import pyqtgraph.exporters
    import pyqtgraph as pg

    outputfile = '%s.%s' % (filenamebase, format)
    # Check for existance of earlier exports
    if os.path.isfile(outputfile):
        index=1
        outputfile = '%s_%i.%s' % (filenamebase, index, format )
        while os.path.isfile(outputfile):
            index += 1
            outputfile = '%s_%i.%s' % (filenamebase, index, format)

    if format=='png':
        exporter = pg.exporters.ImageExporter(scene)
        exporter.export(outputfile)
    elif format=='svg':
        exporter = pg.exporters.SVGExporter(scene)
        exporter.export(outputfile)
        # This is where PDF export would be handled via conversion from SVG
        #    elif format=='pdf':
        #        exporter = pg.exporters.SVGExporter(scene)
        #        exporter.export(outputfile)
        #        import subprocess
        #        # TODO: error handling
        #        process = subprocess.Popen('svg2pdf %s %s.pdf' % (outputfile, '%s_%i.%s' % (filenamebase, index, 'pdf')), 
        #                                   stdout=subprocess.PIPE, 
        #                                   stderr=subprocess.PIPE,
        #                                   shell=True,
        #                                   universal_newlines=True)
    else:
        raise Exception('png, and svg are the only supported file formats (format=%s)' % format)
    if os.path.isfile(outputfile):
        print('Graphics output ' + outputfile + ' saved')


def get_datetimestr():
    return datetime.strftime(datetime.now(), "%Y%m%d_%H%M_%S")


def make_executable(path):
    mode = os.stat(path).st_mode
    mode |= (mode & 0o444) >> 2    # copy R bits to X
    os.chmod(path, mode)
