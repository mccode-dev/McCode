#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
Merges McStas/McXtrace datasets from separate runs of the same simulation, as if
they were one longer run. The output is a copy of the first dataset of the output
format, where only the numbers are patched. Any real difference is refused.
'''
import argparse
import os
import re
import shutil
import subprocess
import sys

import h5py
import numpy as np

# Header keys that may differ. 'Format' is the file format.
IGNORE = set('Date Directory File Format Seed Nodes Ncount filename values statistics signal Merged_from'.split())
KEYS = ('Date type Source component position title Ncount mean filename statistics signal '
        'values xvar yvar xlabel ylabel zvar zlabel xlimits xylimits variables').split()
ARRAYS = ('data', 'errors', 'ncount')
MCPL = r'.*\.mcpl(\.gz)?'
FORMAT = {'McCode': 'ASCII', 'NeXus': 'NeXus'}


def refuse(msg):
    print('Error: ' + msg, file=sys.stderr)
    exit(-1)


def warn(msg, seen=set()):
    if msg not in seen:
        print('Warning: ' + msg, file=sys.stderr)
        seen.add(msg)


def S(v):
    return v.decode('utf-8', 'surrogateescape') if isinstance(v, bytes) else str(v)


def fopen(path, mode='r'):
    return open(path, mode, encoding='utf-8', errors='surrogateescape')


def lines_of(path):
    with fopen(path) as f:
        return f.readlines()


# ---- reading: a run is one simulation, either a whole dataset or a scan point

def make_run(where, fmt, path, head, merged, mons, **more):
    ''' Checks one run. mons: list of monitors, each a dict with its header '''
    if not mons:
        refuse('%s: no monitors' % where)
    count = {}
    run = dict(more, where=where, fmt=fmt, path=path, head=head, mons={})
    # monitors of a component are in different order in the two formats, so sort
    for m in sorted(mons, key=lambda m: m['head'].get('filename', '')):
        h = m['head']
        m['where'] = '%s: monitor %s' % (where, h.get('component'))
        if h.get('mean') not in ('yes', 'no'):
            refuse(m['where'] + ': no "mean: yes/no" in header, the data is from an older version')
        if not re.fullmatch('[1-9][0-9]*', h.get('Ncount', '')):
            refuse(m['where'] + ': no valid Ncount, the run was not finished')
        if not re.match(r'array_[012]d|list', h.get('type', '')):
            refuse(m['where'] + ': type "%s" is not supported' % h.get('type'))
        name = h.get('filename', '')
        if not name or os.path.basename(name) != name:
            refuse(m['where'] + ': no filename, or a path: "%s"' % name)
        m['n'] = int(h['Ncount'])
        m['list'] = h['type'].startswith('list')
        c = h['component']
        count[c] = count.get(c, 0) + 1
        run['mons'][c, count[c]] = m
    run['n'] = max(m['n'] for m in mons)
    run['from'] = merged or ['%s (%s), Ncount %i, Seed %s, Nodes %s' % (
        os.path.basename(os.path.abspath(path)), fmt, run['n'], head.get('Seed'), head.get('Nodes', 1))]
    run['seeds'] = []
    for line in run['from']:
        m = re.fullmatch(r'.* \((?:ASCII|NeXus)\), Ncount \d+, Seed (-?\d+), Nodes (\d+)', line)
        if not m:
            refuse('%s: bad Seed, Nodes or Merged_from: "%s"' % (where, line))
        run['seeds'].append((int(m[1]), int(m[1]) + int(m[2]), where))
    return run


def blocks(lines):
    ''' (name, first line, last line) of the begin/end blocks of a mccode.sim '''
    res = []
    for i, line in enumerate(lines):
        if line.startswith('begin '):
            res.append([line.split()[1].rstrip(':'), i, len(lines)])
        elif line.startswith('end ') and res:
            res[-1][2] = i
    return res


def read_ascii(path, sub):
    d = os.path.join(path, sub)
    lines = lines_of(os.path.join(d, 'mccode.sim'))
    head, merged, mons, found = {}, [], [], []
    for name, i, j in blocks(lines):
        found.append(name)
        h = {}
        for line in lines[i+1:j]:
            k, _, v = line[2:].rstrip('\n').partition(': ')
            if k == 'Param':
                k, _, v = (k + ': ' + v).partition('=')
            if k == 'Merged_from':
                merged.append(v)
            h[k] = v
        if name == 'data':
            mons.append(dict(head=h, block=(i, j), file=os.path.join(d, h.get('filename', ''))))
        elif name in ('instrument', 'simulation'):
            head.update(h)
        if name == 'instrument':
            head['Instrument name'] = lines[i].partition(':')[2].strip()
    if 'timing' not in found:
        refuse(d + ': the run was not finished (no timing in mccode.sim)')
    if found.count('simulation') != 1:
        refuse(d + ': intermediate saves are not supported')
    source = os.path.join(path, head['Source'])
    if os.path.isfile(source):
        head['Instrument source'] = ''.join(lines_of(source)).strip()
    return make_run(d, 'ASCII', path, head, merged, mons, sub=sub, lines=lines)


def read_nexus(path, entry, e):
    where = '%s: %s' % (path, entry)
    if 'timing_finally' not in e:
        refuse(where + ': the run was not finished, or is an intermediate save')
    sim, instr = e['simulation'], e['instrument']
    head = {k: S(v) for g in (sim, instr) for k, v in g.attrs.items()}
    head = {k: head[k] for k in ('Source Parameters Trace_enabled Default_main Embedded_runtime URL '
                                 'Instrument Trace Gravitation Seed').split() if k in head}
    head['Creator'] = S(sim.attrs['program'])
    head['Instrument name'] = S(instr.attrs['name'])
    head['Instrument source'] = '\n'.join(S(instr['description'][0]).splitlines()).strip()
    if 'Nodes' in sim:
        head['Nodes'] = S(sim['Nodes'][0])
    for k, v in sim['Param'].attrs.items():
        if k != 'NX_class':
            head['Param: ' + k] = S(v)
    mons = []
    outputs = [c['output'] for c in instr['components'].values() if 'output' in c]
    for g in [g for o in outputs for k, g in o.items() if k != 'BINS']:
        mons.append(dict(head={k: S(g.attrs[k]) for k in KEYS if k in g.attrs}, group=g.name,
                         file=os.path.join(path, 'mccode.h5')))
    merged = S(sim.attrs.get('Merged_from', '')).splitlines()
    return make_run(where, 'NeXus', path, head, merged, mons, entry=entry)


def read(path):
    ''' The runs of a dataset: one, or one per scan point '''
    if os.path.isfile(os.path.join(path, 'mccode.h5')):
        with h5py.File(os.path.join(path, 'mccode.h5'), 'r') as f:
            return [read_nexus(path, k, f[k]) for k in sorted(f, key=lambda k: int(k[5:]))]
    if not os.path.isfile(os.path.join(path, 'mccode.sim')):
        refuse(path + ': no mccode.sim or mccode.h5')
    steps = [s for s in os.listdir(path) if s.isdigit() and os.path.isdir(os.path.join(path, s))]
    return [read_ascii(path, s) for s in sorted(steps, key=int) or ['']]


def load(m):
    ''' Axis (1D), I, E, N of a monitor. 2D as [y, x]. E, N may be None '''
    h = m['head']
    if h['type'] == 'array_0d':
        return [None] + [np.array(float(v)) for v in h['values'].split()]
    if 'group' in m:
        with h5py.File(m['file'], 'r') as f:
            g = f[m['group']]
            D = [g[k][()].T if k in g else None for k in ARRAYS]
            x = [v[()] for k, v in g.items() if k not in ARRAYS]
        if D[0].ndim == 2:
            return [None] + D
        return [x[0] if x else np.full(len(D[0]), float(h['xlimits'].split()[0]))] + D
    lines = lines_of(m['file'])
    A = np.loadtxt(lines, ndmin=2)
    dims = [int(v) for v in re.findall(r'\d+', h['type'][8:])]
    names = re.findall(r'^# (Data|Errors|Events) \[', ''.join(lines), re.M)
    shape = (dims[0], 4) if len(dims) == 1 else (dims[1]*len(names), dims[0])
    if A.shape != shape or len(set(names)) < len(names):
        refuse(m['where'] + ': data does not match the header, or was saved more than once')
    if len(dims) == 1:
        return list(A.T)
    D = dict(zip(names, np.split(A, len(names))))
    return [None] + [D.get(k) for k in ('Data', 'Errors', 'Events')]


# ---- checks

def compare(a, b, where, strict):
    ''' Headers a and b must be equal. Not strict: b (ASCII) may lack what a (NeXus) has '''
    for k in dict.fromkeys(list(a) + list(b)):
        if a.get(k) == b.get(k) or k in IGNORE or k in ('type', 'xylimits') and a['type'].startswith('list'):
            continue
        if k == 'Instrument source' and not (k in a and k in b):
            continue    # only there if run by mcrun
        if k in b or strict:
            refuse('%s: "%s" differs: "%.60s" and "%.60s"' % (where, k, a.get(k), b.get(k)))
        warn('%s: "%s" is only in the NeXus data' % (where, k))


def compare_runs(a, b, strict):
    compare(a['head'], b['head'], b['where'], strict)
    for key in dict.fromkeys(list(a['mons']) + list(b['mons'])):
        if key not in a['mons'] or strict and key not in b['mons']:
            refuse('%s, %s: monitor %s is not in both' % (a['where'], b['where'], key[0]))
        if key in b['mons']:
            m = b['mons'][key]
            compare(a['mons'][key]['head'], m['head'], m['where'], strict)
        else:
            warn('monitor %s is only in the NeXus data, and merged from that only' % key[0])


def check(runs):
    first = {}
    for r in runs:
        compare_runs(first.setdefault(r['fmt'], r), r, True)
    if len(first) == 2:
        compare_runs(first['NeXus'], first['ASCII'], False)
    seeds = sorted(s for r in runs for s in r['seeds'])
    for a, b in zip(seeds, seeds[1:]):
        if b[0] < a[1]:
            refuse('%s, %s: same Seed (with MPI: overlapping Seed to Seed+Nodes-1)' % (a[2], b[2]))


# ---- merging

def pool(n, N, I, E, mean):
    ''' Pools over the first axis. n: rays per run '''
    n = np.array(n, float).reshape((-1,) + (1,)*(N.ndim-1))
    T = N.sum(0)
    D = np.where(T > 0, T, 1)
    if mean:
        m = (N*I).sum(0)/D
        Q = (N*(E**2*np.maximum(N-1, 0) + I**2)).sum(0)
        e = np.sqrt(np.maximum(Q/D - m**2, 0)/np.maximum(T-1, 1))
    else:
        f = n/n.sum()
        M = np.where(N > 0, N, 1)
        m = (f*I).sum(0)
        p2 = (f**2*(E**2*np.maximum(N-1, 0)/M + (I/M)**2)).sum(0)
        e = np.sqrt(np.abs(p2 - (m/D)**2)*D/np.maximum(T-1, 1))
    return T, m, np.where(T <= 1, m, e)


def statistics(h, I):
    ''' As mcdetector_statistics of the runtime '''
    if I.ndim == 0:
        return {}
    ok = np.isfinite(I)
    z = np.where(ok, I, 0)
    lim = [float(v) for v in h.get('xlimits', h.get('xylimits')).split()]
    s = ''
    for i, c in enumerate('XY'[:I.ndim]):
        size = I.shape[::-1][i]
        x = lim[2*i] + (np.arange(size) + 0.5)/size*(lim[2*i+1] - lim[2*i])
        w = z if I.ndim == 1 else z.sum(i)
        c0 = (x*w).sum()/z.sum() if z.sum() else 0
        var = (x*x*w).sum()/z.sum() - c0**2 if z.sum() else 0
        s += '%s0=%g; d%s=%g; ' % (c, c0, c, np.sqrt(max(var, 0)))
    low, high = (I[ok].min(), I[ok].max()) if ok.any() else (np.nan, np.nan)
    return {'statistics': s.strip(), 'signal': 'Min=%g; Max=%g; Mean=%g;' % (low, high, z.sum()/z.size)}


def merge(mons):
    ''' New header values, and axis, I, E, N of the merged monitor '''
    n, h, mean = [m['n'] for m in mons], mons[0]['head'], mons[0]['head']['mean'] == 'yes'
    data = [load(m) for m in mons]
    for m, (x, I, E, N) in zip(mons, data):
        if x is not None and not np.allclose(x, data[0][0], 1e-9, 1e-9*abs(x).max()):
            refuse(m['where'] + ': axis values differ')
    noerr = [E is None or not mean and I.any() and not E.any() for x, I, E, N in data]
    if len(set(noerr)) > 1 or len(set(d[3] is None for d in data)) > 1:
        refuse('monitor %s: errors or N are only in some inputs' % h['component'])
    I, E, N = [np.stack([d[k] if d[k] is not None else np.full_like(d[1], k == 3) for d in data])
               for k in (1, 2, 3)]
    T, I, E = pool(n, N, I, E, mean)
    tot = np.array([m['head']['values'].split() for m in mons], float).T
    tT, tI, tE = pool(n, tot[2], tot[0], tot[1], mean)
    if noerr[0]:
        warn('monitor %s has no errors, they are set to 0' % h['component'])
        E, tE = 0*E, 0
        T = T if data[0][3] is not None else 0*T
    new = dict(Ncount='%i' % sum(n), values='%g %g %g' % (tI, tE, tT), **statistics(h, I))
    return new, data[0][0], [I, E, T]


def row(a):
    return ''.join('%.10g ' % x for x in a) + '\n'


def sub(lines, new):
    ''' Replaces the header lines with a key in new '''
    res = []
    for line in lines:
        m = re.match(r'(# |  |)(\w+):', line)
        if m and m[2] in new:
            v = new[m[2]]
            res += ['%s%s: %s\n' % (m[1], m[2], v)] if isinstance(v, str) else [m[1] + s + '\n' for s in v]
        else:
            res.append(line)
    return res


def load_list(m):
    ''' Events of an event list, one per row '''
    if 'group' not in m:
        with fopen(m['file']) as f:
            return np.loadtxt(f, ndmin=2)
    with h5py.File(m['file'], 'r') as f:
        d = f[m['group']]['events']
        for v in d.virtual_sources() if d.is_virtual else ():   # MPI. Reads as 0 if missing
            if not os.path.isfile(os.path.join(os.path.dirname(m['file']), v.file_name)):
                refuse('%s: %s is missing' % (m['where'], v.file_name))
        return d[()]


def merge_list(mons):
    ''' New header values, and events of the merged event lists '''
    h, n, data = mons[0]['head'], sum(m['n'] for m in mons), []
    names = h['variables'].split()
    if 'p' not in names or 'I' in names:
        refuse(mons[0]['where'] + ': event list without p, or with a second weight I')
    for m in mons:
        N, A = m['head']['values'].split()[2], load_list(m)
        if A.shape[1] != len(names):
            refuse(m['where'] + ': data does not match the header')
        if float(N) == 0:
            refuse(m['where'] + ': event list without N in "values", the data is from an older version')
        if float('%g' % len(A)) < float(N):     # more if written in blocks, the header is of the first
            refuse('%s: truncated event list, has %i of %s events' % (m['where'], len(A), N))
        A[:, names.index('p')] *= m['n']/n
        data.append(A)
    A = np.concatenate(data)
    return dict(Ncount='%i' % n, values='0 0 %g' % len(A), type='list(%i, %i)' % A.shape[::-1],
                xylimits='1 %i 1 %i' % A.shape, signal='Min=%g; Max=%g; Mean=0;' % (A[0, 0], A[0, 0])), None, A


def write_data(path, head, h, x, D):
    ''' Writes an ASCII data file. D: I, E, N, or the events of a list '''
    title = '# %%s [%s/%s] %%s:\n' % (h['component'], h['filename'])
    with fopen(path, 'w') as f:
        f.writelines(head)
        if h['type'].startswith('list'):
            f.write(title % ('Data', 'I'))
            f.writelines(map(row, D))
        elif h['type'] == 'array_0d':
            f.write(h['values'] + '\n')
        elif x is not None:   # 1D
            f.writelines(map(row, np.column_stack([x] + D)))
        else:
            for name, A in zip((('Data', 'I'), ('Errors', 'I_err'), ('Events', 'N')), D):
                f.write(title % name)
                f.writelines(map(row, A))


def write_ascii(out, t, runs, n, merged):
    ''' Writes a merged run to directory out, as a copy of run t '''
    d = os.path.join(out, t['sub'])
    os.makedirs(d, exist_ok=True)
    lines, table, extra = list(t['lines']), {}, []
    runsub = {'Ncount': '%i' % n, 'Seed': '-1', 'Nodes': [], 'Merged_from': [], 'Directory':
              ['Directory: ' + t['head']['Directory']] + ['Merged_from: ' + s for s in merged]}
    at = {b[0]: b[1:] for b in blocks(lines)}
    patch = [at['simulation'] + [runsub]]
    i, j = at['simulation']
    runhead = ['# ' + s[2:] for s in sub(lines[i+1:j], runsub)]
    for key in dict.fromkeys(k for r in (t,) + runs for k in r['mons']):
        mons = [r['mons'][key] for r in runs if key in r['mons']]
        m = t['mons'].get(key, mons[0])
        new, x, D = merge_list(mons) if m['list'] else merge(mons)
        h = dict(m['head'], **new)
        if 'block' in m:
            i, j = m['block']
            head = sub(lines[i+1:j], new)
            patch.append([i, j, new])
            add_values(table, m, new)
        else:   # from NeXus
            head = ['  %s: %s\n' % (k, h[k]) for k in KEYS if k in h]
            extra += ['begin data\n'] + head + ['end data\n', '\n']
        r = sub(runhead, {'Format': 'McCode list with text headers'}) if m['list'] else runhead
        write_data(os.path.join(d, h['filename']), r + ['# ' + s[2:] for s in head], h, x, D)
    i = at['timing'][0]
    lines[i:i] = extra
    for i, j, new in sorted(patch, key=lambda p: -p[0]):
        lines[i:j] = sub(lines[i:j], new)
    with fopen(os.path.join(d, 'mccode.sim'), 'w') as f:
        f.writelines(lines)
    return table


def write_events(g, A):
    ''' Replaces the events of a NeXus event list '''
    d = g['events']
    if d.maxshape[0] is None:
        d.resize(len(A), 0)
        d[...] = A
    else:   # from MPI, linked from other files
        attrs = dict(d.attrs)
        del g['events']
        d = g.create_dataset('events', data=A, maxshape=(None, A.shape[1]), compression='gzip')
        d.attrs.update(attrs)


def write_nexus(out, t, runs, n, merged):
    ''' Writes a merged run to the copy of the NeXus file in directory out '''
    table = {}
    with h5py.File(os.path.join(out, 'mccode.h5'), 'r+') as f:
        sim = f[t['entry']]['simulation']
        sim.attrs['Ncount_requested'] = '%i' % n
        sim.attrs['Merged_from'] = '\n'.join(merged)
        sim.attrs['Seed'] = '-1'
        if 'Nodes' in sim:
            del sim['Nodes']
        for key, m in t['mons'].items():
            mons = [r['mons'][key] for r in runs if key in r['mons']]
            new, x, D = merge_list(mons) if m['list'] else merge(mons)
            g = f[m['group']]
            if m['list']:
                new['type'] = 'list(%i, %i)' % D.shape
                write_events(g, D)
            for k, v in new.items():
                g.attrs[k] = v
            for k, A in zip(ARRAYS, D):
                if k in g:
                    g[k][...] = A.T
            add_values(table, m, new)
    return table


# ---- scans: the I and ERR columns of mccode.dat

def add_values(table, m, new):
    key = (m['head']['component'],) + tuple('%g' % float(v) for v in m['head']['values'].split()[:2])
    table.setdefault(key, set()).add(tuple(new.get('values', '0 0 0').split()[:2]))


def write_scan(src, dst, tables, n):
    lines = sub(lines_of(src), {'Ncount': '%i' % n})
    names = [s for s in lines if s.startswith('# variables:')][0].split()[2:]
    rows = [i for i, s in enumerate(lines) if s[0] != '#']
    if len(rows) != len(tables):
        refuse('%s: %i points, expected %i' % (src, len(rows), len(tables)))
    for i, table in zip(rows, tables):
        v = lines[i].split()
        for j, name in enumerate(names):
            if name.endswith('_I') and names[j+1:j+2] == [name[:-2] + '_ERR']:
                # several monitors of a component are told apart by their values
                new = table.get((name[:-2], '%g' % float(v[j]), '%g' % float(v[j+1])), ())
                if len(new) != 1:
                    refuse('%s: can not tell which monitor %s is' % (src, name))
                v[j:j+2], = new
        lines[i] = ' '.join(v) + '\n'
    with fopen(dst, 'w') as f:
        f.writelines(lines)


def merge_mcpl(out, runs):
    ''' Merges the MCPL files of a run with mcpltool '''
    dirs = [os.path.join(r['path'], r.get('sub', '')) for r in runs]
    for name in sorted({s for d in dirs for s in os.listdir(d) if re.fullmatch(MCPL, s)}):
        dst = os.path.join(out, name)
        if os.path.exists(dst):     # NeXus scans have one file for all points
            continue
        # mcpltool compresses in the end
        new = [s for s in (dst, re.sub(r'\.gz$', '', dst)) if not os.path.exists(s)]
        try:
            p = subprocess.run(['mcpltool', '--merge', dst] + [os.path.join(d, name) for d in dirs],
                               stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, errors='replace')
            error = p.returncode and 'mcpltool: ' + p.stdout.strip().split('\n')[0]
        except FileNotFoundError:
            error = 'mcpltool not found'
        if error:
            warn('%s is not merged: %s' % (name, error))
            for s in filter(os.path.exists, new):
                os.remove(s)
        else:
            warn('intensities in merged MCPL files are only correct with weight_mode=1 in MCPL_output')


def copy(out, datasets, t):
    ''' Copies what is not patched later, and names the files that are left out '''
    other = set()
    for runs in datasets:
        known = {'mccode.sim', 'mccode.dat', 'mccode.h5', runs[0]['head']['Source']}
        known.update(m['head']['filename'] for r in runs for m in r['mons'].values())
        for d in {os.path.join(r['path'], r.get('sub', '')) for r in runs} | {runs[0]['path']}:
            other.update(s for s in os.listdir(d) if s not in known and os.path.isfile(os.path.join(d, s))
                         and not re.fullmatch(r'mccode_node\d+\.h5|' + MCPL, s))
    if other:
        print('Not merged: ' + ' '.join(sorted(other)))
    os.mkdir(out)
    for name in [os.path.basename(t['head']['Source'])] + ['mccode.h5']*(t['fmt'] == 'NeXus'):
        if os.path.isfile(os.path.join(t['path'], name)):
            shutil.copyfile(os.path.join(t['path'], name), os.path.join(out, name))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('-d', metavar='OUT', required=True, help='output directory, must not exist')
    parser.add_argument('--format', choices=list(FORMAT), help='output format (default: NeXus if any input is)')
    parser.add_argument('input', nargs='+', help='dataset directories')
    args = parser.parse_args()
    out, part = args.d, args.d.rstrip('/' + os.sep) + '.part'
    if os.path.exists(out) or os.path.exists(part):
        refuse('%s or %s exists' % (out, part))
    if len(args.input) < 2:
        refuse('at least two datasets are needed')

    datasets = [read(p) for p in args.input]
    if len({len(d) for d in datasets}) > 1:
        refuse('different number of scan points')
    fmts = [d[0]['fmt'] for d in datasets]
    fmt = FORMAT[args.format] if args.format else 'NeXus' if 'NeXus' in fmts else 'ASCII'
    if fmt not in fmts:
        refuse('--format %s needs an input in that format' % args.format)
    points = list(zip(*datasets))
    for runs in points:
        check(runs)
    template = datasets[fmts.index(fmt)]
    if fmt == 'ASCII' and 'NeXus' in fmts:
        warn('geometry, component parameters and BINS of the NeXus data are left out')

    try:
        copy(part, datasets, template[0])
        tables = []
        for t, runs in zip(template, points):
            n, merged = sum(r['n'] for r in runs), [s for r in runs for s in r['from']]
            tables.append((write_ascii if fmt == 'ASCII' else write_nexus)(part, t, runs, n, merged))
            merge_mcpl(os.path.join(part, t.get('sub', '')), runs)
        path = template[0]['path']
        if os.path.isfile(os.path.join(path, 'mccode.dat')):
            write_scan(os.path.join(path, 'mccode.dat'), os.path.join(part, 'mccode.dat'), tables, n)
            if fmt == 'ASCII':
                with fopen(os.path.join(part, 'mccode.sim'), 'w') as f:
                    f.writelines(sub(lines_of(os.path.join(path, 'mccode.sim')), {'Ncount': '%i' % n}))
        os.rename(part, out)
    except BaseException:
        shutil.rmtree(part, ignore_errors=True)
        raise


if __name__ == '__main__':
    main()
