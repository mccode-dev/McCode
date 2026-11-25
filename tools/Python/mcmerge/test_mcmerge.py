#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
Tests mcmerge on small synthetic datasets, run without arguments.
With real output: test_mcmerge.py REF A B ... checks that merging A B ... gives REF,
e.g. an MPI run with seed s on 2 nodes, and serial runs with seeds s and s+1.
'''
import os
import shutil
import subprocess
import sys
import tempfile

import h5py
import numpy as np

import mcmerge

SCRIPT = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'mcmerge.py')
SIM = '''mcstas simulation description file for t.
Date: now

begin instrument: t
  File: {d}/mccode
  Source: t.instr
end instrument

begin simulation: {d}
  Format: McCode with text headers
  URL: http://www.mccode.org
  Creator: 3
  Instrument: t.instr
  Ncount: {n}
  Trace: no
  Gravitation: no
  Seed: {seed}
  Directory: {d}
  Param: s=a=b
end simulation

{data}begin timing
  Init: 0 [s]
end timing
'''


def header(comp, kind, n, values, **more):
    h = dict(Date='now', type=kind, Source='t (t.instr)', component=comp, position='0 0 1', title=comp,
             Ncount=n, mean='no', filename=comp + '.dat', statistics='None', signal='None',
             values='%g %g %g' % values, xvar='x', yvar='(I,I_err)', xlabel='x', ylabel='y',
             xlimits='0 4', variables='x I I_err N')
    h.update(more)
    if 'xylimits' in h:
        del h['xlimits']
    if kind == 'array_0d':
        for k in ('xvar', 'yvar', 'xlabel', 'ylabel'):
            del h[k]
    return h


def hist(w, b, bins):
    ''' N, I, E as the runtime '''
    N, I, Q = [np.bincount(b, v, bins) for v in (np.ones(len(w)), w, w*w)]
    M = np.maximum(N, 1)
    return N, I, np.where(N > 1, np.sqrt(abs(Q - (I/M)**2)*M/np.maximum(M-1, 1)), I)


def rays(seed):
    r = np.random.default_rng(seed)
    return r.exponential(size=50)/100, r.integers(0, 4, 50)


def monitors(seeds):
    ''' header, axis, I, E, N of the monitors, as from one run over all seeds '''
    n, (w, b) = 100*len(seeds), [np.concatenate(v) for v in zip(*map(rays, seeds))]
    w = w/len(seeds)
    N, I, E = hist(w, b, 4)
    tN, tI, tE = hist(w, 0*b, 1)
    tot = (tI[0], tE[0], tN[0])
    x = np.arange(4) + 0.5
    stat = dict(mcmerge.statistics({'xlimits': '0 4'}, I))
    ev = header('ev', 'list(2, %i)' % len(w), n, (0, 0, len(w)), variables='p x ', xylimits='1 %i 1 2' % len(w))
    return [(header('m0', 'array_0d', n, tot), None, None, None, None),
            (header('m1', 'array_1d(4)', n, tot, **stat), x, I, E, N),
            (header('m2', 'array_2d(2, 2)', n, tot, xylimits='0 2 0 2'), None) +
            tuple(v.reshape(2, 2) for v in (I, E, N)),
            (ev, None, np.column_stack([w, b]), None, None)]


def put(path, text):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w') as f:
        f.write(text)


def ascii_run(d, seed, skip=('ev',)):
    data, n = '', 0
    for h, x, I, E, N in monitors([seed]):
        if h['component'] in skip:
            continue
        n = h['Ncount']
        head = ''.join('  %s: %s\n' % v for v in h.items())
        data += 'begin data\n%send data\n\n' % head
        rows = [h['values'] + '\n'] if I is None else map(mcmerge.row, I)
        if x is not None:
            rows = map(mcmerge.row, np.column_stack([x, I, E, N]))
        elif E is not None:
            rows = ['# Data [c/f] I:\n', *map(mcmerge.row, I), '# Errors [c/f] I_err:\n', *map(mcmerge.row, E),
                    '# Events [c/f] N:\n', *map(mcmerge.row, N)]
        put(os.path.join(d, h['filename']), '# Seed: %i\n' % seed + head.replace('  ', '# ') + ''.join(rows))
    put(os.path.join(d, 'mccode.sim'), SIM.format(d=d, n=n, seed=seed, data=data))


def nexus_run(d, seed, skip=('ev',)):
    os.makedirs(d)
    with h5py.File(os.path.join(d, 'mccode.h5'), 'w') as f:
        e = f.create_group('entry1')
        e['timing_finally'] = [b'0']
        e['instrument/description'] = [b'source']
        e['instrument'].attrs.update(name='t', Source='t.instr')
        e.create_group('simulation/Param').attrs['s'] = 'a=b'
        e['simulation'].attrs.update(Format='NeXus', URL='http://www.mccode.org', program='3', Trace='no',
                                     Instrument='t.instr', Gravitation='no', Seed=str(seed), Ncount_requested='100')
        for h, x, I, E, N in monitors([seed]):
            if h['component'] in skip:
                continue
            g = e.create_group('instrument/components/%s/output/%s_dat' % ((h['component'],)*2))
            for k, v in h.items():
                g.attrs[k] = str(v)
            if E is None and I is not None:     # event list, as from MPI
                with h5py.File(os.path.join(d, 'mccode_node0.h5'), 'w') as node:
                    node['ev'] = I
                layout = h5py.VirtualLayout(I.shape, float)
                layout[:] = h5py.VirtualSource('mccode_node0.h5', 'ev', I.shape)
                g.create_virtual_dataset('events', layout)
            elif I is not None:
                for k, v in (('data', I), ('errors', E), ('ncount', N)):
                    g[k] = v.T
            if x is not None:
                g['x'] = x


def edit(path, old, new):
    with open(path) as f:
        text = f.read()
    assert old in text, old
    put(path, text.replace(old, new))


def mcmerge_run(*args, error=None, warning=''):
    ''' Runs mcmerge to out. With error: must fail with that message and leave nothing '''
    shutil.rmtree('out', ignore_errors=True)
    p = subprocess.run([sys.executable, SCRIPT, '-d', 'out', *args], capture_output=True, text=True)
    assert not os.path.exists('out.part')
    if error:
        assert p.returncode and error in p.stderr and not os.path.exists('out'), (error, p.stderr)
    else:
        assert p.returncode == 0 and warning in p.stderr, p.stderr
    return p


def same(ref, out):
    ''' Datasets are equal: N exactly, data to 1e-8, totals to 2e-5. Polarisation to 1e-2, as sums of weights are not in the files '''
    a, b = mcmerge.read(ref), mcmerge.read(out)
    assert len(a) == len(b)
    for r, o in zip(a, b):
        assert set(r['mons']) == set(o['mons'])
        for key, m in r['mons'].items():
            q, pol = o['mons'][key], 'Pol' in m['head']['title']
            assert m['n'] == q['n'], key
            if m['list']:
                A, B = [v[np.lexsort(v.T[1:])] for v in map(mcmerge.load_list, (m, q))]
                assert np.allclose(A, B, 1e-8, 0), key
                continue
            t, u = [np.array(v['head']['values'].split(), float) for v in (m, q)]
            assert np.allclose(t[:2], u[:2], 1e-2 if pol else 2e-5, 0) and t[2] == u[2], (key, t, u)
            if m['head']['type'] != 'array_0d':
                assert pol or all(m['head'][k] == q['head'][k] for k in ('statistics', 'signal')), key
                for v, w in zip(mcmerge.load(m)[1:], mcmerge.load(q)[1:]):
                    assert np.allclose(v, w, 1e-2 if pol else 1e-8, 0), key
                assert np.array_equal(v, w), key


def test_synthetic():
    for i in (1, 2, 3):
        ascii_run('a%i' % i, i)
        nexus_run('n%i' % i, i)
    ref = monitors([1, 2, 3])
    edit('a1/mccode.sim', 'Directory: a1', 'Directory: a1\n  Nodes: 1')

    def good(tol=1e-8):
        mons = list(mcmerge.read('out')[0]['mons'].values())
        for (h, x, I, E, N), m in zip(ref, mons):
            assert m['n'] == 300
            assert np.allclose([float(v) for v in h['values'].split()],
                               [float(v) for v in m['head']['values'].split()], 2e-5)
            if I is not None:
                assert all(np.allclose(v, w, tol, 0) for v, w in zip((I, E, N), mcmerge.load(m)[1:]))
            if x is not None:
                assert m['head']['signal'] == h['signal'] and m['head']['statistics'] == h['statistics']

    # formats, mix, orders, rerun
    for names in ('a1 a2 a3', 'a3 a1 a2', 'a2 a3 a1'):
        mcmerge_run(*names.split())
        good()
    mcmerge_run('n1', 'n2', 'n3')
    good(1e-12)
    for names in ('a1 n2 a3', 'n1 a2 n3'):
        mcmerge_run(*names.split())
        assert os.path.exists('out/mccode.h5')
        good()
        mcmerge_run('--format', 'McCode', *names.split(), warning='left out')
        assert os.path.exists('out/mccode.sim') and not os.path.exists('out/mccode.h5')
        good()
    for a, b, c in (('a1', 'a2', 'a3'), ('n1', 'n2', 'n3'), ('a1', 'n2', 'a3')):
        mcmerge_run(a, b)
        shutil.move('out', 'ab')
        mcmerge_run('ab', c)
        good()
        head = mcmerge.read('out')[0]['head']
        assert head['Seed'] == '-1' and 'Nodes' not in head
        assert [s[:2] for s in mcmerge.read('out')[0]['seeds']] == [(1, 2), (2, 3), (3, 4)]
        mcmerge_run('ab', b, error='same Seed')
        shutil.rmtree('ab')

    # monitors only in NeXus
    ascii_run('few', 1, skip=('ev', 'm1', 'm2'))
    for f in ((), ('--format', 'McCode')):
        mcmerge_run(*f, 'few', 'n2', 'n3', warning='monitor m1 is only in the NeXus data')
        mons = mcmerge.read('out')[0]['mons']
        assert [m['n'] for m in mons.values()] == [300, 200, 200]
        assert np.allclose(mcmerge.load(mons['m2', 1])[1], monitors([2, 3])[2][2], 1e-8)

    # event lists
    for i in (1, 2):
        ascii_run('e%i' % i, i, skip=())
    mcmerge_run('e1', 'e2')
    m = mcmerge.read('out')[0]['mons']['ev', 1]
    assert m['head']['type'] == 'list(2, 100)' and m['head']['xylimits'] == '1 100 1 2'
    assert m['head']['values'] == '0 0 100'
    assert np.allclose(np.loadtxt(m['file']), monitors([1, 2])[3][2], 1e-9)
    shutil.move('out', 'e12')
    mcmerge_run('e1', 'n2', error='not in both')
    ascii_run('e3', 3, skip=())
    mcmerge_run('e12', 'e3')
    m = mcmerge.read('out')[0]['mons']['ev', 1]
    assert np.allclose(np.loadtxt(m['file']), monitors([1, 2, 3])[3][2], 1e-9)
    for i in (1, 2, 3):
        nexus_run('f%i' % i, i, skip=())
    for a, b, c in (('f1', 'f2', 'f3'), ('e1', 'f2', 'e3'), ('f1', 'f2', 'e3')):
        mcmerge_run(a, b)
        shutil.move('out', 'ab')
        for f in ('NeXus', 'McCode')[:2 if c == 'e3' else 1]:
            mcmerge_run('--format', f, 'ab', c)
            m = mcmerge.read('out')[0]['mons']['ev', 1]
            assert m['head']['type'] == ('list(150, 2)' if f == 'NeXus' else 'list(2, 150)')
            assert m['head']['values'] == '0 0 150' and m['head']['xylimits'] == '1 150 1 2'
            assert np.allclose(mcmerge.load_list(m), monitors([1, 2, 3])[3][2], 1e-9)
        shutil.rmtree('ab')
    os.remove('f1/mccode_node0.h5')
    mcmerge_run('f1', 'f2', error='mccode_node0.h5 is missing')
    edit('e3/mccode.sim', 'values: 0 0 50', 'values: 0 0 51')
    mcmerge_run('e12', 'e3', error='truncated')
    edit('e3/mccode.sim', 'values: 0 0 51', 'values: 0 0 0')
    mcmerge_run('e12', 'e3', error='older version')
    for d in ('e1', 'e2'):
        edit(d + '/mccode.sim', 'variables: p x', 'variables: p I')
    mcmerge_run('e1', 'e2', error='second weight')

    # scan
    for i in (1, 2):
        row = ''
        for k in range(2):
            d = 's%i/%i' % (i, k)
            ascii_run(d, 10*k + i)
            edit(d + '/mccode.sim', 's=a=b', 's=%i' % k)
            row += '%i %s\n' % (k, ' '.join(monitors([10*k + i])[1][0]['values'].split()[:2]))
        put('s%i/mccode.dat' % i, '# Ncount: 100\n# variables: s m1_I m1_ERR\n' + row)
        put('s%i/mccode.sim' % i, 'begin simulation\nNcount: 100\nend simulation\n')
    mcmerge_run('s1', 's2')
    for k, line in enumerate(np.loadtxt('out/mccode.dat')):
        v = monitors([10*k + 1, 10*k + 2])[1][0]['values'].split()
        assert np.allclose(line, [k] + [float(s) for s in v[:2]], 2e-5)
    assert 'Ncount: 200' in open('out/mccode.sim').read()
    shutil.rmtree('s2/1')
    mcmerge_run('s1', 's2', error='number of scan points')

    # no errors at all, or not in one
    for i in (1, 2):
        ascii_run('x%i' % i, i)
        path = 'x%i/m2.dat' % i
        with open(path) as f:
            text = f.read()
        put(path, text[:text.index('# Errors')] + text[text.index('# Events'):])
    mcmerge_run('x1', 'x2', warning='no errors')
    assert not mcmerge.load(mcmerge.read('out')[0]['mons']['m2', 1])[2].any()
    mcmerge_run('x1', 'a2', error='only in some')


def test_refused():
    ascii_run('a1', 1)
    nexus_run('n1', 1)
    mcmerge_run('a1', error='at least two')
    mcmerge_run('a1', 'none', error='no mccode.sim')
    for file, old, new, error in (
            ('mccode.sim', 's=a=b', 's=a=c', 'Param: s'),
            ('mccode.sim', 'Gravitation: no', 'Gravitation: yes', 'Gravitation'),
            ('mccode.sim', 'Trace: no', 'Trace: yes', 'Trace'),
            ('mccode.sim', 'Creator: 3', 'Creator: 4', 'Creator'),
            ('mccode.sim', 'instrument: t', 'instrument: u', 'Instrument name'),
            ('mccode.sim', 'mean: no', 'mean: yes', 'mean'),
            ('mccode.sim', '  mean: no\n', '', 'no "mean: yes/no"'),
            ('mccode.sim', 'Ncount: 100\n  mean', 'ratio: 50/100\n  mean', 'not finished'),
            ('mccode.sim', 'component: m1', 'component: m3', 'm1 is not in both'),
            ('mccode.sim', 'xlimits: 0 4', 'xlimits: 0 5', 'xlimits'),
            ('mccode.sim', 'begin timing', 'begin nothing', 'not finished'),
            ('mccode.sim', 'begin simulation', 'begin simulation: x\nend simulation\nbegin simulation', 'intermediate'),
            ('mccode.sim', 'Seed: 2', 'Seed: 1', 'same Seed'),
            ('mccode.sim', 'Seed: 2', 'Seed: 0\n  Nodes: 2', 'same Seed'),
            ('mccode.sim', 'Seed: 2', 'Seed: x', 'bad Seed'),
            ('mccode.sim', 'Seed: 2', 'Merged_from: a2, Seed 2', 'Merged_from'),
            ('mccode.sim', 'array_2d(2, 2)', 'array_3d(2, 2, 2)', 'not supported'),
            ('mccode.sim', 'filename: m1.dat', 'filename: ../m1.dat', 'a path'),
            ('m1.dat', '\n0.5 ', '\n0.51 ', 'axis values'),
            ('m2.dat', '# Events', '# Data', 'saved more than once')):
        ascii_run('a2', 2)
        edit('a2/' + file, old, new)
        mcmerge_run('a1', 'a2', error=error)
        if file == 'mccode.sim' and error in ('Param: s', 'Gravitation', 'Creator', 'mean', 'xlimits'):
            mcmerge_run('n1', 'a2', error=error)
    ascii_run('a2', 2)
    mcmerge_run('--format', 'NeXus', 'a1', 'a2', error='needs an input')
    for change, error in ((lambda e: e.__delitem__('timing_finally'), 'not finished'),
                          (lambda e: e.__setitem__('simulation/Nodes', [b'1']), None),
                          (lambda e: e['simulation/Param'].attrs.__setitem__('s', 'c'), 'Param: s'),
                          (lambda e: e['instrument/description'].__setitem__(0, b'other'), 'Instrument source'),
                          (lambda e: e['simulation'].attrs.__setitem__('Merged_from', 'x'), 'Merged_from'),
                          (lambda e: e['simulation'].attrs.__setitem__('Seed', '1'), 'same Seed')):
        shutil.rmtree('n2', ignore_errors=True)
        nexus_run('n2', 2)
        with h5py.File('n2/mccode.h5', 'r+') as f:
            change(f['entry1'])
        mcmerge_run('n1', 'n2', error=error)

    # out exists; left out files; cleanup after an exception
    put('a2/x.mcpl', '')
    assert 'x.mcpl' in mcmerge_run('a1', 'a2').stdout and not os.path.exists('out/x.mcpl')
    p = subprocess.run([sys.executable, SCRIPT, '-d', 'out', 'a1', 'a2'], capture_output=True, text=True)
    assert p.returncode == 255 and 'exists' in p.stderr
    edit('a2/m2.dat', '# Errors', 'garbage\n# Errors')
    mcmerge_run('a1', 'a2', error='ValueError')


if __name__ == '__main__':
    inputs = [os.path.abspath(p) for p in sys.argv[1:]]
    for test in (test_synthetic, test_refused) if not inputs else (None,):
        with tempfile.TemporaryDirectory() as tmp:
            os.chdir(tmp)
            if test:
                test()
            else:
                mcmerge_run(*inputs[1:])
                same(inputs[0], 'out')
            os.chdir('/')
    print('ok')
