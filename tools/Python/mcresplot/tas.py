#!/usr/bin/env python3
#
# calculates TAS angles from rlu
# @author Tobias Weber <tweber@ill.fr>
# @date 1-aug-18
# @license GNU GPLv3
# @descr This tool comes from Takin 2: https://dx.doi.org/10.5281/zenodo.4117437
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

import sys

try:
	import numpy as np
	import numpy.linalg as la
except ImportError:
	print("Numpy could not be imported!")
	exit(-1)

use_scipy = False


# -----------------------------------------------------------------------------
# rotate a vector around an axis using Rodrigues' formula
# see https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
def rotate(_axis, vec, phi):
	axis = _axis / la.norm(_axis)

	s = np.sin(phi)
	c = np.cos(phi)

	return c*vec + (1.-c)*np.dot(vec, axis)*axis + s*np.cross(axis, vec)


# get metric from crystal B matrix
# basis vectors are in the columns of B, i.e. the second index
# see T. Arens et al., "Mathematik", 2015, ISBN: 978-3-642-44919-2, p. 815
def get_metric(B):
	#return np.einsum("ij,ik -> jk", B, B)
	return np.dot(np.transpose(B), B)


# cross product in fractional coordinates: c^l = eps_ijk g^li a^j b^k
# see T. Arens et al., "Mathematik", 2015, ISBN: 978-3-642-44919-2, p. 815
def cross(a, b, B):
	# levi-civita in fractional coordinates
	def levi(i,j,k, B):
		M = np.array([B[:,i], B[:,j], B[:,k]])
		return la.det(M)

	metric_inv = la.inv(get_metric(B))
	eps = [[[ levi(i,j,k, B) for k in range(0,3) ] for j in range(0,3) ] for i in range(0,3) ]
	return np.einsum("ijk,j,k,li -> l", eps, a, b, metric_inv)


# dot product in fractional coordinates
# see T. Arens et al., "Mathematik", 2015, ISBN: 978-3-642-44919-2, p. 808
def dot(a, b, metric):
	return np.dot(a, np.dot(metric, b))


# angle between peaks in fractional coordinates
# see T. Arens et al., "Mathematik", 2015, ISBN: 978-3-642-44919-2, p. 808
def angle(a, b, metric):
	len_a = np.sqrt(dot(a, a, metric))
	len_b = np.sqrt(dot(b, b, metric))

	c = dot(a, b, metric) / (len_a * len_b)

	# check for rounding errors
	if c > 1.:
		#print("arccos precision overflow: " + str(c) + ".")
		c = 1.
	if c < -1.:
		#print("arccos precision underflow: " + str(c) + ".")
		c = -1.

	return np.arccos(c)
# -----------------------------------------------------------------------------


# -----------------------------------------------------------------------------
if use_scipy:
	try:
		import scipy as sp
		import scipy.constants as co
	except ImportError:
		print("Scipy could not be imported!")
		exit(-1)

	hbar_in_meVs = co.Planck/co.elementary_charge*1000./2./np.pi
	E_to_k2 = 2.*co.neutron_mass/hbar_in_meVs**2. / co.elementary_charge*1000. * 1e-20
else:
	E_to_k2 = 0.482596406464	# calculated with scipy, using the formula above

k2_to_E = 1./E_to_k2
# -----------------------------------------------------------------------------


# -----------------------------------------------------------------------------
# mono (or ana) k  ->  A1 & A2 angles (or A5 & A6)
def get_a1a2(k, d):
	s = np.pi/(d*k)
	a1 = np.arcsin(s)
	return [a1, 2.*a1]


# a1 angle (or a5)  ->  mono (or ana) k
def get_monok(theta, d):
	s = np.sin(theta)
	k = np.pi/(d*s)
	return k
# -----------------------------------------------------------------------------


# -----------------------------------------------------------------------------
# scattering angle a4
def get_a4(ki, kf, Q):
	c = (ki**2. + kf**2. - Q**2.) / (2.*ki*kf)
	return np.arccos(c)


# get |Q| from ki, kf and a4
def get_Q(ki, kf, a4):
	c = np.cos(a4)
	return np.sqrt(ki**2. + kf**2. - c*(2.*ki*kf))
# -----------------------------------------------------------------------------


# -----------------------------------------------------------------------------
# angle enclosed by ki and Q
def get_psi(ki, kf, Q, sense=1.):
	c = (ki**2. + Q**2. - kf**2.) / (2.*ki*Q)
	return sense*np.arccos(c)


# crystallographic A matrix converting fractional to lab coordinates
# see https://de.wikipedia.org/wiki/Fraktionelle_Koordinaten
def get_A(lattice, angles):
	cs = np.cos(angles)
	s2 = np.sin(angles[2])

	a = lattice[0] * np.array([1, 0, 0])
	b = lattice[1] * np.array([cs[2], s2, 0])
	c = lattice[2] * np.array([cs[1], \
		(cs[0]-cs[1]*cs[2]) / s2, \
		(np.sqrt(1. - np.dot(cs,cs) + 2.*cs[0]*cs[1]*cs[2])) / s2])

	# testing equality with own derivation
	#print((np.sqrt(1. - np.dot(cs,cs) + 2.*cs[0]*cs[1]*cs[2])) / s2)
	#print(np.sqrt(1. - cs[1]*cs[1] - ((cs[0] - cs[2]*cs[1])/s2)**2.))

	# the real-space basis vectors form the columns of the A matrix
	return np.transpose(np.array([a, b, c]))


# crystallographic B matrix converting rlu to 1/A
# the reciprocal-space basis vectors form the columns of the B matrix
def get_B(lattice, angles):
	A = get_A(lattice, angles)
	B = 2.*np.pi * np.transpose(la.inv(A))
	return B


# UB orientation matrix
# see https://dx.doi.org/10.1107/S0021889805004875
def get_UB(B, orient1_rlu, orient2_rlu, orientup_rlu):
	orient1_invA = np.dot(B, orient1_rlu)
	orient2_invA = np.dot(B, orient2_rlu)
	orientup_invA = np.dot(B, orientup_rlu)

	orient1_invA = orient1_invA / la.norm(orient1_invA)
	orient2_invA = orient2_invA / la.norm(orient2_invA)
	orientup_invA = orientup_invA / la.norm(orientup_invA)

	U_invA = np.array([orient1_invA, orient2_invA, orientup_invA])
	UB = np.dot(U_invA, B)
	return UB


