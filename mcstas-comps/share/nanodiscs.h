#include <gsl/gsl_sf_bessel.h>
// Functions used for compution the intensity from a given liposome
double FormfactorCylinder(double q, double MajorSemiAxis, double MinorSemiAxis, double Height, double Alpha, double Beta)
	{
		double const ProjectedRadius = sqrt(pow(MajorSemiAxis * sin(Beta), 2) + pow(MinorSemiAxis * cos(Beta), 2));

		double const Formfactor1 = gsl_sf_bessel_J1(q * ProjectedRadius * sin(Alpha)) / (q * ProjectedRadius * sin(Alpha));
		double const Formfactor2 = sin(q * Height * cos(Alpha) / 2.0) / (q * Height * cos(Alpha) / 2.0);

		return 2 * Formfactor1 * Formfactor2;
	}

	double IntensityOfEmptyNanodiscs(double q, double MajorSemiAxis, double MinorSemiAxis, double ThicknessOfBelt,
									 double HeightOfBelt, double HeightOfLipids, double HeightOfTails, double HeightOfCH3,
									 double DeltaRhoBelt, double DeltaRhoHead, double DeltaRhoCH2Tail, double DeltaRhoCH3Tail)
	{
		// Declaration
		double Intensity;
		double IntensityPart;

		double AmplitudeOfBelt;
		double AmplitudeOfHeads;
		double AmplitudeOfCH2Tail;
		double AmplitudeOfCH3Tail;

		double FormfactorOfBelt;
		double FormfactorOfHeads;
		double FormfactorOfCH2Tail;
		double FormfactorOfCH3Tail;

		const double OuterMajorSemiAxis = MajorSemiAxis + ThicknessOfBelt;
		const double OuterMinorSemiAxis = MinorSemiAxis + ThicknessOfBelt;

		const double VolumeOfBelt    = PI * HeightOfBelt * OuterMajorSemiAxis * OuterMinorSemiAxis - PI * HeightOfBelt * MajorSemiAxis * MinorSemiAxis;
		const double VolumeOfHeads   = PI * HeightOfLipids * MajorSemiAxis * MinorSemiAxis         - PI * HeightOfTails * MajorSemiAxis * MinorSemiAxis;
		const double VolumeOfCH2Tail = PI * HeightOfTails * MajorSemiAxis * MinorSemiAxis          - PI * HeightOfCH3 * MajorSemiAxis * MinorSemiAxis;
		const double VolumeOfCH3Tail = PI * HeightOfCH3 * MajorSemiAxis * MinorSemiAxis;

		// Variables needed for integration over alpha
		int i;
		const int NumberOfStepsInAlpha = 50;
		double Alpha;
		const double AlphaMin = 0.0;
		const double AlphaMax = PI / 2.0;
		const double AlphaStep = (AlphaMax - AlphaMin) / (1.0 * NumberOfStepsInAlpha);

		// Variables needed in integration over beta
		int j;
		const int NumberOfStepsInBeta = 50;
		double Beta;
		const double BetaMin = 0.0;
		const double BetaMax = PI / 2.0;
		const double BetaStep = (BetaMax - BetaMin) / (1.0 * NumberOfStepsInBeta);

		// Computation
		Intensity = 0.0;

		for (i = 0; i < NumberOfStepsInAlpha; ++i) {
			Alpha = (i + 0.5) * AlphaStep;

			for (j = 0; j < NumberOfStepsInBeta; ++j) {
				Beta = (j + 0.5) * BetaStep;

				// Compute formfactors
				FormfactorOfBelt = (PI * HeightOfBelt * OuterMajorSemiAxis * OuterMinorSemiAxis * FormfactorCylinder(q, OuterMajorSemiAxis, OuterMinorSemiAxis, HeightOfBelt, Alpha, Beta) -
								    PI * HeightOfBelt * MajorSemiAxis * MinorSemiAxis * FormfactorCylinder(q, MajorSemiAxis, MinorSemiAxis, HeightOfBelt, Alpha, Beta)) /
								   (PI * HeightOfBelt * OuterMajorSemiAxis * OuterMinorSemiAxis - PI * HeightOfBelt * MajorSemiAxis * MinorSemiAxis);

				FormfactorOfHeads = (PI * HeightOfLipids * MajorSemiAxis * MinorSemiAxis * FormfactorCylinder(q, MajorSemiAxis, MinorSemiAxis, HeightOfLipids, Alpha, Beta) -
								     PI * HeightOfTails  * MajorSemiAxis * MinorSemiAxis * FormfactorCylinder(q, MajorSemiAxis, MinorSemiAxis, HeightOfTails , Alpha, Beta)) /
								    (PI * HeightOfLipids * MajorSemiAxis * MinorSemiAxis - PI * HeightOfTails * MajorSemiAxis * MinorSemiAxis);

				FormfactorOfCH2Tail = (PI * HeightOfTails * MajorSemiAxis * MinorSemiAxis * FormfactorCylinder(q, MajorSemiAxis, MinorSemiAxis, HeightOfTails, Alpha, Beta) -
								       PI * HeightOfCH3   * MajorSemiAxis * MinorSemiAxis * FormfactorCylinder(q, MajorSemiAxis, MinorSemiAxis, HeightOfCH3  , Alpha, Beta)) /
								      (PI * HeightOfTails * MajorSemiAxis * MinorSemiAxis - PI * HeightOfCH3 * MajorSemiAxis * MinorSemiAxis);

				FormfactorOfCH3Tail = FormfactorCylinder(q, MajorSemiAxis, MinorSemiAxis, HeightOfCH3, Alpha, Beta);

				// Compute amplitudes
				AmplitudeOfBelt    = DeltaRhoBelt * VolumeOfBelt * FormfactorOfBelt;
				AmplitudeOfHeads   = DeltaRhoHead * VolumeOfHeads * FormfactorOfHeads;
				AmplitudeOfCH2Tail = DeltaRhoCH2Tail * VolumeOfCH2Tail * FormfactorOfCH2Tail;
				AmplitudeOfCH3Tail = DeltaRhoCH3Tail * VolumeOfCH3Tail * FormfactorOfCH3Tail;

				// Perform integration
				IntensityPart = pow(AmplitudeOfBelt + AmplitudeOfHeads + AmplitudeOfCH2Tail + AmplitudeOfCH3Tail, 2);

				Intensity += 2 / PI * sin(Alpha) * IntensityPart * AlphaStep * BetaStep;
			}
		}

		return Intensity;
	}
	// Functions used to compute the characteristics of the histidine tags
	double Debye(double q, double r)
	{
		// Variables needed in functions
		double ReturnValue;

		// Constants needed in function
		double Dummy1;
		double Dummy2;

		// Computation
		Dummy1 = (q * q) * (r * r);
		Dummy2 = 2 * ((exp(-Dummy1) + Dummy1 - 1) / (Dummy1 * Dummy1));

		ReturnValue = Dummy2;

		return ReturnValue;
	}

	double XiRodWithoutEndcaps(double q, double r, double l, double Alpha)
	{
		// Variables needed in function
		double ReturnValue;

		// Computation
		ReturnValue = j0(q * r * sin(Alpha)) * sin(q * l * cos(Alpha) / 2.f) / (q * l * cos(Alpha) / 2.f);

		return ReturnValue;
	}

	double PsiHammouda(double x)
	{
		// Variables used in function
		double ReturnValue;

		// Computation
		ReturnValue = (1.0 - exp(-pow(x, 2))) / pow(x, 2);

		return ReturnValue;
	}

	// Computation of the intensity from a given nanodisc
	double IntensityOfEmptyNanodiscsWithTags(
		double q,
		double MajorSemiAxis,
		double MinorSemiAxis,
		double ThicknessOfBelt,
		double HeightOfBelt,
		double HeightOfLipids,
		double HeightOfTails,
		double HeightOfCH3,
		double RadiusOfGyrationForHisTag,
		double VolumeOfOneHisTag,
		double DeltaRhoBelt,
		double DeltaRhoHead,
		double DeltaRhoCH2Tail,
		double DeltaRhoCH3Tail,
		double DeltaRhoTag)
	{
		// Declarations
		double Intensity;
		double IntensityPart;
		double IntensityDisc;

		double AmplitudeOfBelt;
		double AmplitudeOfHeads;
		double AmplitudeOfCH2Tail;
		double AmplitudeOfCH3Tail;

		double FormfactorOfBelt;
		double FormfactorOfHeads;
		double FormfactorOfCH2Tail;
		double FormfactorOfCH3Tail;

		const double OuterMajorSemiAxis = MajorSemiAxis + ThicknessOfBelt;
		const double OuterMinorSemiAxis = MinorSemiAxis + ThicknessOfBelt;
		const double AverageRadiusOfBelt = sqrt(pow(OuterMajorSemiAxis, 2) + pow(OuterMinorSemiAxis, 2));

		const double VolumeOfBelt    = PI * HeightOfBelt * OuterMajorSemiAxis * OuterMinorSemiAxis - PI * HeightOfBelt * MajorSemiAxis * MinorSemiAxis;
		const double VolumeOfHeads   = PI * HeightOfLipids * MajorSemiAxis * MinorSemiAxis         - PI * HeightOfTails * MajorSemiAxis * MinorSemiAxis;
		const double VolumeOfCH2Tail = PI * HeightOfTails * MajorSemiAxis * MinorSemiAxis          - PI * HeightOfCH3 * MajorSemiAxis * MinorSemiAxis;
		const double VolumeOfCH3Tail = PI * HeightOfCH3 * MajorSemiAxis * MinorSemiAxis;

		double Autocorrelation;
		double Crosscorrelation;
		double Disccorrelation;
		double FormfactorOfTags;

		// Variables needed for integration over alpha
		int i;
		const int NumberOfStepsInAlpha = 50;
		double Alpha;
		const double AlphaMin = 0.0;
		const double AlphaMax = PI / 2.0;
		const double AlphaStep = (AlphaMax - AlphaMin) / (1.0 * NumberOfStepsInAlpha);

		// Variables needed in integration over beta
		int j;
		const int NumberOfStepsInBeta = 50;
		double Beta;
		const double BetaMin = 0.0;
		const double BetaMax = PI / 2.0;
		const double BetaStep = (BetaMax - BetaMin) / (1.0 * NumberOfStepsInBeta);

		// Computation
		Intensity = 0.0;

		for (i = 0; i < NumberOfStepsInAlpha; ++i) {
			Alpha = (i + 0.5) * AlphaStep;

			for (j = 0; j < NumberOfStepsInBeta; ++j) {
				Beta = (j + 0.5) * BetaStep;

				// Compute formfactors
				FormfactorOfBelt = (PI * HeightOfBelt * OuterMajorSemiAxis * OuterMinorSemiAxis * FormfactorCylinder(q, OuterMajorSemiAxis, OuterMinorSemiAxis, HeightOfBelt, Alpha, Beta) -
								    PI * HeightOfBelt * MajorSemiAxis * MinorSemiAxis * FormfactorCylinder(q, MajorSemiAxis, MinorSemiAxis, HeightOfBelt, Alpha, Beta)) /
								   (PI * HeightOfBelt * OuterMajorSemiAxis * OuterMinorSemiAxis - PI * HeightOfBelt * MajorSemiAxis * MinorSemiAxis);

				FormfactorOfHeads = (PI * HeightOfLipids * MajorSemiAxis * MinorSemiAxis * FormfactorCylinder(q, MajorSemiAxis, MinorSemiAxis, HeightOfLipids, Alpha, Beta) -
								     PI * HeightOfTails  * MajorSemiAxis * MinorSemiAxis * FormfactorCylinder(q, MajorSemiAxis, MinorSemiAxis, HeightOfTails , Alpha, Beta)) /
								    (PI * HeightOfLipids * MajorSemiAxis * MinorSemiAxis - PI * HeightOfTails * MajorSemiAxis * MinorSemiAxis);

				FormfactorOfCH2Tail = (PI * HeightOfTails * MajorSemiAxis * MinorSemiAxis * FormfactorCylinder(q, MajorSemiAxis, MinorSemiAxis, HeightOfTails, Alpha, Beta) -
								       PI * HeightOfCH3   * MajorSemiAxis * MinorSemiAxis * FormfactorCylinder(q, MajorSemiAxis, MinorSemiAxis, HeightOfCH3  , Alpha, Beta)) /
								      (PI * HeightOfTails * MajorSemiAxis * MinorSemiAxis - PI * HeightOfCH3 * MajorSemiAxis * MinorSemiAxis);

				FormfactorOfCH3Tail = FormfactorCylinder(q, MajorSemiAxis, MinorSemiAxis, HeightOfCH3, Alpha, Beta);

				// Compute amplitudes
				AmplitudeOfBelt    = DeltaRhoBelt * VolumeOfBelt * FormfactorOfBelt;
				AmplitudeOfHeads   = DeltaRhoHead * VolumeOfHeads * FormfactorOfHeads;
				AmplitudeOfCH2Tail = DeltaRhoCH2Tail * VolumeOfCH2Tail * FormfactorOfCH2Tail;
				AmplitudeOfCH3Tail = DeltaRhoCH3Tail * VolumeOfCH3Tail * FormfactorOfCH3Tail;

				// Perform integration
				IntensityDisc = pow(AmplitudeOfBelt + AmplitudeOfHeads + AmplitudeOfCH2Tail + AmplitudeOfCH3Tail, 2);

				// Add the histidine tags
		        FormfactorOfTags = XiRodWithoutEndcaps(q, AverageRadiusOfBelt + RadiusOfGyrationForHisTag, HeightOfBelt, Alpha);

		        Crosscorrelation = 2.0f * pow(DeltaRhoTag * VolumeOfOneHisTag, 2) * pow(FormfactorOfTags, 2) * pow(PsiHammouda(q * RadiusOfGyrationForHisTag), 2);

		        Autocorrelation  = 2.0f * pow(DeltaRhoTag * VolumeOfOneHisTag, 2) * Debye(q, RadiusOfGyrationForHisTag);

		        Disccorrelation  = 4.0f * (AmplitudeOfBelt + AmplitudeOfHeads + AmplitudeOfCH2Tail + AmplitudeOfCH3Tail) *
									      DeltaRhoTag * VolumeOfOneHisTag * FormfactorOfTags * PsiHammouda(q * RadiusOfGyrationForHisTag);

		        IntensityPart = IntensityDisc + Crosscorrelation + Autocorrelation + Disccorrelation;

				// Return the final intensity
				Intensity += 2 / PI * sin(Alpha) * IntensityPart * AlphaStep * BetaStep;
			}
		}
		return Intensity;
	}
