from mcplopp import _edges, to_dataarray
from mccodelib.mcplotloader import Data2D

dim, e = _edges('Wavelength [AA]', 0, 10, 20)
assert dim == 'Wavelength' and str(e.unit) == 'Å' and e.sizes == {dim: 21}
dim, e = _edges('TOF [\\gms]', 0, 1, 4)  # unknown unit: kept in label, dimensionless
assert dim == 'TOF [\\gms]' and str(e.unit) == 'dimensionless'

d = Data2D()  # array_2d(3, 2): 2 rows (y) x 3 columns (x)
d.xlabel, d.ylabel, d.xlimits, d.zvals = 'x [cm]', 'y [cm]', (0, 3, 0, 2), [[1, 2, 3], [4, 5, 6]]
da = to_dataarray(d)
assert da.dims == ('y', 'x') and da.coords['x'].sizes == {'x': 4} and da['x', 2]['y', 1].value == 6
print('ok')

# NeXus: (nx, ny) storage, transposed to the ASCII ('y', 'x') layout; errors -> variances; one entry per scan step
import os, tempfile
import h5py
import numpy as np
from mcplopp import load

def entry(f, name, lam, scale):
    out = f.create_group(f'{name}/instrument/components/0001_psd/output')
    f.create_group(f'{name}/simulation/Param').attrs['lambda'] = str(lam)
    g = out.create_group('psd_dat')
    g.attrs.update({'type': 'array_2d(3, 2)', 'title': 'PSD', 'xlabel': 'X position [cm]', 'ylabel': 'Y position [cm]',
                    'xylimits': '0 3 0 2'})
    g['data'] = scale * np.arange(6.).reshape(3, 2)  # data[ix, iy]
    g['errors'] = np.ones((3, 2))
    h = out.create_group('l_dat')
    h.attrs.update({'type': 'array_1d(4)', 'title': 'L', 'xlabel': 'Wavelength [AA]', 'xlimits': '1 3'})
    h['data'], h['errors'] = np.arange(4.), np.full(4, 0.5)

with tempfile.TemporaryDirectory() as d:
    with h5py.File(os.path.join(d, 'mccode.h5'), 'w') as f:
        entry(f, 'entry1', 2.0, 1)
    das = load(d)  # directory with mccode.h5 and no mccode.sim
    psd = das['PSD [1]']
    assert psd.dims == ('Y position', 'X position') and psd.values[1, 2] == 5  # (iy=1, ix=2) <- data[2, 1]
    assert psd.variances is not None and das['L [0]'].coords['Wavelength'].sizes == {'Wavelength': 5}
    with h5py.File(os.path.join(d, 'scan.h5'), 'w') as f:
        for i, lam in enumerate((1.5, 2.0, 2.5)):
            entry(f, f'entry{i + 1}', lam, i + 1)
    scan = load(os.path.join(d, 'scan.h5'))['PSD [1]']
    assert scan.dims[0] == 'lambda' and scan.coords['lambda'].values.tolist() == [1.5, 2.0, 2.5]
    assert scan['lambda', 2].values[1, 2] == 15
print('ok nexus')
