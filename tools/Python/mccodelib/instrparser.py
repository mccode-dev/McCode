'''
Implementation of classes involved in the PLY-based translation of the mcdisplay "--trace output" 
mini language.

Read the PLY documentation here: http://www.dabeaz.com/ply/ply.html#ply_nn23.
'''
from ply import lex, yacc
from .nodetree import Node
from .instrgeom import InstrumentSpecific, Component, Vector3d, Matrix3, drawclass_factory, DrawCommand

class InstrTraceParser:
    '''
    Parser for the instrument definition section of mcdisplay --trace output.
    '''
    parsetree = None
    def __init__(self, data=None, debug=False):
        self.mantid = None

        
        self.debug = debug
        self.build_lexer()
        self.build_parser()
        if data:
            self.parse(data)
    
    # these tokens match ID, but are of these types (handled in t_ID)
    reserved = {
        'INSTRUMENT'  : 'INSTRUMENT',
        'END'         : 'END',
        'Instrument'  : 'INSTRKW',
        'COMPONENT'   : 'COMPONENT',
        'POS'         : 'POS',
        'MCDISPLAY'   : 'MCDISPLAY',
        'start'       : 'STARTKWLC',
        'end'         : 'ENDKWLC',
        'component'   : 'COMPKWLC',

        'multiline'   : 'DRAWCALL',
        'mcdisrectangle'   : 'DRAWCALL',
        'mcdisbox'         : 'DRAWCALL',
        'mcdiscircle'      : 'DRAWCALL',

        'mcdissphere'      : 'DRAWCALL',
        'mcdiscone'        : 'DRAWCALL',
        'mcdiscylinder'    : 'DRAWCALL',
        'mcdisdisc'        : 'DRAWCALL',
        'mcdisannulus'     : 'DRAWCALL',
        'mcdisnew_circle'  : 'DRAWCALL',
        'polygon'     : 'DRAWCALL',
        'polyhedron'  : 'DRAWCALL',


        'MANTID_PIXEL': 'MANTID_PIXEL',
        'MANTID_BANANA_DET': 'MANTID_BANANA_DET',
        'MANTID_RECTANGULAR_DET': 'MANTID_RECTANGULAR_DET',

    }

    # tokens 
    tokens = [
              'LB',
              'RB',
              'COLON',
              'QUOTE',
              'SQUOTE',
              'COMMA',
              'NL',
              'INSTRPATH',
              'DEC',
              'ID',
              'INSTRNAME',
              'JSON',
             ] + list(set(reserved.values()))
    
    t_LB = r'\('
    t_RB = r'\)'
    t_COLON = r':'
    t_QUOTE = r'"'
    t_SQUOTE = r'\''
    t_COMMA = r','

    def t_ANY_NL(self, t):
        r'\n'
        self.lexer.lineno += 1
        return t
    
    def t_INSTRPATH(self, t):
        r'[/\w\\\:\-\.]+\.instr'
        return t
    
    def t_DEC(self, t):
        r'-?[\d.]+(e[+-][0-9]+)?'
        return t
    
    def t_ID(self, t):
        r'[a-zA-Z_]\w*'
        t.type = self.reserved.get(t.value, 'ID')
        return t

    def t_INSTRNAME(self, t):
        r'\w[\w\-0-9\.]*'
        return t

    def t_JSON(self, t):
        r'\{.*\}'
        return t
    
    # ignore whitespaces and tabs means that we do not tokenize them, but they are still applied in the regex checks defined above for our tokens
    t_ignore = ' \t'
    
    def t_error(self, t):
        print('error: %s' % t.value)
    
    ##################################
    # parsing rules and action code
    ##################################
    
    def p_document(self, p):
        'document : instr_open comp_defs draw_lines instr_end'
        print('instrument definition parsed')
        # quirky: reverse ordering of components
        self.comps.children = self.comps.children[::-1]
        # assemble parse tree
        if not self.mantid: 
            self.parsetree = Node(type='instrdeftree', children=[self.instr, self.comps])
        else:
            self.parsetree = Node(type='instrdeftree', children=[self.instr, self.comps, self.mantid])
    
    instr = None
    def p_instr_open(self, p):
        '''instr_open : INSTRUMENT COLON NL INSTRKW SQUOTE instr_name SQUOTE LB INSTRPATH RB NL
                      | INSTRUMENT COLON NL INSTRKW SQUOTE instr_name SQUOTE LB ID RB NL'''
        self.instr = Node(type='instrument', children=[p[6], Node(type='abspath', leaf=p[9])])
    
    def p_instr_end(self, p):
        'instr_end : INSTRUMENT END COLON NL'
        
    def p_instr_name(self, p):
        '''instr_name : INSTRNAME
                      | ID '''
        p[0] = Node(type='instr_name', leaf=p[1])
    
    comps = Node(type='comps')
    def p_comp_defs(self, p):
        '''comp_defs : comp_def comp_defs
                     | comp_def '''
        self.comps.children.append(p[1])
        p[0] = self.comps
    
    def p_comp_def(self, p):
        'comp_def : COMPONENT COLON QUOTE comp_name QUOTE NL POS COLON m4 NL'
        v3 = Node(type='v3', leaf=[p[9].leaf[0], p[9].leaf[1], p[9].leaf[2]])
        p[0] = Node(type='comp', children=[p[4], p[9], v3])
    
    def p_comp_name(self, p):
        'comp_name : ID'
        p[0] = Node(type="comp_name", leaf=p[1])
    
    def p_m4(self, p):
        'm4 : DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC'
        p[0] = Node(type='m4', leaf=[p[1], p[3], p[5], p[7], p[9], p[11], p[13], p[15], p[17], p[19], p[21], p[23]])
    
    def p_draw_lines(self, p):
        '''draw_lines : draw_line draw_lines
                      | draw_line '''
    
    def p_draw_line(self, p):
        '''draw_line : draw_header
                     | draw_command
                     | draw_open
                     | draw_close'''
    
    def p_draw_header(self, p):
        '''draw_header : MCDISPLAY COLON COMPKWLC comp_name NL'''
        self.commands = Node(type='draw_commands')
        for comp in self.comps.children:
            for c in comp.children:
                if c.leaf==p[4].leaf:
                    comp.children.append(self.commands)
                    break
    
    commands = None
    def p_draw_command(self, p):
        '''draw_command : MCDISPLAY COLON DRAWCALL LB args RB NL
                        | MCDISPLAY COLON DRAWCALL LB SQUOTE arg SQUOTE RB NL
                        | MCDISPLAY COLON DRAWCALL LB SQUOTE arg SQUOTE COMMA args RB NL
                        | MCDISPLAY COLON DRAWCALL LB SQUOTE SQUOTE RB NL
                        | MCDISPLAY COLON DRAWCALL LB RB NL
                        | MCDISPLAY COLON DRAWCALL json NL
                        | MANTID_PIXEL COLON nineteen_dec NL
                        | MANTID_BANANA_DET COLON eight_dec NL
                        | MANTID_RECTANGULAR_DET COLON seven_dec NL
        '''
        if p[1] in ['MANTID_PIXEL', 'MANTID_BANANA_DET', 'MANTID_RECTANGULAR_DET']:
            if not self.mantid:
                self.mantid = Node(type='mantid', leaf={})
                self.mantid.leaf['MANTID_PIXEL'] = []
                self.mantid.leaf['MANTID_BANANA_DET'] = []
                self.mantid.leaf['MANTID_RECTANGULAR_DET'] = []
            if p[1] == 'MANTID_PIXEL':
                self.mantid.leaf['MANTID_PIXEL'].append(p[3])
                self.commands.children.append(Node(type='mantid', children=[], leaf=MantidPixelLine(p[3])))
            elif p[1] == 'MANTID_RECTANGULAR_DET':
                self.mantid.leaf['MANTID_RECTANGULAR_DET'].append(p[3])
                self.commands.children.append(Node(type='mantid', children=[], leaf=MantidRectangularDetectorLine(p[3])))
            elif p[1] == 'MANTID_BANANA_DET':
                self.mantid.leaf['MANTID_BANANA_DET'].append(p[3])
                self.commands.children.append(Node(type='mantid', children=[], leaf=MantidBananaDetectorLine(p[3])))
            return
        
        # special case: remove first argument of args
        if p[3] == 'multiline':
            self.args.leaf = self.args.leaf[1:]
        
        self.commands.children.append(Node(type='draw', children=[self.args], leaf=p[3]))
        # reset args after having parsed them all, which is now
        self.args = Node(type='args', leaf=[])
    
    def p_nineteen_dec(self, p):
        ''' nineteen_dec : DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC
        '''
        p[0] = [p[2*i+1] for i in range(19)]
    
    def p_eight_dec(self, p):
        ''' eight_dec : DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC
        '''
        p[0] = [p[2*i+1] for i in range(8)]
    
    def p_seven_dec(self, p):
        ''' seven_dec : DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC COMMA DEC
        '''
        p[0] = [p[2*i+1] for i in range(7)]
    
    args = Node(type='args', leaf=[])
    def p_args(self, p):
        '''args : arg COMMA args
                | arg'''
    
    def p_arg(self, p):
        '''arg : DEC
               | ID'''
        self.args.leaf.append(p[1])

    def p_json(self, p):
        'json : JSON'
        self.args.leaf.append(p[1])

    def p_draw_open(self, p):
        '''draw_open : MCDISPLAY COLON STARTKWLC NL'''
    
    def p_draw_close(self, p):
        'draw_close : MCDISPLAY COLON ENDKWLC NL'
    
    # error rule for syntax errors
    def p_error(self, p):
        print("Syntax error in input!")
        print(p)

    ##################################
    # build and test 
    ##################################
    
    def build_lexer(self, **kwargs):
        ''' builds the lexer '''
        self.lexer = lex.lex(module=self, **kwargs)
    
    def test_lexer(self, data):
        ''' test built lexer on data '''
        self.lexer.input(data)
        for token in self.lexer:
            print(token)
    
    def build_parser(self, **kwargs):
        ''' builds the yaccer '''
        self.parser = yacc.yacc(module=self, debug=self.debug, write_tables=False)

    def parse(self, data):
        ''' attempts to parse data '''
        self.parser.parse(data, lexer=self.lexer)


