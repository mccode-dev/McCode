# The `RITA_II` Instrument

*McStas: RITA type triple-axis spectrometer (TAS)*

## Identification

- **Site:** PSI
- **Author:** <a href="mailto:udby@fys.ku.dk">Linda Udby</a>  and <a href="mailto:pkwi@fysik.dtu.dk">Peter Willendrup</a>
- **Origin:** <a href="http://www.risoe.dk">Ris&oslash; (Denmark)</a>
- **Date:** 2012

## Description

```text
This instrument is model of RITA-II at the RNR13 guide at SINQ, PSI as of 2009. The energy and q-resolution as been verified against measured data in 2- and 3-axis modes. See Udby et al., Nuclear Instruments and Methods 634 (2011) s138-.<br>
The model is based on the following components in downstream order
<ul>
<li> The SINQ source in 2009 - tested up to Ei=15 meV versus measurements. The spectrum of genereated rays is controlled by the BPL and BPH parameters.</li>
<li> The RNR13 guide with average m-value based on color-codes on guide pieces and gaps as measured/from technical drawings.</li>
<li> The Druckal monochromator: 5 slabs of PG(002) with vertically focusing option. The horizontal rotation angle (A1) and  scattering angle (A2) can either be set by user input or calculated automatically from EI/KI. </li>
<li> Optional filter after the monochromator, before the sample. </li>
<li> Variable linear collimator after the monochromator, before the sample. </li>
<li> Optional perspex attenuation. </li>
<li> Inbound Diaphagm slit. </li>
<li> Sample: Incoherent scatterer, powder or single crystal from reflection list. </li>
<li> Outbound Diaphragm slit. </li>
<li> Optional filter after the sample, with radial collimator.</li>
<li> 9 - blade analyser: Base-rotation (AA5) and individual blade angles (C1-C9) can either be set by user or calculated automatically from EF/KF. </li>
<li> Coarse collimator: Removes cross-talk by allowing only passage of scattered neutrons from specified blade in each channel.</li>
<li> Detector: PSD detector with 100% efficiency and specified point-spread function from measurements.</li>
<li> Windows: Monitors the neutrons in each electronically specified window [XWinMin,XWinMax;YWinMin,YWinMax].</li>
</ul>
EXAMPLES
<ul>
<li>Generating a virtual source </li>
mcrun RITA-II.instr -n3e6  BPL=0.97 BPH=1.03 EI=5 EF=5 COLL_MS=40 SAMPLE=1 OUTFILTER=0 REP=10 VIRTUALOUT=1 -d  RITA-II_Vsource40 <br>
<li>Vanadium sample, no filters, seeing 1st-3rd order neutrons </li>
mcrun RITA-II.instr -n3e6 BPL=0.30 BPH=1.03 EI=5 EF=5 SAMPLE=1 INFILTER=0 OUTFILTER=0  REP=10 -d RITA-II_Vanadium <br>
<li>Powder sample (default sapphire), simulation 1) through entire instrument or  2)from virtual source 3) in 2-axis mode, with particular slit settings scaling the source yield to 2008 values </li>
1) mcrun RITA-II.instr -n3e6 EF=5 EN=0 SAMPLE=2 COLL_MS=40 SAMPLEFILE="Al2O3_sapphire.lau" BARNS=0 SOURCEFILE="RITA-II_Vsource40/Vin_default.dat" REP=10 -d RITA-II_40_powder<br>
2) mcrun RITA-II.instr EI=5 EF=5 SAMPLE=2 SAMPLEFILE="Al2O3_sapphire.lau" BARNS=0 SOURCEFILE="RITA-II_Vsource40/Vin_default.dat" VIRTUALIN=1 REP=3 -d RITA-II_VINsource40_powder <br>
3) mcrun RITA-II.instr -n 1e7 ITAR=0.71 COLL_MS=19.6 BPL=0.97 BPH=1.03 EI=5 EF=5 QH=0 QK=0 QL=0 QM=1 A3=0.001 A4=-71 AA5=-90 A6=1e-3 MST=25 MSB=25 MSL=10 MSR=10 SAMPLE=2 SAMPLEFILE="Al2O3_sapphire.lau" SST=25 SSB=25 SSL=10 SSR=10 OUTFILTER=0 OUTFILTERFILE=Be.trm COARSE=1 REP=5 LC=6 RC=4 REP=1 BARNS=0 ANAFORCE=1 -d RITA-II_Al2O3_2axis<br>
<li>Single crystal sample. Define first lattice vector (a) along -x (to the right) , second lattice vector (b) along z (forward), third lattice vector (c) along y (up). </li>
mcrun RITA-II.instr -n1e6 BPL=0.97 BPH=1.03 EI=5 EF=5 SAMPLE=3 REP=10 QH=2 QM=0 AS=4.95 BS=4.95 CS=4.95 AAX=-4.95 BBZ=4.95 CCY=4.95 AH=0 AK=1 AL=0  BH=1 BK=0 BL=0 SAMPLEFILE="Pb.laz" -d RITA-II_SXPb200 <br>
<li> Phonon sample, inelastic scattering, no Bragg peaks in sample </li>
mcrun RITA-II.instr -n1e6 L0=3.418 BPL=0.97 BPH=1.03 EI=7 EF=5 SAMPLE=4 REP=10 QH=2 QK=0.16 QM=0 AS=4.95 BS=4.95 CS=4.95 AAX=-4.95 BBZ=4.95 CCY=4.95 AH=0 AK=1 AL=0  BH=1 BK=0 BL=0 SAMPLEFILE="Pb.laz" -d RITA-II_SXPb200_2meV <br>
</ul>

Example: BPL=0.97 BPH=1.03 EI=5 EN=0 COLL_MS=40 SAMPLE=1 OUTFILTER=0 REP=10 Detector: psd_detector_I=216.427
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| ITAR | mA | Relative neutron yield from the spallation target. Value relative to 2009 | 1.0 |
| L0 | Angs | Centre of generated wavelength distribution from source | 4.045 |
| BPL |  | Band Pass Low factor, multiplied on source wavelength L0 to allow neutrons with wavelengths with lambda {BPL*L0,BPH*L0} to be traced from the source. | 0.97 |
| BPH |  | Band Pass High factor, multiplied on source wavelength L0 to allow neutrons with wavelengths with lambda {BPL*L0,BPH*L0} to be traced from the source. | 1.03 |
| MONO_N | 1 | Order of reflection used on mono | 1 |
| MONOFORCE | 1 | If set, monochromator is flat | 0 |
| MONO_MOS_H | min | Monochromator, horizontal mosaicity | 37 |
| MONO_MOS_V | min | Monochromator, vertical mosaicity | 37 |
| EI | meV | Incoming neutron energy. Also used to set range of energy monitors | 0 |
| EF | meV | Outgoing neutron energy. Also used to set range of energy monitors | 0 |
| EN | meV | Energy transferred in crystal | 0 |
| SM | 1 |  | 1 |
| SS | 1 | Scattering configuration signs. 'W' is SM=1,SS=-1,SA=1 | -1 |
| SA | 1 |  | 1 |
| QH | rlu | Measurement QH position in crystal | 0 |
| QK | rlu | Measurement QK position in crystal | 0 |
| QL | rlu | Measurement QL position in crystal | 0 |
| QM | Angs-1 | Wavevector transferred in sample, use QM=0 if (QH,QK,QL) is specified | 1.8051 |
| AS | Angs | Sample lattice parameter A | 4.95 |
| BS | Angs | Sample lattice parameter B | 4.95 |
| CS | Angs | Sample lattice parameter C | 4.95 |
| AA | deg | Angle between lattice vectors B,C | 90 |
| BB | deg | Angle between lattice vectors C,A | 90 |
| CC | deg | Angle between lattice vectors A,B | 90 |
| AH | rlu | First reciprocal lattice vector in scattering plane, X | 0 |
| AK | rlu | First reciprocal lattice vector in scattering plane, Y | 1 |
| AL | rlu | First reciprocal lattice vector in scattering plane, Z | 0 |
| BH | rlu | Second reciprocal lattice vector in scattering plane, X | 1 |
| BK | rlu | Second reciprocal lattice vector in scattering plane, Y | 0 |
| BL | rlu | Second reciprocal lattice vector in scattering plane, Z | 0 |
| INFILTER | 1 | Flag to indicate if mono-block filter is in or out | 0 |
| INFILTERFILE | string | An input file of wavevector vs transmission to use with incoming filter | "Be.trm" |
| COLL_MS | deg | Primary collimator max divergence | 80 |
| MST | m | Monochromator TOP slit | 40 |
| MSB | m | Monochromator BOTTOM slit | 40 |
| MSL | m | Monochromator LEFT slit | 40 |
| MSR | m | Monochromator RIGHT slit | 40 |
| PTHICK | m | Thickness of perspex attenuator | 1e-3 |
| PERSPEX | 1 | Flag to indicate if perspex attenuator is in or out | 0 |
| SAMPLE | 1 | 1 is incoherent scatterer, 2 is powder, 3 is single crystal. | 1 |
| SAMPLEFILE | string | Name of samplefile (with reflectionlist etc) | "default" |
| MOS |  | Isotropic 'mosaicity' of single crystal | 100 |
| DD_D |  | spead of lattice parameter | 1e-3 |
| SAMPLESIZE | m | Length, height and width of single crystal sample, or radius and height of phonon sample | 0.01 |
| BARNS | 1 | If set the flag indicates that reflection list structure factors are in units of barns, otherwise fm^2 | 1 |
| AAX |  |  | -4.95 |
| AAY |  | Orientation vector of unit cell, single_crystal | 0 |
| AAZ |  |  | 0 |
| BBX |  |  | 0 |
| BBY |  | Orientation vector of unit cell, single_crystal | 0 |
| BBZ |  |  | 4.95 |
| CCX |  |  | 0 |
| CCY |  | Orientation vector of unit cell, single_crystal | 4.95 |
| CCZ |  |  | 0 |
| A1 | deg | Monohromator rotation angle | 0 |
| A2 | deg | Monohromator take-off angle | 0 |
| A3 | deg | Sample rotation angle | 0 |
| A4 | deg | Sample take-off angle | 0 |
| A6 | deg | Analyzer take-off angle | 0 |
| TILT | deg | Tilt angle out of plane of the sample scattering vector out of plane (rotation around z of the A3 arm) | 0 |
| SST | m | Sample TOP slit | 100 |
| SSB | m | Sample BOTTOM slit | 100 |
| SSL | m | Sample LEFT slit | 100 |
| SSR | m | Sample RIGHT slit | 100 |
| OUTFILTER | 1 | Flag to indicate if the outbound filter is in or out | 1 |
| OUTFILTERFILE | string | An input file of wavevector vs transmission to use with outgoing filter | "Be.trm" |
| ANAFORCE | 1 | If set the analyzer is forced flat | 0 |
| AA5 | deg | Analyzer rack rotation angle | 0 |
| C1 | deg | Analyzer blade 1 position | 0 |
| C2 | deg | Analyzer blade 2 position | 0 |
| C3 | deg | Analyzer blade 3 position | 0 |
| C4 | deg | Analyzer blade 4 position | 0 |
| C5 | deg | Analyzer blade 5 position | 0 |
| C6 | deg | Analyzer blade 6 position | 0 |
| C7 | deg | Analyzer blade 7 position | 0 |
| C8 | deg | Analyzer blade 8 position | 0 |
| C9 | deg | Analyzer blade 9 position | 0 |
| COARSE | 1 | Flag to indicate if Detector collimator is in or out | 1 |
| LC | deg | Detector-collimator angle of the leftmost blade | 4 |
| RC | deg | Detector-collimator angle of the rightmost blade | 3 |
| REP | 1 | Repetition factor of virtual_input or Monochromator/Sample/Analyzer SPLITs | 1 |
| VIRTUALOUT | 1 | If set, flag indicates that a Virtual source is created after the monochromator collimator | 0 |
| VIRTUALIN | 1 | If set, flag to indicates that Virtual source is used after the monochromator collimator | 0 |
| SOURCEFILE | string | Name of file to be used/genereated as virtual input/output | "Vin_default.dat" |
| verbose | 1 | print TAS configuration. 0 to be quiet | 1 |

## Links

- [Source code](RITA-II.instr) for `RITA-II.instr`.
- Rescal for Matlab at http://www.ill.fr/tas/matlab
- Restrax at http://omega.ujf.cas.cz/restrax/

---