# a3 & a4 angles
# see https://dx.doi.org/10.1107/S0021889805004875
def get_a3a4(ki, kf, Q_rlu, orient_rlu, orient_up_rlu, B, sense_sample=1., a3_offs=np.pi):
	metric = get_metric(B)

	# angle xi between Q and orientation reflex
	xi = angle(Q_rlu, orient_rlu, metric)

	# sign of xi
	if dot(cross(orient_rlu, Q_rlu, B), orient_up_rlu, metric) < 0.:
		xi = -xi

	# length of Q
	Qlen = np.sqrt(dot(Q_rlu, Q_rlu, metric))

	# distance to plane
	up_len = np.sqrt(dot(orient_up_rlu, orient_up_rlu, metric))
	dist_Q_plane = dot(Q_rlu, orient_up_rlu, metric) / up_len

	# angle psi enclosed by ki and Q
	psi = get_psi(ki, kf, Qlen, sense_sample)

	a3 = - psi - xi + a3_offs
	a4 = get_a4(ki, kf, Qlen)

	#print("xi = " + str(xi/np.pi*180.) + ", psi = " + str(psi/np.pi*180.) + ", offs = " + str(a3_offs/np.pi*180.))
	return [a3, a4, dist_Q_plane]


# (hkl) position
# see https://dx.doi.org/10.1107/S0021889805004875
def get_hkl(ki, kf, a3, Qlen, orient_rlu, orient_up_rlu, B, sense_sample=1., a3_offs=np.pi):
	B_inv = la.inv(B)

	# angle enclosed by ki and Q
	psi = get_psi(ki, kf, Qlen, sense_sample)

	# angle between Q and orientation reflex
	xi = - a3 + a3_offs - psi

	Q_lab = rotate(np.dot(B, orient_up_rlu), np.dot(B, orient_rlu*Qlen), xi)
	Q_lab *= Qlen / la.norm(Q_lab)
	Q_rlu = np.dot(B_inv, Q_lab)

	return Q_rlu
# -----------------------------------------------------------------------------


# -----------------------------------------------------------------------------
# get ki from kf and energy transfer
def get_ki(kf, E):
	return np.sqrt(kf**2. + E_to_k2*E)


# get kf from ki and energy transfer
def get_kf(ki, E):
	return np.sqrt(ki**2. - E_to_k2*E)


# get energy transfer from ki and kf
def get_E(ki, kf):
	return (ki**2. - kf**2.) / E_to_k2
# -----------------------------------------------------------------------------



# -----------------------------------------------------------------------------
# get the difference in tas angles for two positions
def get_angle_deltas(ki1, kf1, Q_rlu1, di1, df1, \
	ki2, kf2, Q_rlu2, di2, df2, \
	orient_rlu, orient_up_rlu, B, sense_sample=1., a3_offs=np.pi):

	# position 1
	[a1_1, a2_1] = get_a1a2(ki1, di1)
	[a5_1, a6_1] = get_a1a2(kf1, df1)
	[a3_1, a4_1, dist_Q_plane_1] = get_a3a4(ki1, kf1, Q_rlu1, orient_rlu, orient_up_rlu, B, sense_sample, a3_offs)

	# position 2
	[a1_2, a2_2] = get_a1a2(ki2, di2)
	[a5_2, a6_2] = get_a1a2(kf2, df2)
	[a3_2, a4_2, dist_Q_plane_2] = get_a3a4(ki2, kf2, Q_rlu2, orient_rlu, orient_up_rlu, B, sense_sample, a3_offs)

	return [a1_2-a1_1, a2_2-a2_1, a3_2-a3_1, a4_2-a4_1, a5_2-a5_1, a6_2-a6_1, dist_Q_plane_1, dist_Q_plane_2]


# get the instrument driving time
def driving_time(deltas, rads_per_times):
	times = np.abs(deltas) / rads_per_times
	return np.max(times)

# -----------------------------------------------------------------------------