class InstrObjectConstructor:
    '''
    Instrument reconstruction from the syntax tree produced by InstrTraceParser
    '''
    def __init__(self, parsetreeroot):
        self.root = parsetreeroot
        if (type(self.root) is not Node) or (self.root.type != 'instrdeftree'):
            raise Exception('InstrObjectConstructor: parsetreeroot must be a Node of type "instrdeftree"')
    
    def build_instr(self):
        ''' builds and returns the instrument representation object '''
        
        # create instrument object
        instrument_tree = InstrumentSpecific(name='', params=[], params_defaults=[])
        
        # for internal use
        self.compindices = {}
        comp_idx = 0
        
        # iterate through parse tree, parsing instr information and components
        for dc in self.root.children[0:2]:
            # handle instrument branch
            if dc.type == 'instrument':
                for ic in dc.children:    
                    if ic.type == 'instr_name':
                        instrument_tree.name = ic.leaf
                    if ic.type == 'abspath':
                        instrument_tree.abspath = ic.leaf
            
            # handle component branch
            if dc.type == 'comps':
                for csc in dc.children:
                    
                    if csc.type == 'comp':
                        name = ''
                        pos = None
                        rot = None
                        
                        # get name, pos and rot
                        for cc in csc.children:
                            if cc.type == 'comp_name':
                                name = cc.leaf
                            if cc.type == 'm4':
                                pos = Vector3d(x=float(cc.leaf[0]), y=float(cc.leaf[1]), z=float(cc.leaf[2]))
                                # transpose rotation to conform from mcstas trans rot convention to 4x4 transform notation
                                rot = Matrix3(
                                        a11=float(cc.leaf[3]),
                                        a21=float(cc.leaf[4]),
                                        a31=float(cc.leaf[5]),
                                        a12=float(cc.leaf[6]),
                                        a22=float(cc.leaf[7]),
                                        a32=float(cc.leaf[8]),
                                        a13=float(cc.leaf[9]),
                                        a23=float(cc.leaf[10]),
                                        a33=float(cc.leaf[11])
                                        )

                        comp = Component(name=name, pos=pos, rot=rot)
                        
                        # get draw commands (please print a parse tree with NodeTreePrint to understand this)
                        for cc in csc.children:
                            if cc.type == 'draw_commands':
                                for dc in cc.children:
                                    if dc.type == 'mantid':
                                        comp.drawcalls.append(dc.leaf)
                                        continue

                                    # get args
                                    argsnode = dc.children[0]
                                    args = argsnode.leaf
                                    
                                    commandname = dc.leaf
                                    draw = drawclass_factory(commandname, args, reduced=True)
                                    if draw:
                                        comp.drawcalls.append(draw)
                        
                        instrument_tree.components.append(comp)
                        
                        # save component in dictionary-by-name for internal use
                        comp_idx += 1
                        self.compindices[name] = comp_idx

        return instrument_tree

class MantidPixelLine(DrawCommand):
    def __init__(self, line_lst):
        self.line = line_lst
        super().__init__()

class MantidRectangularDetectorLine(DrawCommand):
    def __init__(self, line_lst):
        self.line = line_lst
        super().__init__()

class MantidBananaDetectorLine(DrawCommand):
    def __init__(self, line_lst):
        self.line = line_lst
        super().__init__()
