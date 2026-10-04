## Assesment by Claude: Default output data vs. instrument webpage:

At the reference setting (white beam, L/D = 333) the model reproduces ILL's headline flux to within 6 %. The collimation and field of view are also consistent with the website. The peak wavelength differs, but that depends on where you look along the beam, and the website doesn't say where its value applies. Spatial resolution and tomography speed can't be compared yet.

NeXT website	Model (your 10⁹-neutron run)	Match?
Flux at sample: 3 × 10⁸ n/cm²/s at L/D = 333	2.83 ± 0.01 × 10⁸ (averaged over 3 × 3 cm²)	✓ −6 %
Peak neutron energy: 11 meV / 2.8 Å	Guide exit: broad plateau at 3.0–3.6 Å, peak ≈ 3.3 Å (7.6 meV). Sample: plateau at 1.9–2.5 Å, peak ≈ 2.4 Å (14 meV)	~ ILL's value falls between the two
Max field of view 170 × 170 mm²	Beam FWHM 150 (h) × 182 (v) mm at L = 10 m; 85 % of it falls within 170 × 170	✓ covered, but not flat
Collimation L/D = 333	Divergence at the sample, FWHM 0.14° (h) / 0.17° (v); geometric D/L = 0.17°	✓
Max spatial resolution 3 µm	Not testable: the detector is ideal, with 0.85 mm pixels	—
Fastest tomography 1.5 s	Not simulated	—
Δλ/λ: 5 % DCM, 16 % selector	Not in this run. My earlier tests gave ~16–18 % for the selector (after I tuned its assumed twist angle to this value) and ~2 % for the DCM	✓ / ✗

Flux. The statistical error is 0.4 %, so the 6 % gap is systematic. Two effects could close it or move it:

Truncated band: the source only produces 1–10 Å. The 1–1.5 Å bin already carries 5.6 % of the flux, so neutrons below 1 Å would probably add a few percent.
How ILL measured it: if their number comes from gold-foil activation, it's a capture flux, which weights each neutron by λ/1.8 Å. On that basis the model would give about 5.3 × 10⁸, roughly 1.8× the website figure.

The comparison is tight either way, given the source term is a 2011 design model.

Peak wavelength. The spectrum at the sample is shifted to shorter wavelengths than the one at the guide exit, and that's real physics. The pinhole selects a fixed small solid angle, while the guide's divergence grows with λ. So the pinhole passes a smaller fraction of long-wavelength neutrons, roughly as 1/λ². ILL's 2.8 Å is in between the two model values; it's probably an integrated guide spectrum rather than one at the sample. Both model spectra also show dips at 2.2, 2.8 and 4.4 Å. These are real features: SuperADAM (set to 4.4 Å) removes 4.4 Å and its second order at 2.2 Å, and D16 (set to 5.6 Å) removes 2.8 Å, the second order of 5.6 Å.

Field of view. The intensity falls about 6× from the centre to the corners of 170 × 170 mm². Within 80 × 80 mm it's flat to about ±9 %. So the full field is usable only with flat-field correction. The beam is narrower horizontally because the guide is 60 mm wide and 120–150 mm tall.

Spatial resolution. With the default 30 mm sample-to-detector distance, the geometric blur alone is 30 mm / 333 ≈ 90 µm. Getting down to ILL's 3 µm needs the sample within about 1 mm of the scintillator at this L/D, or a higher L/D, plus a detector model with scintillator blur and realistic pixels. I'd rate this as not yet comparable rather than a mismatch.

Fe sample check. Transmission through the 10 mm rod jumps from 0.23 below the Fe(110) edge at 4.05 Å to 0.55 above it. That agrees with the ~0.59 I expect from absorption plus incoherent scattering over the ~9.5 mm path.

The two open gaps are the DCM resolution and the missing detector response. Adding scintillator blur and a realistic pixel size would make the 3 µm figure testable. Choosing a DCM geometry that reproduces ILL's 5 % needs input on how ILL defines that number.
