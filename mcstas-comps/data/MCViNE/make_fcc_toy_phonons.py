import numpy as np, importlib.util, struct, itertools, os
"""Toy fcc nearest-neighbour Born-von Karman phonons (a=4.05 AA, 1 atom) written in
MCViNE IDF format (Qgridinfo, Omega2, Polarizations, DOS). Needs the MCViNE repo
for the IDF writers: set MCVINE_IDF to mcvine/packages/mccomponents/python/mccomponents/sample/idf"""
MCVINE_IDF=os.environ.get('MCVINE_IDF','mcvine/packages/mccomponents/python/mccomponents/sample/idf')
os.makedirs('fcc_toy_phonons', exist_ok=True)
def load(name):
    sp=importlib.util.spec_from_file_location(name,MCVINE_IDF+'/%s.py'%name)
    m=importlib.util.module_from_spec(sp); sp.loader.exec_module(m); return m
O2=load('Omega2'); PO=load('Polarizations')
hbar=1.05457148e-34; hertz2mev=hbar/1.60217653e-22
a=4.05
nn=np.array([p for p in itertools.product([-1,0,1],repeat=3) if sum(abs(np.array(p)))==2])*a/2   # 12 neighbours
b=2*np.pi/a*np.array([[-1,1,1],[1,-1,1],[1,1,-1]])
def dyn(q):
    D=np.zeros((3,3))
    for R in nn:
        Rh=R/np.linalg.norm(R); D+= (1-np.cos(q@R))*np.outer(Rh,Rh)
    return D
Emax_target=38.
# scale: max eigenvalue over zone
lam_max=max(np.linalg.eigvalsh(dyn(c@b)).max() for c in np.random.rand(3000,3))
C=(Emax_target/hertz2mev)**2/lam_max
def phonons(q):
    w2,ev=np.linalg.eigh(dyn(q)*C); ev=ev.T
    for m in range(3):
        i=np.argmax(abs(ev[m]));
        if ev[m,i]<0: ev[m]*=-1
    return w2, ev   # rows = modes
if __name__=='__main__':
    n=13
    f=np.linspace(0,1,n)
    om2=[];pol=[]
    for i,j,k in itertools.product(range(n),repeat=3):
        q=np.array([f[i],f[j],f[k]])@b
        w2,ev=phonons(q); om2.append(w2); pol.append(ev.reshape(3,1,3).astype(complex))
    om2=np.array(om2); pol=np.array(pol)
    O2.write(om2,'fcc_toy_phonons/Omega2'); PO.write(pol,'fcc_toy_phonons/Polarizations')
    with open('fcc_toy_phonons/Qgridinfo','w') as fh:
        for i in range(3): fh.write("b%d = [%r, %r, %r]\n"%(i+1,*b[i]))
        for i in range(3): fh.write("n%d = %d\n"%(i+1,n))
    # DOS by sampling
    E=np.sqrt(np.clip(np.array([phonons(c@b)[0] for c in np.random.rand(200000,3)]),0,None))*hertz2mev
    h,edges=np.histogram(E.ravel(),bins=200,range=(0,40)); e=0.5*(edges[1:]+edges[:-1])
    np.savetxt('fcc_toy_dos.dat', np.c_[e,h/h.sum()/0.2], header='E(meV) g')
    # IDF DOS (THz) with bins starting at 0
    dE_THz=0.2/hertz2mev/1e12/2/np.pi
    with open('fcc_toy_phonons/DOS','wb') as fh:
        fh.write(struct.pack('<64s',b'DOS')); fh.write(struct.pack('<i',1)); fh.write(struct.pack('<1024s',b''))
        fh.write(struct.pack('<i',len(h))); fh.write(struct.pack('<d',dE_THz)); fh.write(struct.pack('<%dd'%len(h),*(h/h.sum())))
    open('fcc_toy_atoms.dat','w').write("# x y z mass b_coh sigma_inc sigma_abs\n0 0 0 26.98 3.449 0.0082 0.231\n")
    print("done; max E", E.max())