# -----------------------------------------------------------------------------
# GUI
# -----------------------------------------------------------------------------
class TasGUI:
	B = np.array([[1,0,0], [0,1,0], [0,0,1]])
	orient_rlu = np.array([1,0,0])
	orient2_rlu = np.array([0,1,0])
	orient_up_rlu = np.array([0,0,1])

	g_eps = 1e-4
	a3_offs = np.pi


	# -----------------------------------------------------------------------------
	# helpers
	def getfloat(self, str):
		try:
			return float(str)
		except ValueError:
			return 0.
	# -----------------------------------------------------------------------------


	# -----------------------------------------------------------------------------
	# crystal tab
	def xtalChanged(self):
		lattice = np.array([self.getfloat(self.editA.text()), self.getfloat(self.editB.text()), self.getfloat(self.editC.text())])
		angles = np.array([self.getfloat(self.editAlpha.text()), self.getfloat(self.editBeta.text()), self.getfloat(self.editGamma.text())])
		self.orient_rlu = np.array([self.getfloat(self.editAx.text()), self.getfloat(self.editAy.text()), self.getfloat(self.editAz.text())])
		self.orient2_rlu = np.array([self.getfloat(self.editBx.text()), self.getfloat(self.editBy.text()), self.getfloat(self.editBz.text())])

		try:
			self.B = get_B(lattice, angles/180.*np.pi)
			invB = la.inv(self.B)

			metric = get_metric(self.B)
			ang = angle(self.orient_rlu, self.orient2_rlu, metric)

			self.orient_up_rlu = cross(self.orient_rlu, self.orient2_rlu, self.B)
			self.orient_up_rlu_norm = self.orient_up_rlu / la.norm(self.orient_up_rlu)

			UB = get_UB(self.B, self.orient_rlu, self.orient2_rlu, self.orient_up_rlu)
			invUB = la.inv(UB)

			self.editBMat.setPlainText("Scattering plane normal: %s rlu.\n" % str(self.orient_up_rlu_norm) \
				+"Angle between orientation vectors 1 and 2: %.4g deg.\n" % (ang/np.pi*180.) \
				+"\nB =\n%10.4f %10.4f %10.4f\n%10.4f %10.4f %10.4f\n%10.4f %10.4f %10.4f\n" \
				% (self.B[0,0],self.B[0,1],self.B[0,2], self.B[1,0],self.B[1,1],self.B[1,2], self.B[2,0],self.B[2,1],self.B[2,2]) \
				+"\nB^(-1) =\n%10.4f %10.4f %10.4f\n%10.4f %10.4f %10.4f\n%10.4f %10.4f %10.4f\n" \
				% (invB[0,0],invB[0,1],invB[0,2], invB[1,0],invB[1,1],invB[1,2], invB[2,0],invB[2,1],invB[2,2]) \
				+"\nUB =\n%10.4f %10.4f %10.4f\n%10.4f %10.4f %10.4f\n%10.4f %10.4f %10.4f\n" \
				% (UB[0,0],UB[0,1],UB[0,2], UB[1,0],UB[1,1],UB[1,2], UB[2,0],UB[2,1],UB[2,2]) \
				+"\n(UB)^(-1) =\n%10.4f %10.4f %10.4f\n%10.4f %10.4f %10.4f\n%10.4f %10.4f %10.4f\n" \
				% (invUB[0,0],invUB[0,1],invUB[0,2], invUB[1,0],invUB[1,1],invUB[1,2], invUB[2,0],invUB[2,1],invUB[2,2]) \
			)
		except (ArithmeticError, la.LinAlgError) as err:
			self.editBMat.setPlainText("invalid")

		self.QChanged()
		self.QChanged_angles()


	def planeChanged(self):
		self.xtalChanged()
		#self.QChanged()
	# -----------------------------------------------------------------------------


	# -----------------------------------------------------------------------------
	# tas tab
	def TASChanged(self):
		a1 = self.getfloat(self.editA1.text()) / 180. * np.pi
		a2 = a1 * 2.
		a3 = self.getfloat(self.editA3.text()) / 180. * np.pi
		a4 = self.getfloat(self.editA4.text()) / 180. * np.pi
		a5 = self.getfloat(self.editA5.text()) / 180. * np.pi
		a6 = a5 * 2.
		dmono = self.getfloat(self.editDm.text())
		dana = self.getfloat(self.editDa.text())

		sense_sample = 1.
		if self.checkA4Sense.isChecked() == False:
			sense_sample = -1.

		self.editA2.setText("%.6g" % (a2 / np.pi * 180.))
		self.editA6.setText("%.6g" % (a6 / np.pi * 180.))

		try:
			ki = get_monok(a1, dmono)
			kf = get_monok(a5, dana)
			E = get_E(ki, kf)
			Qlen = get_Q(ki, kf, a4)
			Qvec = get_hkl(ki, kf, a3, Qlen, self.orient_rlu, self.orient_up_rlu, self.B, sense_sample, self.a3_offs)

			self.edith.setText("%.6g" % Qvec[0])
			self.editk.setText("%.6g" % Qvec[1])
			self.editl.setText("%.6g" % Qvec[2])
			self.editQAbs.setText("%.6g" % Qlen)
			self.editKi.setText("%.6g" % ki)
			self.editKf.setText("%.6g" % kf)
			self.editE.setText("%.6g" % E)
		except (ArithmeticError, la.LinAlgError) as err:
			self.edith.setText("invalid")
			self.editk.setText("invalid")
			self.editl.setText("invalid")
			self.editKi.setText("invalid")
			self.editKf.setText("invalid")
			self.editE.setText("invalid")


	def A2Changed(self):
		a2 = self.getfloat(self.editA2.text()) / 180. * np.pi
		self.editA1.setText("%.6g" % (0.5*a2 / np.pi * 180.))
		self.TASChanged()


	def A6Changed(self):
		a6 = self.getfloat(self.editA6.text()) / 180. * np.pi
		self.editA5.setText("%.6g" % (0.5*a6 / np.pi * 180.))
		self.TASChanged()


	def DChanged(self):
		self.QChanged()


	def QChanged(self):
		Q_rlu = np.array([self.getfloat(self.edith.text()), self.getfloat(self.editk.text()), self.getfloat(self.editl.text())])
		ki = self.getfloat(self.editKi.text())
		kf = self.getfloat(self.editKf.text())

		try:
			[a1, a2] = get_a1a2(ki, self.getfloat(self.editDm.text()))

			self.editA1.setText("%.6g" % (a1 / np.pi * 180.))
			self.editA2.setText("%.6g" % (a2 / np.pi * 180.))
		except (ArithmeticError, la.LinAlgError) as err:
			self.editA1.setText("invalid")
			self.editA2.setText("invalid")

		try:
			[a5, a6] = get_a1a2(kf, self.getfloat(self.editDa.text()))

			self.editA5.setText("%.6g" % (a5 / np.pi * 180.))
			self.editA6.setText("%.6g" % (a6 / np.pi * 180.))
		except (ArithmeticError, la.LinAlgError) as err:
			self.editA5.setText("invalid")
			self.editA6.setText("invalid")

		try:
			sense_sample = 1.
			if self.checkA4Sense.isChecked() == False:
				sense_sample = -1.

			[a3, a4, dist_Q_plane] = get_a3a4(ki, kf, Q_rlu, self.orient_rlu, self.orient_up_rlu, self.B, sense_sample, self.a3_offs)
			Qlen = get_Q(ki, kf, a4)
			Q_in_plane = np.abs(dist_Q_plane) < self.g_eps

			self.editA3.setText("%.6g" % (a3 / np.pi * 180.))
			self.editA4.setText("%.6g" % (a4 / np.pi * 180.))
			self.editQAbs.setText("%.6g" % Qlen)
			if Q_in_plane:
				self.tasstatus.setText("")
			else:
				metric = get_metric(self.B)
				#ang1 = angle(Q_rlu, self.orient_rlu, metric)
				#ang2 = angle(Q_rlu, self.orient2_rlu, metric)
				ang_plane = np.pi*0.5 - angle(Q_rlu, self.orient_up_rlu, metric)

				self.tasstatus.setText(u"WARNING: Q is out of the plane by %.4g \u212b\u207b\u00b9, i.e. %.4g deg!" \
					% (dist_Q_plane, ang_plane/np.pi*180.))

			if np.isnan(a4) or np.isnan(Qlen):
				self.tasstatus.setText(u"WARNING: Scattering triangle cannot be closed.")
				self.editA3.setText("invalid")
				self.editA4.setText("invalid")
				self.editQAbs.setText("invalid")
		except (ArithmeticError, la.LinAlgError) as err:
			self.editA3.setText("invalid")
			self.editA4.setText("invalid")


	def KiKfChanged(self):
		ki = self.getfloat(self.editKi.text())
		kf = self.getfloat(self.editKf.text())

		try:
			E = get_E(ki, kf)
			self.editE.setText("%.6g" % E)

			self.QChanged()
		except (ArithmeticError, la.LinAlgError) as err:
			self.editE.setText("invalid")


	def EChanged(self):
		E = self.getfloat(self.editE.text())
		kf = self.getfloat(self.editKf.text())

		try:
			ki = get_ki(kf, E)
			self.editKi.setText("%.6g" % ki)

			self.QChanged()
		except (ArithmeticError, la.LinAlgError) as err:
			self.editKi.setText("invalid")


	def QChanged_angles(self):
		Q_rlu1 = np.array([self.getfloat(self.edith1.text()), self.getfloat(self.editk1.text()), self.getfloat(self.editl1.text())])
		Q_rlu2 = np.array([self.getfloat(self.edith2.text()), self.getfloat(self.editk2.text()), self.getfloat(self.editl2.text())])
		ki1 = self.getfloat(self.editKi1.text())
		ki2 = self.getfloat(self.editKi2.text())
		kf1 = self.getfloat(self.editKf1.text())
		kf2 = self.getfloat(self.editKf2.text())
		di = self.getfloat(self.editDm.text())
		df = self.getfloat(self.editDa.text())

		speed_a1 = self.getfloat(self.editSpeedA1.text()) / 180.*np.pi
		speed_a2 = self.getfloat(self.editSpeedA2.text()) / 180.*np.pi
		speed_a3 = self.getfloat(self.editSpeedA3.text()) / 180.*np.pi
		speed_a4 = self.getfloat(self.editSpeedA4.text()) / 180.*np.pi
		speed_a5 = self.getfloat(self.editSpeedA5.text()) / 180.*np.pi
		speed_a6 = self.getfloat(self.editSpeedA6.text()) / 180.*np.pi

		try:
			sense_sample = 1.
			if self.checkA4Sense.isChecked() == False:
				sense_sample = -1.

			[da1, da2, da3, da4, da5, da6, dist1, dist2] = get_angle_deltas(\
				ki1, kf1, Q_rlu1, di, df, \
				ki2, kf2, Q_rlu2, di, df, \
				self.orient_rlu, self.orient_up_rlu, self.B, sense_sample, self.a3_offs)

			self.editdA1.setText("%.6g" % (da1 / np.pi * 180.))
			self.editdA2.setText("%.6g" % (da2 / np.pi * 180.))
			self.editdA3.setText("%.6g" % (da3 / np.pi * 180.))
			self.editdA4.setText("%.6g" % (da4 / np.pi * 180.))
			self.editdA5.setText("%.6g" % (da5 / np.pi * 180.))
			self.editdA6.setText("%.6g" % (da6 / np.pi * 180.))

			Q1_in_plane = np.abs(dist1) < self.g_eps
			Q2_in_plane = np.abs(dist2) < self.g_eps

			status = ""
			if not Q1_in_plane:
				status += "Position 1 is out-of-plane! "
			if not Q2_in_plane:
				status += "Position 2 is out-of-plane! "

			if status != "":
				status = "WARNING: " + status

			if status == "":
				driving = driving_time([da1, da2, da3, da4, da5, da6], \
					[speed_a1, speed_a2, speed_a3, speed_a4, speed_a5, speed_a6])
				status = "Instrument driving time: %.2f s" % (driving)

			self.anglesstatus.setText(status)

		except (ArithmeticError, la.LinAlgError) as err:
			self.editdA1.setText("invalid")
			self.editdA2.setText("invalid")
			self.editdA3.setText("invalid")
			self.editdA4.setText("invalid")
			self.editdA5.setText("invalid")
			self.editdA6.setText("invalid")


	def KiKfChanged_angles(self):
		ki1 = self.getfloat(self.editKi1.text())
		ki2 = self.getfloat(self.editKi2.text())
		kf1 = self.getfloat(self.editKf1.text())
		kf2 = self.getfloat(self.editKf2.text())

		try:
			E1 = get_E(ki1, kf1)
			self.editE1.setText("%.6g" % E1)
		except (ArithmeticError, la.LinAlgError) as err:
			self.editE1.setText("invalid")

		try:
			E2 = get_E(ki2, kf2)
			self.editE2.setText("%.6g" % E2)
		except (ArithmeticError, la.LinAlgError) as err:
			self.editE2.setText("invalid")

		self.QChanged_angles()


	def EChanged_angles(self):
		E1 = self.getfloat(self.editE1.text())
		kf1 = self.getfloat(self.editKf1.text())
		E2 = self.getfloat(self.editE2.text())
		kf2 = self.getfloat(self.editKf2.text())

		try:
			ki1 = get_ki(kf1, E1)
			self.editKi1.setText("%.6g" % ki1)
		except (ArithmeticError, la.LinAlgError) as err:
			self.editKi1.setText("invalid")

		try:
			ki2 = get_ki(kf2, E2)
			self.editKi2.setText("%.6g" % ki2)
		except (ArithmeticError, la.LinAlgError) as err:
			self.editKi2.setText("invalid")

		self.QChanged_angles()

	# -----------------------------------------------------------------------------


	# -----------------------------------------------------------------------------
	# info/settings tab
	def comboA3ConvChanged(self):
		idx = self.comboA3.currentIndex()
		self.a3_offs = self.a3_offsets[idx]
		self.QChanged()
	# -----------------------------------------------------------------------------


	#
	# show qt GUI
	#
	def __init__(self):
		# -----------------------------------------------------------------------------
		# dependencies
		# try to import qt5...
		try:
			import qtpy as qt
			import qtpy.QtCore as qtc
			import qtpy.QtGui as qtg
			import qtpy.QtWidgets as qtw
			qt_ver = 5
		except ImportError:
			print("Error: No suitable version of Qt was found!")
			exit(-1)
		# -----------------------------------------------------------------------------


		# -----------------------------------------------------------------------------
		# main application
		np.set_printoptions(suppress=True, precision=4)

		app = qtw.QApplication(sys.argv)
		app.setApplicationName("qtas")
		#app.setStyle("Fusion")

		sett = qtc.QSettings("tobis_stuff", "in20tool")
		if sett.contains("mainwnd/theme"):
			app.setStyle(sett.value("mainwnd/theme"))

		tabs = qtw.QTabWidget()
		# -----------------------------------------------------------------------------


		# -----------------------------------------------------------------------------
		# crystal tab
		xtalpanel = qtw.QWidget()
		xtallayout = qtw.QGridLayout(xtalpanel)

		self.editA = qtw.QLineEdit(xtalpanel)
		self.editB = qtw.QLineEdit(xtalpanel)
		self.editC = qtw.QLineEdit(xtalpanel)
		self.editAlpha = qtw.QLineEdit(xtalpanel)
		self.editBeta = qtw.QLineEdit(xtalpanel)
		self.editGamma = qtw.QLineEdit(xtalpanel)
		separatorXtal = qtw.QFrame(xtalpanel)
		separatorXtal.setFrameStyle(qtw.QFrame.HLine)
		self.editAx = qtw.QLineEdit(xtalpanel)
		self.editAy = qtw.QLineEdit(xtalpanel)
		self.editAz = qtw.QLineEdit(xtalpanel)
		self.editBx = qtw.QLineEdit(xtalpanel)
		self.editBy = qtw.QLineEdit(xtalpanel)
		self.editBz = qtw.QLineEdit(xtalpanel)
		self.editBMat = qtw.QPlainTextEdit(xtalpanel)
		self.editBMat.setReadOnly(True)

		self.editA.textEdited.connect(self.xtalChanged)
		self.editB.textEdited.connect(self.xtalChanged)
		self.editC.textEdited.connect(self.xtalChanged)
		self.editAlpha.textEdited.connect(self.xtalChanged)
		self.editBeta.textEdited.connect(self.xtalChanged)
		self.editGamma.textEdited.connect(self.xtalChanged)
		self.editAx.textEdited.connect(self.planeChanged)
		self.editAy.textEdited.connect(self.planeChanged)
		self.editAz.textEdited.connect(self.planeChanged)
		self.editBx.textEdited.connect(self.planeChanged)
		self.editBy.textEdited.connect(self.planeChanged)
		self.editBz.textEdited.connect(self.planeChanged)


		self.editA.setText("%.6g" % sett.value("qtas/a", 5., type=float))
		self.editB.setText("%.6g" % sett.value("qtas/b", 5., type=float))
		self.editC.setText("%.6g" % sett.value("qtas/c", 5., type=float))
		self.editAlpha.setText("%.6g" % sett.value("qtas/alpha", 90., type=float))
		self.editBeta.setText("%.6g" % sett.value("qtas/beta", 90., type=float))
		self.editGamma.setText("%.6g" % sett.value("qtas/gamma", 90., type=float))
		self.editAx.setText("%.6g" % sett.value("qtas/ax", 1., type=float))
		self.editAy.setText("%.6g" % sett.value("qtas/ay", 0., type=float))
		self.editAz.setText("%.6g" % sett.value("qtas/az", 0., type=float))
		self.editBx.setText("%.6g" % sett.value("qtas/bx", 0., type=float))
		self.editBy.setText("%.6g" % sett.value("qtas/by", 1., type=float))
		self.editBz.setText("%.6g" % sett.value("qtas/bz", 0., type=float))


		xtallayout.addWidget(qtw.QLabel(u"a (\u212b):", xtalpanel), 0,0, 1,1)
		xtallayout.addWidget(self.editA, 0,1, 1,3)
		xtallayout.addWidget(qtw.QLabel(u"b (\u212b):", xtalpanel), 1,0, 1,1)
		xtallayout.addWidget(self.editB, 1,1, 1,3)
		xtallayout.addWidget(qtw.QLabel(u"c (\u212b):", xtalpanel), 2,0, 1,1)
		xtallayout.addWidget(self.editC, 2,1, 1,3)
		xtallayout.addWidget(qtw.QLabel(u"\u03b1 (deg):", xtalpanel), 3,0, 1,1)
		xtallayout.addWidget(self.editAlpha, 3,1, 1,3)
		xtallayout.addWidget(qtw.QLabel(u"\u03b2 (deg):", xtalpanel), 4,0, 1,1)
		xtallayout.addWidget(self.editBeta, 4,1, 1,3)
		xtallayout.addWidget(qtw.QLabel(u"\u03b3 (deg):", xtalpanel), 5,0, 1,1)
		xtallayout.addWidget(self.editGamma, 5,1, 1,3)

		xtallayout.addWidget(separatorXtal, 6,0, 1,4)
		xtallayout.addWidget(qtw.QLabel("Orient. 1 (rlu):", xtalpanel), 7,0, 1,1)
		xtallayout.addWidget(self.editAx, 7,1, 1,1)
		xtallayout.addWidget(self.editAy, 7,2, 1,1)
		xtallayout.addWidget(self.editAz, 7,3, 1,1)
		xtallayout.addWidget(qtw.QLabel("Orient. 2 (rlu):", xtalpanel), 8,0, 1,1)
		xtallayout.addWidget(self.editBx, 8,1, 1,1)
		xtallayout.addWidget(self.editBy, 8,2, 1,1)
		xtallayout.addWidget(self.editBz, 8,3, 1,1)
		xtallayout.addWidget(self.editBMat, 9,0, 2,4)

		tabs.addTab(xtalpanel, "Crystal")
		# -----------------------------------------------------------------------------


		# -----------------------------------------------------------------------------
		# tas tab
		taspanel = qtw.QWidget()
		taslayout = qtw.QGridLayout(taspanel)

		self.editA1 = qtw.QLineEdit(taspanel)
		self.editA2 = qtw.QLineEdit(taspanel)
		self.editA3 = qtw.QLineEdit(taspanel)
		self.editA4 = qtw.QLineEdit(taspanel)
		self.editA5 = qtw.QLineEdit(taspanel)
		self.editA6 = qtw.QLineEdit(taspanel)

		self.checkA4Sense = qtw.QCheckBox(taspanel)

		self.editDm = qtw.QLineEdit(taspanel)
		self.editDa = qtw.QLineEdit(taspanel)

		self.edith = qtw.QLineEdit(taspanel)
		self.editk = qtw.QLineEdit(taspanel)
		self.editl = qtw.QLineEdit(taspanel)
		self.editE = qtw.QLineEdit(taspanel)
		self.editKi = qtw.QLineEdit(taspanel)
		self.editKf = qtw.QLineEdit(taspanel)
		self.editQAbs = qtw.QLineEdit(taspanel)
		self.editQAbs.setReadOnly(True)

		self.tasstatus = qtw.QLabel(taspanel)

		separatorTas = qtw.QFrame(taspanel)
		separatorTas.setFrameStyle(qtw.QFrame.HLine)
		separatorTas2 = qtw.QFrame(taspanel)
		separatorTas2.setFrameStyle(qtw.QFrame.HLine)
		separatorTas3 = qtw.QFrame(taspanel)
		separatorTas3.setFrameStyle(qtw.QFrame.HLine)


		self.editA1.textEdited.connect(self.TASChanged)
		self.editA3.textEdited.connect(self.TASChanged)
		self.editA4.textEdited.connect(self.TASChanged)
		self.editA5.textEdited.connect(self.TASChanged)
		self.editA2.textEdited.connect(self.A2Changed)
		self.editA6.textEdited.connect(self.A6Changed)
		self.editDm.textEdited.connect(self.DChanged)
		self.editDa.textEdited.connect(self.DChanged)
		self.edith.textEdited.connect(self.QChanged)
		self.editk.textEdited.connect(self.QChanged)
		self.editl.textEdited.connect(self.QChanged)
		self.editKi.textEdited.connect(self.KiKfChanged)
		self.editKf.textEdited.connect(self.KiKfChanged)
		self.editE.textEdited.connect(self.EChanged)


		self.editDm.setText("%.6g" % sett.value("qtas/dm", 3.355, type=float))
		self.editDa.setText("%.6g" % sett.value("qtas/da", 3.355, type=float))
		self.edith.setText("%.6g" % sett.value("qtas/h", 1., type=float))
		self.editk.setText("%.6g" % sett.value("qtas/k", 0., type=float))
		self.editl.setText("%.6g" % sett.value("qtas/l", 0., type=float))
		#self.editE.setText("%.6g" % sett.value("qtas/E", 0., type=float))
		self.editKi.setText("%.6g" % sett.value("qtas/ki", 2.662, type=float))
		self.editKf.setText("%.6g" % sett.value("qtas/kf", 2.662, type=float))


		self.checkA4Sense.setText("a4 sense is counter-clockwise")
		self.checkA4Sense.setChecked(sett.value("qtas/a4_sense", 1, type=bool))
		self.checkA4Sense.stateChanged.connect(self.QChanged)


		taslayout.addWidget(qtw.QLabel("h (rlu):", taspanel), 0,0, 1,1)
		taslayout.addWidget(self.edith, 0,1, 1,2)
		taslayout.addWidget(qtw.QLabel("k (rlu):", taspanel), 1,0, 1,1)
		taslayout.addWidget(self.editk, 1,1, 1,2)
		taslayout.addWidget(qtw.QLabel("l (rlu):", taspanel), 2,0, 1,1)
		taslayout.addWidget(self.editl, 2,1, 1,2)
		taslayout.addWidget(qtw.QLabel("E (meV):", taspanel), 3,0, 1,1)
		taslayout.addWidget(self.editE, 3,1, 1,2)
		taslayout.addWidget(qtw.QLabel(u"ki, kf (\u212b\u207b\u00b9):", taspanel), 4,0, 1,1)
		taslayout.addWidget(self.editKi, 4,1, 1,1)
		taslayout.addWidget(self.editKf, 4,2, 1,1)
		taslayout.addWidget(qtw.QLabel(u"|Q| (\u212b\u207b\u00b9):", taspanel), 5,0, 1,1)
		taslayout.addWidget(self.editQAbs, 5,1, 1,2)

		taslayout.addWidget(separatorTas, 6,0,1,3)
		taslayout.addWidget(qtw.QLabel("a1, a2 (deg):", taspanel), 7,0, 1,1)
		taslayout.addWidget(self.editA1, 7,1, 1,1)
		taslayout.addWidget(self.editA2, 7,2, 1,1)
		taslayout.addWidget(qtw.QLabel("a3, a4 (deg):", taspanel), 8,0, 1,1)
		taslayout.addWidget(self.editA3, 8,1, 1,1)
		taslayout.addWidget(self.editA4, 8,2, 1,1)
		taslayout.addWidget(qtw.QLabel("a5, a6 (deg):", taspanel), 9,0, 1,1)
		taslayout.addWidget(self.editA5, 9,1, 1,1)
		taslayout.addWidget(self.editA6, 9,2, 1,1)

		taslayout.addWidget(separatorTas2, 10,0, 1,3)
		taslayout.addWidget(qtw.QLabel("Sense:", taspanel), 11,0, 1,1)
		taslayout.addWidget(self.checkA4Sense, 11,1, 1,2)

		taslayout.addWidget(separatorTas3, 12,0, 1,3)
		taslayout.addWidget(qtw.QLabel(u"Mono., Ana. d (\u212b):", taspanel), 13,0, 1,1)
		taslayout.addWidget(self.editDm, 13,1, 1,1)
		taslayout.addWidget(self.editDa, 13,2, 1,1)

		taslayout.addItem(qtw.QSpacerItem(16,16, qtw.QSizePolicy.Minimum, qtw.QSizePolicy.Expanding), 14,0, 1,3)
		taslayout.addWidget(self.tasstatus, 15,0, 1,3)

		tabs.addTab(taspanel, "TAS")
		# -----------------------------------------------------------------------------


		# -----------------------------------------------------------------------------
		# angular distance tab
		anglespanel = qtw.QWidget()
		angleslayout = qtw.QGridLayout(anglespanel)

		self.editdA1 = qtw.QLineEdit(anglespanel)
		self.editdA2 = qtw.QLineEdit(anglespanel)
		self.editdA3 = qtw.QLineEdit(anglespanel)
		self.editdA4 = qtw.QLineEdit(anglespanel)
		self.editdA5 = qtw.QLineEdit(anglespanel)
		self.editdA6 = qtw.QLineEdit(anglespanel)

		self.editdA1.setReadOnly(True)
		self.editdA2.setReadOnly(True)
		self.editdA3.setReadOnly(True)
		self.editdA4.setReadOnly(True)
		self.editdA5.setReadOnly(True)
		self.editdA6.setReadOnly(True)

		self.editSpeedA1 = qtw.QLineEdit(anglespanel)
		self.editSpeedA2 = qtw.QLineEdit(anglespanel)
		self.editSpeedA3 = qtw.QLineEdit(anglespanel)
		self.editSpeedA4 = qtw.QLineEdit(anglespanel)
		self.editSpeedA5 = qtw.QLineEdit(anglespanel)
		self.editSpeedA6 = qtw.QLineEdit(anglespanel)

		self.edith1 = qtw.QLineEdit(anglespanel)
		self.editk1 = qtw.QLineEdit(anglespanel)
		self.editl1 = qtw.QLineEdit(anglespanel)
		self.editE1 = qtw.QLineEdit(anglespanel)
		self.editKi1 = qtw.QLineEdit(anglespanel)
		self.editKf1 = qtw.QLineEdit(anglespanel)

		self.edith2 = qtw.QLineEdit(anglespanel)
		self.editk2 = qtw.QLineEdit(anglespanel)
		self.editl2 = qtw.QLineEdit(anglespanel)
		self.editE2 = qtw.QLineEdit(anglespanel)
		self.editKi2 = qtw.QLineEdit(anglespanel)
		self.editKf2 = qtw.QLineEdit(anglespanel)

		self.anglesstatus = qtw.QLabel(anglespanel)

		separatorAngles = qtw.QFrame(anglespanel)
		separatorAngles.setFrameStyle(qtw.QFrame.HLine)
		separatorAngles2 = qtw.QFrame(anglespanel)
		separatorAngles2.setFrameStyle(qtw.QFrame.HLine)


		self.edith1.textEdited.connect(self.QChanged_angles)
		self.editk1.textEdited.connect(self.QChanged_angles)
		self.editl1.textEdited.connect(self.QChanged_angles)
		self.editKi1.textEdited.connect(self.KiKfChanged_angles)
		self.editKf1.textEdited.connect(self.KiKfChanged_angles)
		self.editE1.textEdited.connect(self.EChanged_angles)

		self.edith2.textEdited.connect(self.QChanged_angles)
		self.editk2.textEdited.connect(self.QChanged_angles)
		self.editl2.textEdited.connect(self.QChanged_angles)
		self.editKi2.textEdited.connect(self.KiKfChanged_angles)
		self.editKf2.textEdited.connect(self.KiKfChanged_angles)
		self.editE2.textEdited.connect(self.EChanged_angles)

		self.editSpeedA1.textEdited.connect(self.QChanged_angles)
		self.editSpeedA2.textEdited.connect(self.QChanged_angles)
		self.editSpeedA3.textEdited.connect(self.QChanged_angles)
		self.editSpeedA4.textEdited.connect(self.QChanged_angles)
		self.editSpeedA5.textEdited.connect(self.QChanged_angles)
		self.editSpeedA6.textEdited.connect(self.QChanged_angles)


		self.edith1.setText("%.6g" % sett.value("qtas/h1", 1., type=float))
		self.editk1.setText("%.6g" % sett.value("qtas/k1", 0., type=float))
		self.editl1.setText("%.6g" % sett.value("qtas/l1", 0., type=float))
		#self.editE1.setText("%.6g" % sett.value("qtas/E1", 0., type=float))
		self.editKi1.setText("%.6g" % sett.value("qtas/ki1", 2.662, type=float))
		self.editKf1.setText("%.6g" % sett.value("qtas/kf1", 2.662, type=float))

		self.edith2.setText("%.6g" % sett.value("qtas/h2", 1., type=float))
		self.editk2.setText("%.6g" % sett.value("qtas/k2", 0., type=float))
		self.editl2.setText("%.6g" % sett.value("qtas/l2", 0., type=float))
		#self.editE2.setText("%.6g" % sett.value("qtas/E2", 0., type=float))
		self.editKi2.setText("%.6g" % sett.value("qtas/ki2", 2.662, type=float))
		self.editKf2.setText("%.6g" % sett.value("qtas/kf2", 2.662, type=float))

		self.editSpeedA1.setText("%.2f" % sett.value("qtas/v_a1", 0.15, type=float))
		self.editSpeedA2.setText("%.2f" % sett.value("qtas/v_a2", 0.15, type=float))
		self.editSpeedA3.setText("%.2f" % sett.value("qtas/v_a3", 1.25, type=float))
		self.editSpeedA4.setText("%.2f" % sett.value("qtas/v_a4", 1.88, type=float))
		self.editSpeedA5.setText("%.2f" % sett.value("qtas/v_a5", 1., type=float))
		self.editSpeedA6.setText("%.2f" % sett.value("qtas/v_a6", 1., type=float))


		angleslayout.addWidget(qtw.QLabel("Position 1:", anglespanel), 0,1, 1,1)
		angleslayout.addWidget(qtw.QLabel("Position 2:", anglespanel), 0,2, 1,1)
		angleslayout.addWidget(qtw.QLabel("h (rlu):", anglespanel), 1,0, 1,1)
		angleslayout.addWidget(self.edith1, 1,1, 1,1)
		angleslayout.addWidget(self.edith2, 1,2, 1,1)
		angleslayout.addWidget(qtw.QLabel("k (rlu):", anglespanel), 2,0, 1,1)
		angleslayout.addWidget(self.editk1, 2,1, 1,1)
		angleslayout.addWidget(self.editk2, 2,2, 1,1)
		angleslayout.addWidget(qtw.QLabel("l (rlu):", anglespanel), 3,0, 1,1)
		angleslayout.addWidget(self.editl1, 3,1, 1,1)
		angleslayout.addWidget(self.editl2, 3,2, 1,1)
		angleslayout.addWidget(qtw.QLabel("E (meV):", anglespanel), 4,0, 1,1)
		angleslayout.addWidget(self.editE1, 4,1, 1,1)
		angleslayout.addWidget(self.editE2, 4,2, 1,1)
		angleslayout.addWidget(qtw.QLabel(u"ki (\u212b\u207b\u00b9):", anglespanel), 5,0, 1,1)
		angleslayout.addWidget(self.editKi1, 5,1, 1,1)
		angleslayout.addWidget(self.editKi2, 5,2, 1,1)
		angleslayout.addWidget(qtw.QLabel(u"kf (\u212b\u207b\u00b9):", anglespanel), 6,0, 1,1)
		angleslayout.addWidget(self.editKf1, 6,1, 1,1)
		angleslayout.addWidget(self.editKf2, 6,2, 1,1)

		angleslayout.addWidget(separatorAngles, 7,0,1,3)
		angleslayout.addWidget(qtw.QLabel("Motor Speeds:", anglespanel), 8,1, 1,3)
		angleslayout.addWidget(qtw.QLabel("v_a1, v_a2 (deg/s):", anglespanel), 9,0, 1,1)
		angleslayout.addWidget(self.editSpeedA1, 9,1, 1,1)
		angleslayout.addWidget(self.editSpeedA2, 9,2, 1,1)
		angleslayout.addWidget(qtw.QLabel("v_a3, v_a4 (deg/s):", anglespanel), 10,0, 1,1)
		angleslayout.addWidget(self.editSpeedA3, 10,1, 1,1)
		angleslayout.addWidget(self.editSpeedA4, 10,2, 1,1)
		angleslayout.addWidget(qtw.QLabel("v_a5, v_a6 (deg/s):", anglespanel), 11,0, 1,1)
		angleslayout.addWidget(self.editSpeedA5, 11,1, 1,1)
		angleslayout.addWidget(self.editSpeedA6, 11,2, 1,1)

		angleslayout.addWidget(separatorAngles2, 12,0,1,3)
		angleslayout.addWidget(qtw.QLabel("Angular Distances:", anglespanel), 13,1, 1,3)
		angleslayout.addWidget(qtw.QLabel("\u0394a1, \u0394a2 (deg):", anglespanel), 14,0, 1,1)
		angleslayout.addWidget(self.editdA1, 14,1, 1,1)
		angleslayout.addWidget(self.editdA2, 14,2, 1,1)
		angleslayout.addWidget(qtw.QLabel("\u0394a3, \u0394a4 (deg):", anglespanel), 15,0, 1,1)
		angleslayout.addWidget(self.editdA3, 15,1, 1,1)
		angleslayout.addWidget(self.editdA4, 15,2, 1,1)
		angleslayout.addWidget(qtw.QLabel("\u0394a5, \u0394a6 (deg):", anglespanel), 16,0, 1,1)
		angleslayout.addWidget(self.editdA5, 16,1, 1,1)
		angleslayout.addWidget(self.editdA6, 16,2, 1,1)

		angleslayout.addItem(qtw.QSpacerItem(16,16, qtw.QSizePolicy.Minimum, qtw.QSizePolicy.Expanding), 17,0, 1,3)
		angleslayout.addWidget(self.anglesstatus, 18,0, 1,3)

		tabs.addTab(anglespanel, "Distances")
		# -----------------------------------------------------------------------------


		# -----------------------------------------------------------------------------
		# info/settings tab
		infopanel = qtw.QWidget()
		infolayout = qtw.QGridLayout(infopanel)

		self.comboA3 = qtw.QComboBox(infopanel)
		self.comboA3.addItems(["Takin", "NOMAD", "SICS", "NICOS"])
		self.a3_offsets = [np.pi/2., np.pi, 0., 0.]
		self.comboA3.setCurrentIndex(sett.value("qtas/a3_conv", 1, type=int))

		self.comboA3.currentIndexChanged.connect(self.comboA3ConvChanged)


		separatorInfo = qtw.QFrame(infopanel)
		separatorInfo.setFrameStyle(qtw.QFrame.HLine)


		infolayout.addWidget(qtw.QLabel("TAS Calculator (doi: <a href=\"https://doi.org/10.5281/zenodo.4117437\">10.5281/zenodo.4117437</a>).", infopanel), 0,0, 1,2)
		infolayout.addWidget(qtw.QLabel("Written by Tobias Weber <tweber@ill.fr>.", infopanel), 1,0, 1,2)
		infolayout.addWidget(qtw.QLabel("Date: October 24, 2018.", infopanel), 2,0, 1,2)
		infolayout.addWidget(separatorInfo, 3,0, 1,2)
		infolayout.addWidget(qtw.QLabel("Interpreter Version: " + sys.version + ".", infopanel), 4,0, 1,2)
		infolayout.addWidget(qtw.QLabel("Numpy Version: " + np.__version__ + ".", infopanel), 5,0, 1,2)
		infolayout.addWidget(qtw.QLabel("Qt Version: " + qtc.QT_VERSION_STR + ".", infopanel), 6,0, 1,2)
		infolayout.addItem(qtw.QSpacerItem(16,16, qtw.QSizePolicy.Minimum, qtw.QSizePolicy.Expanding), 7,0, 1,2)
		infolayout.addWidget(qtw.QLabel("A3 Convention:", infopanel), 8,0, 1,1)
		infolayout.addWidget(self.comboA3, 8,1, 1,1)

		tabs.addTab(infopanel, "Infos")
		# -----------------------------------------------------------------------------


		# -----------------------------------------------------------------------------
		# main dialog window
		dlg = qtw.QDialog()
		dlg.setWindowTitle("TAS Calculator")
		mainlayout = qtw.QGridLayout(dlg)
		mainlayout.addWidget(tabs)

		if sett.contains("qtas/geo"):
			geo = sett.value("qtas/geo")
			if qt_ver == 4:
				try:
					geo = geo.toByteArray()
				except AttributeError as err:
					pass
			dlg.restoreGeometry(geo)

		self.xtalChanged()
		self.KiKfChanged()
		self.comboA3ConvChanged()
		#self.QChanged()
		self.KiKfChanged_angles()

		dlg.show()
		app.exec_()


		# save settings
		sett.setValue("qtas/geo", dlg.saveGeometry())

		sett.setValue("qtas/a", self.getfloat(self.editA.text()))
		sett.setValue("qtas/b", self.getfloat(self.editB.text()))
		sett.setValue("qtas/c", self.getfloat(self.editC.text()))
		sett.setValue("qtas/alpha", self.getfloat(self.editAlpha.text()))
		sett.setValue("qtas/beta", self.getfloat(self.editBeta.text()))
		sett.setValue("qtas/gamma", self.getfloat(self.editGamma.text()))
		sett.setValue("qtas/ax", self.getfloat(self.editAx.text()))
		sett.setValue("qtas/ay", self.getfloat(self.editAy.text()))
		sett.setValue("qtas/az", self.getfloat(self.editAz.text()))
		sett.setValue("qtas/bx", self.getfloat(self.editBx.text()))
		sett.setValue("qtas/by", self.getfloat(self.editBy.text()))
		sett.setValue("qtas/bz", self.getfloat(self.editBz.text()))

		sett.setValue("qtas/dm", self.getfloat(self.editDm.text()))
		sett.setValue("qtas/da", self.getfloat(self.editDa.text()))
		sett.setValue("qtas/h", self.getfloat(self.edith.text()))
		sett.setValue("qtas/k", self.getfloat(self.editk.text()))
		sett.setValue("qtas/l", self.getfloat(self.editl.text()))
		#sett.setValue("qtas/E", self.getfloat(self.editE.text()))
		sett.setValue("qtas/ki", self.getfloat(self.editKi.text()))
		sett.setValue("qtas/kf", self.getfloat(self.editKf.text()))
		sett.setValue("qtas/a3_conv", self.comboA3.currentIndex())
		sett.setValue("qtas/a4_sense", self.checkA4Sense.isChecked())

		sett.setValue("qtas/h1", self.getfloat(self.edith1.text()))
		sett.setValue("qtas/k1", self.getfloat(self.editk1.text()))
		sett.setValue("qtas/l1", self.getfloat(self.editl1.text()))
		sett.setValue("qtas/ki1", self.getfloat(self.editKi1.text()))
		sett.setValue("qtas/kf1", self.getfloat(self.editKf1.text()))
		#sett.setValue("qtas/E1", self.getfloat(self.editE1.text()))
		sett.setValue("qtas/h2", self.getfloat(self.edith2.text()))
		sett.setValue("qtas/k2", self.getfloat(self.editk2.text()))
		sett.setValue("qtas/l2", self.getfloat(self.editl2.text()))
		sett.setValue("qtas/ki2", self.getfloat(self.editKi2.text()))
		sett.setValue("qtas/kf2", self.getfloat(self.editKf2.text()))
		#sett.setValue("qtas/E2", self.getfloat(self.editE2.text()))

		sett.setValue("qtas/v_a1", self.getfloat(self.editSpeedA1.text()))
		sett.setValue("qtas/v_a2", self.getfloat(self.editSpeedA2.text()))
		sett.setValue("qtas/v_a3", self.getfloat(self.editSpeedA3.text()))
		sett.setValue("qtas/v_a4", self.getfloat(self.editSpeedA4.text()))
		sett.setValue("qtas/v_a5", self.getfloat(self.editSpeedA5.text()))
		sett.setValue("qtas/v_a6", self.getfloat(self.editSpeedA6.text()))
		# -----------------------------------------------------------------------------



# -----------------------------------------------------------------------------
#
# main
#
if __name__ == "__main__":
	gui = TasGUI()
# -----------------------------------------------------------------------------
