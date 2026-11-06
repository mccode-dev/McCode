''' pencolor state in the MCDISPLAY parser. Run from tools/Python: python -m mccodelib.test_pencolor (or pytest) '''
from mccodelib.instrparser import InstrTraceParser, InstrObjectConstructor

TRACE = '''INSTRUMENT:
Instrument 'Test_pencolor' (Test_pencolor.instr)
COMPONENT: "p"
POS: 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1
COMPONENT: "q"
POS: 0, 0, 2, 1, 0, 0, 0, 1, 0, 0, 0, 1
MCDISPLAY: start
MCDISPLAY: component p
MCDISPLAY: multiline(2,0,0,0,0.1,0,0)
MCDISPLAY: pencolor(255,0,0)
MCDISPLAY: multiline(2,0,0,0,0,0.1,0)
MCDISPLAY: pencolor(0,255,128)
MCDISPLAY: mcdiscircle('xy',0,0,0,0.05)
MCDISPLAY: component q
MCDISPLAY: multiline(2,0,0,0,0.1,0,0)
MCDISPLAY: end
INSTRUMENT END:
'''

def test_pencolor():
    instr = InstrObjectConstructor(InstrTraceParser(TRACE).parsetree).build_instr()
    p, q = [[(d['key'], d.get('color')) for d in c.jsonize()['drawcalls']] for c in instr.components]
    assert p == [('multiline', None), ('multiline', '#ff0000'), ('circle', '#00ff80')], p
    assert q == [('multiline', None)], q  # pen resets per component

if __name__ == '__main__':
    test_pencolor()
    print('OK')
