#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
SasView_generator.py analyzes C-files prefixed "sas_" in the specified directory.
It the C-files contain valid sasmodels, it generates a file bassed on these - "sasview_proxy.c".
This file is written to the same directory as the C-files (and thus may overwrite any previous version of the file).
It also generates rudimentary, but necessary, documentation and inserts it into the mcstas component.
The modified component template (suffix .Template) is output as a new file. The input component template must contain
MCDOC and MCDOC_END flags in the header.
The new component file has the same base name, except that the .Template suffix is removed.
'''
import os
import logging
import re
import argparse

class SasViewModelFileInfo():
    '''
    Takes a sas_<modelname>.c file and parses include, function call and doc hint (non-q model parameter names).
    Construct and then use the objects string properties to construct the desired C code or docs.
    '''
    def __init__(self, model_file_fulpath, pars_array_name):
        # check input
        # TODO: implement: throw exception on fail (user should use try-catch)

        # generate static info
        self.input_str = model_file_fulpath
        self.text = open(model_file_fulpath).read()
        self.pars_array_name = pars_array_name
        self.model_name = re.search(r'sas_(.*).c', os.path.basename(model_file_fulpath)).group(1)
        self.model_name_xy = self.model_name + '_xy'

        self.percent_include = '%%include "%s"' % os.path.basename(model_file_fulpath)
        self.hash_include = '#include <%s>' % os.path.basename(model_file_fulpath)

        self.sign_non_q = self.__getSignNonQ(self.text, False)
        self.sign_xy_non_q = self.__getSignNonQ(self.text, True)
        self.sign_form_vol = self.__getFormVolumeSign(self.text)

        self.__num_model_pars = self.__getNumPars(self.sign_non_q)
        self.__num_model_pars_xy = self.__getNumPars(self.sign_xy_non_q)
        self.__num_form_vol_pars = self.__getNumPars(self.sign_form_vol)

        self.Iq_hint = self.__getHint(self.sign_non_q)
        self.Iq_xy_hint = self.__getHint(self.sign_xy_non_q)
        self.form_volume_hint = self.__getHint(self.sign_form_vol)

        self.Iq_call = self.__getIqCall(pars_array_name, self.__num_model_pars, xy=False)
        self.Iqxy_call = self.__getIqCall(pars_array_name, self.__num_model_pars_xy, xy=True)
        self.form_volume_call = self.__getFormVolCall(pars_array_name, self.__num_form_vol_pars, self.sign_non_q, self.sign_form_vol)

    def printMe(self):
        text = 'input: %s\ntext: %s\npars_name: %s\nmodel_name: %s\n%s\n%s\nsign_non_q: %s\nsign_xy_non_q: %s\nsign_form_vol: %s\nIq_hint: %s\nIq_xy_hint: %s\nIq_call: %s\nIqxy_call: %s\nform_volume_call: %s\n' % (
                                            self.input_str, '(not shown)', self.pars_array_name,
                                            self.model_name, self.percent_include, self.hash_include,
                                            self.sign_non_q, self.sign_xy_non_q, self.sign_form_vol,
                                            self.Iq_hint, self.Iq_xy_hint,
                                            self.Iq_call, self.Iqxy_call, self.form_volume_call)
        return '#####\n' + text + '#####\n'

    @staticmethod
    def __getNumPars(sign):
        c = 1
        for comma in re.finditer(',', sign):
            c += 1
        return c

    @staticmethod
    def __getSignNonQ(text, xy=False):
        # get all cases covered
        define_str = r'#define\s+IQ_PARAMETER_DECLARATIONS\s+([\w\s,]*\n)'
        if xy:
            define_str = r'#define\s+IQXY_PARAMETER_DECLARATIONS\s+([\w\s,]*)\n'

        sign_str = r'float\s+Iq\(\s*float\s+q\s*,([\w\s,]*)\)'
        sign_str_2 = r'float\s+Iq\(\s*float\s+qval\s*,([\w\s,]*)\)'
        sign_str_3 = r'float\s+Iq\(\s*float\s+QQ\s*,([\w\s,]*)\)'
        if xy:
            sign_str = r'float\s+Iqxy\(\s*float\s+qx\s*,\s*float\s+qy\s*,([\w\s,]*)\)'
            # LOOK UP IN FILES:
            sign_str_2 = r'float\s+Iqxy\(\s*float\s+qx\s*,\s*float\s+qy\s*,([\w\s,]*)\)'
            sign_str_3 = r'float\s+Iqxy\(\s*float\s+qx\s*,\s*float\s+qy\s*,([\w\s,]*)\)'

        # logics to extract signature
        m = re.search(define_str, text)
        if m:
            # entire non-q sign is contained in the define
            sign = re.sub('\s+', ' ', m.group(1))
            sign = sign.strip(' ')
            return sign
        else:
            # entire sign is contained in the function declaration
            m = re.search(sign_str, text)
            if not m:
                m = re.search(sign_str_2, text)
            if not m:
                m = re.search(sign_str_3, text)
            if m:
                sign = re.sub('\s+', ' ', m.group(1))
                sign = sign.strip(' ')
                return sign
            else:
                raise Exception('no "float Iq(...)" or "float Iqxy(...)" function signature found')

    @staticmethod
    def __getFormVolumeSign(text):
        # get all cases covered
        define_str_ver1 = r'#define\s+VOLUME_PARAMETER_DECLARATIONS\s+([\w\s,]*\n)'
        define_str_ver2 = r'#define\s+VOLUME_PARAMETERS\s+([\w\s,]*\n)'

        sign_str = r'float\s+form_volume\(([\w\s,]*)\)'
        sign_str_2 = r'float\s+form_volume\(([\w\s,]*)\)'

        # NOTE: sometimes VOLUME_PARAMETER_DECLARATIONS turns out "void"

        # logics to extract signature
        m = re.search(define_str_ver1, text)
        if m:
            # entire non-q sign is contained in the define
            sign = re.sub('\s+', ' ', m.group(1))
            sign = sign.strip(' ')
            return sign
        else:
            # entire sign is contained in the function declaration
            m = re.search(sign_str, text)
            if not m:
                m = re.search(sign_str_2, text)
            if m:
                sign = re.sub('\s+', ' ', m.group(1))
                sign = sign.strip(' ')
                return sign
            else:
                return ''
                #raise Exception('float "form_volume(...)" function signature found')

    @staticmethod
    def __getHint(sign):
        sign = re.sub('float', '', sign)
        sign = re.sub('\s+', ' ', sign)
        return '(' + sign.strip(' ') + ')'

    @staticmethod
    def __getIqCall(pars_array_name, numpars, xy=False):
        # Iq(...) function begins with "float q" and Iqxy begins with "float qx, float qy"
        sign = 'q'
        if xy:
            sign = 'qx, qy'
        for i in range(numpars):
            sign += ', %s[%i]' % (pars_array_name, i)
        if xy:
            return 'Iqxy(%s)' % sign
        return 'Iq(%s)' % sign

    @staticmethod
    def __getFormVolCall(pars_array_name, num_form_vol_pars, real_iq_sign, form_vol_sign):

        # special case: form_volume signature is "void"
        if re.search('void', form_vol_sign):
            return 'form_volume()'

        # assemble form_volume call string as we go
        call_str = 'form_volume('

        # remove excess whitespace and "float"
        real_iq_sign = SasViewModelFileInfo.__getHint(real_iq_sign)
        form_vol_sign = SasViewModelFileInfo.__getHint(form_vol_sign)

        pars_iq = []
        m = re.finditer('(\w*)[,)]', real_iq_sign)
        for par in m:
            pars_iq.append(par.group(1))

        pars_fv = []
        m = re.finditer('(\w*)[,)]', form_vol_sign)
        for par in m:
            pars_fv.append(par.group(1))

        right_par_idcs = []
        for i in range(len(pars_fv)):
            for j in range(len(pars_iq)):
                if re.match(pars_fv[i], pars_iq[j]) and re.match(pars_iq[j], pars_fv[i]):
                    right_par_idcs.append(j)

            # error case: no match found for this form_volume parameter - exit (function returns unity)
            if len(right_par_idcs) == i:
                return '1'

            i += 1

        for idx in range(len(pars_fv)):
            call_str = call_str + 'pars[%d]' % right_par_idcs[idx]
            if idx != len(pars_fv)-1:
                call_str = call_str + ', '
            else:
                call_str = call_str + ')'

        return call_str


def getFiles(look_dir, extension):
    file_list = []
    for (dirpath, dirnames, filenames) in os.walk(look_dir):
        for f in filenames:
            if os.path.splitext(f)[1] == '.' + extension:
                if re.match(r'sas_', os.path.basename(f)):
                    file_list.append(os.path.abspath(os.path.join(dirpath,f)))
        break
    return sorted(file_list, key=lambda item: (int(item.lower().partition(' ')[0])
                                               if item.lower()[0].isdigit() else float('inf'), item.lower()))

def mod_comp_file_docs(comp_file, docs_section):
    text = open(comp_file).read()

    pos_D = text.find("MDOC")
    pos_end_D = text.find('MDOC_END')

    if pos_D == -1 or pos_end_D == -1:
        logging.exception('MDOC or MDOC_END flag error. positions: %d, %d' % (pos_D, pos_end_D))

    return text[:pos_D+4] + '\n' + docs_section + text[pos_end_D-2:]

def get_formatted_docs_text(info_lst, left_padding = 4, log_num_models = 2):
    # padding (asterisk-plus-whitespace indentation)
    pad_format_str = '{:<' + str(left_padding) + '}' # e.g. '{:<16}'

    # width of index count (1's 10's or 100's?)
    index_format_str = '{:>' + str(log_num_models) + '}' # e.g. '{:>2}'

    # width of model name
    max_len = 0
    for info in info_lst:
        if len(info.model_name) > max_len:
            max_len = len(info.model_name)
        if len(info.model_name_xy) > max_len:
            max_len = len(info.model_name_xy)
    name_format_str = '{:<' + str(max_len + 1) + '}' # e.g. '{:<35}'

    # make and return the doc lines
    text = pad_format_str.format('* ')
    text += '\n* <table border=1><tr><td><b>Model no.</b></td><td><b>Model name</b></td><td><b>Parameters</b></td></tr>\n* <tr><td>' + index_format_str.format(str(0)) + '</td><td>None</td><td>None</td></tr>\n'
    i = 1
    for info in info_lst:
        text += pad_format_str.format('* <tr><td>')
        text += index_format_str.format(str(i)) + '</td><td>' + '<a href="http://www.sasview.org/sasview/user/models/model_functions.html#'+ name_format_str.format(info.model_name).replace(" ", "").replace("_", "") + 'model">' + name_format_str.format(info.model_name).replace(" ", "") + '</a></td><td>' + info.Iq_hint + '</td></tr>\n'
        i += 1
        text += pad_format_str.format('* <tr><td>')
        text += index_format_str.format(str(i)) + '</td><td>' + '<a href="http://www.sasview.org/sasview/user/models/model_functions.html#'+ name_format_str.format(info.model_name_xy).replace(" ", "").replace(" ", "").replace("_xy", "") + 'model">' + name_format_str.format(info.model_name_xy).replace(" ", "") + '</a></td><td>' + info.Iq_xy_hint + '</td></tr>\n'
        i += 1
    text += '* </table>\n'

    # Include sasmodel parameter descriptions - exclude html header etc.
    sasdoc = open( os.path.join(args.cdir[0], "docs_sasmodels.html") )
    html_list = ['<!DOCTYPE html>','<html>','<body>', '</body>','</html>']
    for line in iter(sasdoc):
      if line not in html_list:
         text += "* " + line

    return text

def get_proxy_file_text(info_lst):
    include_section = ''
    i = 1
    for info  in info_lst:
        include_section += '  #if model_index == %d\n' % i
        include_section += '    %s\n' % info.percent_include
        include_section += '  #endif\n'
        i += 1
        include_section += '  #if model_index == %d\n' % i
        include_section += '    %s\n' % info.percent_include
        include_section += '  #endif\n'
        i += 1

    # Iq interface
    Iq_call_section =      '  float getIq(float q, float qx, float qy, float pars[])\n'
    Iq_call_section +=     '  {\n'
    Iq_call_section +=     '    float Iq_out = 1;\n'
    i = 1
    for info in info_lst:
        Iq_call_section += '    #if model_index == %d\n' % i
        Iq_call_section += '      Iq_out = %s;\n' % info.Iq_call
        Iq_call_section += '    #endif\n'
        i+=1
        Iq_call_section += '    #if model_index == %d\n' % i
        Iq_call_section += '      Iq_out = %s;\n' % info.Iqxy_call
        Iq_call_section += '    #endif\n'
        i+=1
    Iq_call_section +=     '    return Iq_out;\n'
    Iq_call_section +=     '  }\n'

    # form_volume interface
    form_vol_call_section =      '  float getFormVol(float pars[])\n'
    form_vol_call_section +=     '  {\n'
    form_vol_call_section +=     '    float form_vol;\n'
    i = 1
    for info in info_lst:
        form_vol_call_section += '    #if model_index == %d\n' % i
        form_vol_call_section += '      form_vol = %s;\n' % info.form_volume_call
        form_vol_call_section += '    #endif\n'
        i+=1
        form_vol_call_section += '    #if model_index == %d\n' % i
        form_vol_call_section += '      form_vol = %s;\n' % info.form_volume_call
        form_vol_call_section += '    #endif\n'
        i+=1
    form_vol_call_section +=     '    return form_vol;\n'
    form_vol_call_section +=     '  }\n'


    return include_section + '\n' + Iq_call_section + '\n' + form_vol_call_section

def main(args):
    logging.basicConfig(level=logging.INFO)

    logging.info('input comp file: %s', args.compfile[0])
    logging.info('model source dir: %s', args.cdir[0])


    # get model file info
    info_lst = []
    for f in getFiles(args.cdir[0].rstrip('/'), "c") :
        logging.info('integrating: %s', f)
        info_lst.append(SasViewModelFileInfo(f, 'pars'))

    # enable to print debug info
    if False:
        text = ''
        for info in info_lst:
            text = text + info.printMe()
            print(info.printMe())

        for info in info_lst:
            print(info.sign_form_vol)

        debug_file = os.path.splitext(os.path.basename(args.compfile[0]))[0] + '_modelinfo.txt'
        logging.info('output comp file: %s' % debug_file)
        f = open(debug_file, 'w')
        f.write(text)
        f.close()
        exit()

    # assemble proxy file

    # save proxy file
    f = open(os.path.join(os.path.abspath(args.cdir[0]), 'sasview_proxy.c'), 'w')
    f.write(get_proxy_file_text(info_lst))
    f.close()

    # get docs section for .comp file
    docs_section_text = get_formatted_docs_text(info_lst)

    # mod .comp file
    f = open(os.path.splitext(os.path.basename(args.compfile[0]))[0] , 'w')
    f.write(mod_comp_file_docs(args.compfile[0], docs_section_text))
    f.close()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('compfile', nargs=1, help='the SasView model component')
    parser.add_argument('cdir', nargs=1, help='directory containing sasmodel C-files, prefixed by "sas_"')

    args = parser.parse_args()

    main(args)
