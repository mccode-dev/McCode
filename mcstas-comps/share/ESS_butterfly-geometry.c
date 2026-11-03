/* MCDISPLAY-section for the ESS butterfly moderator */

/* NOTA BENE:
   From McStas 3, this file should be included in %SHARE due to requirement  from clang / macOS, that dislikes function definitons in DISPLAY.
   (clang is more c99-strict than e.g. gcc)

   For this reason, the geometry-calls e.g. line() have to be explicitly the mcdis_line() variant */


/* Define the positioning of the buttefly sketch */

/* Point sets for the butterfly */
double butterfly_z[] = {-1.9922764e-01, -1.8484553e-01, -2.0252845e-01, -2.0795122e-01, -2.1054471e-01, -2.1030894e-01, -2.0889431e-01, -2.0535772e-01, -2.0134959e-01, -1.9639837e-01, -1.9026829e-01, -1.8390244e-01, -1.7565041e-01, -1.7093496e-01, -1.4617886e-01, -1.2873171e-01, -9.2658537e-02, -4.0552845e-02, -1.7682927e-02, -9.1951221e-03, -2.3577231e-03, 5.1869889e-03, 1.1788619e-02, 1.7918699e-02, 2.3105689e-02, 2.4991869e-02, 2.4756099e-02, 2.2162599e-02, 1.8154469e-02, 1.2731709e-02, 5.6585389e-03, -2.3577214e-04, 1.4146339e-02, -2.9707317e-02, 1.4146339e-02, -4.7154514e-04, 1.5325199e-02, 2.1691059e-02, 2.4991869e-02, 2.4991869e-02, 2.2634149e-02, 1.8154469e-02, 1.1552849e-02, 3.3008089e-03, -6.1300811e-03, -9.1951221e-03, -6.0593496e-02, -9.2658537e-02, -1.7541463e-01, -1.8508130e-01, -1.9309756e-01, -1.9899187e-01, -2.0182114e-01, -2.0582927e-01, -2.0913008e-01, -2.1078049e-01, -2.1007317e-01, -2.0630081e-01, -2.0229268e-01, -1.9828455e-01, -1.8484553e-01, -1.9922764e-01, -1.5584553e-01, -1.9922764e-01};
#pragma acc declare create(butterfly_z)

double butterfly_x[] = {1.4279319e-02, 3.1692034e-10, -1.7654432e-02, -2.5962400e-02, -3.4789615e-02, -4.3876452e-02, -5.1665172e-02, -5.8934642e-02, -6.4646372e-02, -6.9059982e-02, -7.2694722e-02, -7.5031332e-02, -7.6589082e-02, -7.6589082e-02, -7.6848702e-02, -7.6589082e-02, -7.6589082e-02, -7.6589082e-02, -7.6589082e-02, -7.6848702e-02, -7.5290962e-02, -7.2694722e-02, -6.8281112e-02, -6.1790512e-02, -5.3482542e-02, -4.4136082e-02, -3.3491495e-02, -2.5443152e-02, -1.9212176e-02, -1.2981200e-02, -6.2309757e-03, -2.5962368e-04, 1.4538943e-02, 5.8415398e-02, 1.0229185e-01, 1.1709042e-01, 1.3266786e-01, 1.4149508e-01, 1.5213966e-01, 1.6200537e-01, 1.7135184e-01, 1.7888093e-01, 1.8589078e-01, 1.9082364e-01, 1.9290063e-01, 1.9341988e-01, 1.9341988e-01, 1.9341988e-01, 1.9341988e-01, 1.9186213e-01, 1.8822740e-01, 1.8459266e-01, 1.8069830e-01, 1.7602507e-01, 1.6849597e-01, 1.5811101e-01, 1.4928380e-01, 1.3993733e-01, 1.3370636e-01, 1.2981200e-01, 1.1709042e-01, 1.0229185e-01, 5.8415398e-02, 1.4279319e-02};
#pragma acc declare create(butterfly_x)

double butterfly_e_z1[]= {-3.0488e-04,  -5.5017e-02, -3.0488e-04};
#pragma acc declare create(butterfly_e_z1)
double butterfly_e_x1[]= {-4.3103e-04,   5.8521e-02,  1.1701e-01};
#pragma acc declare create(butterfly_e_x1)
double butterfly_e_z2[]= {-1.8501e-01, -1.2719e-01, -1.8501e-01};
#pragma acc declare create(butterfly_e_z2)
double butterfly_e_x2[]= {3.3156e-05,  5.8985e-02,  1.1701e-01};
#pragma acc declare create(butterfly_e_x2)

/* BEGIN generated ESS logo data (by ESS_butterfly-logo.py, do not edit by hand):
   ESS emblem from the ess.eu logo (themes/custom/ess/graphics/logo-small.svg), 7 contours
   (holes running opposite to outlines) of 439 points in total, centred and scaled to unit
   diameter with u to the right and v up, plus 437 cap triangles (indices into the points).
   The ESS logo is (c) ESS. All rights reserved. */
int ess_logo_ncontours = 7;
int ess_logo_contours[] = {0, 90, 90, 101, 191, 87, 278, 89, 367, 9, 376, 25, 401, 38};
int ess_logo_npoints = 439;
double ess_logo_u[] = {
  -0.4661, -0.4416, -0.4251, -0.4064, -0.3857, -0.3631, -0.3168, -0.2601, -0.2027, -0.1435, -0.0783, -0.0180,
  0.0366, 0.0882, 0.1399, 0.1864, 0.2242, 0.2560, 0.2811, 0.2524, 0.2198, 0.1836, 0.1487, 0.1072,
  0.0639, 0.0192, -0.0213, -0.0620, -0.1066, -0.1499, -0.1916, -0.2304, -0.2699, -0.3083, -0.3207, -0.3199,
  -0.3148, -0.2853, -0.2478, -0.2103, -0.1744, -0.1365, -0.0925, -0.0471, -0.0009, 0.0445, 0.0887, 0.1312,
  0.1671, 0.2005, 0.2311, 0.2650, 0.2936, 0.2983, 0.3002, 0.2936, 0.2794, 0.2649, 0.2454, 0.2060,
  0.1564, 0.1007, 0.0441, -0.0154, -0.0808, -0.1311, -0.1808, -0.2295, -0.2723, -0.3143, -0.3528, -0.3874,
  -0.4209, -0.4490, -0.4692, -0.4845, -0.4939, -0.4975, -0.4945, -0.4828, -0.4745, -0.4698, -0.4676, -0.4786,
  -0.4860, -0.4894, -0.4901, -0.4881, -0.4835, -0.4761, 0.2010, 0.1701, 0.1431, 0.1144, 0.0839, 0.0519,
  0.0185, -0.0162, -0.0560, -0.0906, -0.1250, -0.1635, -0.1964, -0.2328, -0.2635, -0.2922, -0.3212, -0.3443,
  -0.3678, -0.3884, -0.4037, -0.4182, -0.4280, -0.4351, -0.4398, -0.4408, -0.4384, -0.4324, -0.4241, -0.4125,
  -0.3979, -0.3806, -0.3607, -0.3768, -0.3876, -0.3930, -0.3933, -0.3883, -0.3778, -0.3621, -0.3438, -0.3216,
  -0.2927, -0.2598, -0.2230, -0.1836, -0.1370, -0.0833, -0.0634, -0.0593, -0.0625, -0.1075, -0.1559, -0.1928,
  -0.2276, -0.2599, -0.2925, -0.3209, -0.3448, -0.3634, -0.3766, -0.3843, -0.3863, -0.3839, -0.3770, -0.3605,
  -0.3386, -0.3397, -0.3446, -0.3666, -0.3888, -0.4078, -0.4234, -0.4339, -0.4424, -0.4471, -0.4481, -0.4453,
  -0.4387, -0.4286, -0.4151, -0.3983, -0.3783, -0.3553, -0.3294, -0.3007, -0.2684, -0.2382, -0.2024, -0.1653,
  -0.1272, -0.0885, -0.0543, -0.0150, 0.0245, 0.0579, 0.0942, 0.1327, 0.1645, 0.2042, 0.2045, 0.4178,
  0.3974, 0.3715, 0.3475, 0.3176, 0.2858, 0.2521, 0.2065, 0.2022, 0.2052, 0.2508, 0.2936, 0.3333,
  0.3657, 0.3987, 0.4245, 0.4486, 0.4673, 0.4806, 0.4882, 0.4901, 0.4862, 0.4772, 0.4617, 0.4429,
  0.4169, 0.3789, 0.3379, 0.2925, 0.2398, 0.1838, 0.1299, 0.0750, 0.0158, 0.0604, 0.1039, 0.1456,
  0.1807, 0.2172, 0.2501, 0.2791, 0.3012, 0.3217, 0.3368, 0.3466, 0.3507, 0.3501, 0.3430, 0.3277,
  0.3202, 0.3153, 0.3134, 0.3290, 0.3398, 0.3437, 0.3423, 0.3364, 0.3245, 0.3069, 0.2841, 0.2567,
  0.2251, 0.1897, 0.1553, 0.1189, 0.0808, 0.0315, -0.0189, -0.0213, -0.0218, -0.0000, 0.0348, 0.0651,
  0.1006, 0.1613, 0.2243, 0.2835, 0.3344, 0.3836, 0.4247, 0.4572, 0.4791, 0.4935, 0.4975, 0.4917,
  0.4764, 0.4517, 0.1966, 0.2118, 0.2238, 0.2324, 0.2381, 0.2988, 0.2959, 0.2907, 0.2830, 0.2726,
  0.2449, 0.2268, 0.2067, 0.1662, 0.1348, 0.1098, 0.0919, 0.0819, 0.0703, 0.0431, 0.0244, 0.0020,
  -0.0367, -0.0607, -0.0788, -0.0969, -0.1074, -0.1221, -0.1360, -0.1521, -0.1704, -0.2081, -0.2297, -0.2494,
  -0.2763, -0.2964, -0.3176, -0.3316, -0.3415, -0.3482, -0.3491, -0.3456, -0.3399, -0.3316, -0.3208, -0.3074,
  -0.2926, -0.2763, -0.2587, -0.2397, -0.1993, -0.1675, -0.1350, -0.1090, -0.0947, -0.0777, -0.0529, -0.0267,
  0.0136, 0.0476, 0.0752, 0.0947, 0.1064, 0.1196, 0.1496, 0.1697, 0.1935, 0.2317, 0.2636, 0.2797,
  0.2907, 0.2989, 0.3042, 0.3067, 0.3062, 0.3030, 0.2950, 0.2865, 0.2721, 0.2405, 0.1731, 0.1605,
  0.1524, 0.1494, 0.1477, 0.1488, 0.1544, 0.1646, 0.1737, -0.2085, -0.1892, -0.1688, -0.1534, -0.1437,
  -0.2810, -0.2704, -0.2542, -0.2332, -0.0096, -0.0256, -0.0420, -0.0522, -0.0552, -0.1574, -0.1701, -0.1864,
  -0.2076, -0.2286, -0.2466, -0.2651, -0.2778, -0.2836, -0.0814, -0.0794, -0.0465, 0.0187, 0.0299, 0.0371,
  0.0407, 0.0411, 0.0343, 0.0236, 0.0140, 0.1787, 0.2106, 0.2266, 0.2365, 0.2377, 0.2309, 0.2157,
  0.1930, 0.1664, 0.1582, 0.1487, 0.1430, 0.1414, 0.1101, 0.1082, 0.1019, 0.0946, 0.0819, 0.0662,
  0.0481, -0.0123, -0.0362, -0.0473, -0.0489, -0.0478, -0.0393, -0.0229, -0.0000, 0.0196, 0.0333, 0.0394,
  0.0414, 0.0829, 0.0850, 0.0909, 0.1007, 0.1142, 0.1415
};
double ess_logo_v[] = {
  0.2722, 0.3212, 0.3459, 0.3690, 0.3904, 0.4101, 0.4413, 0.4676, 0.4841, 0.4920, 0.4910, 0.4812,
  0.4648, 0.4420, 0.4105, 0.3724, 0.3318, 0.2869, 0.2386, 0.2737, 0.3051, 0.3323, 0.3527, 0.3709,
  0.3838, 0.3915, 0.3937, 0.3915, 0.3839, 0.3709, 0.3526, 0.3294, 0.2984, 0.2586, 0.2428, 0.2376,
  0.2384, 0.2733, 0.3075, 0.3336, 0.3528, 0.3678, 0.3794, 0.3854, 0.3857, 0.3803, 0.3694, 0.3530,
  0.3340, 0.3112, 0.2849, 0.2476, 0.2059, 0.2044, 0.2063, 0.2276, 0.2597, 0.2863, 0.3161, 0.3634,
  0.4075, 0.4436, 0.4698, 0.4882, 0.4987, 0.5000, 0.4955, 0.4852, 0.4708, 0.4511, 0.4270, 0.3989,
  0.3633, 0.3233, 0.2839, 0.2414, 0.1974, 0.1527, 0.1027, 0.0475, 0.0226, 0.0204, 0.0252, 0.0601,
  0.0951, 0.1252, 0.1552, 0.1850, 0.2145, 0.2436, -0.3658, -0.3977, -0.4212, -0.4415, -0.4585, -0.4722,
  -0.4826, -0.4894, -0.4927, -0.4916, -0.4870, -0.4774, -0.4652, -0.4471, -0.4277, -0.4054, -0.3781, -0.3521,
  -0.3199, -0.2855, -0.2537, -0.2156, -0.1809, -0.1452, -0.1043, -0.0689, -0.0288, 0.0109, 0.0451, 0.0798,
  0.1131, 0.1448, 0.1745, 0.1321, 0.0882, 0.0432, 0.0029, -0.0422, -0.0862, -0.1287, -0.1649, -0.1990,
  -0.2339, -0.2649, -0.2918, -0.3138, -0.3326, -0.3459, -0.3487, -0.3455, -0.3414, -0.3333, -0.3179, -0.3009,
  -0.2799, -0.2551, -0.2232, -0.1873, -0.1478, -0.1060, -0.0624, -0.0175, 0.0230, 0.0634, 0.1032, 0.1561,
  0.2017, 0.2065, 0.2060, 0.1790, 0.1455, 0.1096, 0.0716, 0.0372, -0.0027, -0.0432, -0.0842, -0.1255,
  -0.1669, -0.2070, -0.2456, -0.2825, -0.3173, -0.3500, -0.3803, -0.4079, -0.4333, -0.4526, -0.4707, -0.4846,
  -0.4941, -0.4992, -0.5000, -0.4966, -0.4886, -0.4779, -0.4616, -0.4382, -0.4130, -0.3718, -0.3676, 0.2936,
  0.3159, 0.3394, 0.3573, 0.3754, 0.3904, 0.4024, 0.4131, 0.4101, 0.4059, 0.3951, 0.3790, 0.3577,
  0.3346, 0.3040, 0.2731, 0.2354, 0.1950, 0.1524, 0.1079, 0.0620, 0.0151, -0.0291, -0.0763, -0.1171,
  -0.1601, -0.2078, -0.2472, -0.2811, -0.3107, -0.3326, -0.3457, -0.3514, -0.3492, -0.3420, -0.3294, -0.3116,
  -0.2916, -0.2648, -0.2337, -0.1988, -0.1649, -0.1239, -0.0813, -0.0372, 0.0031, 0.0483, 0.0981, 0.1512,
  0.1699, 0.1718, 0.1669, 0.1239, 0.0743, 0.0288, -0.0119, -0.0522, -0.0961, -0.1384, -0.1786, -0.2152,
  -0.2480, -0.2766, -0.2982, -0.3157, -0.3291, -0.3398, -0.3437, -0.3447, -0.3496, -0.3546, -0.3583, -0.3591,
  -0.3571, -0.3466, -0.3257, -0.2955, -0.2596, -0.2135, -0.1617, -0.1051, -0.0495, 0.0139, 0.0754, 0.1350,
  0.1918, 0.2450, 0.1080, 0.1052, 0.0993, 0.0908, 0.0760, 0.0760, 0.0948, 0.1076, 0.1192, 0.1297,
  0.1462, 0.1520, 0.1552, 0.1544, 0.1463, 0.1317, 0.1114, 0.1240, 0.1341, 0.1482, 0.1532, 0.1558,
  0.1534, 0.1468, 0.1376, 0.1221, 0.1062, 0.1221, 0.1333, 0.1427, 0.1498, 0.1559, 0.1553, 0.1521,
  0.1428, 0.1307, 0.1110, 0.0917, 0.0703, 0.0418, 0.0105, -0.0143, -0.0326, -0.0494, -0.0649, -0.0789,
  -0.0908, -0.1002, -0.1070, -0.1114, -0.1127, -0.1061, -0.0900, -0.0664, -0.0832, -0.0955, -0.1063, -0.1119,
  -0.1127, -0.1061, -0.0932, -0.0759, -0.0871, -0.0960, -0.1080, -0.1119, -0.1134, -0.1096, -0.0981, -0.0875,
  -0.0767, -0.0648, -0.0517, -0.0375, -0.0228, -0.0097, 0.0053, 0.0147, 0.0251, 0.0384, 0.0544, 0.0597,
  0.0659, 0.0709, 0.0799, 0.0872, 0.0965, 0.1034, 0.1064, 0.1043, 0.1004, 0.0892, 0.0725, 0.0520,
  0.0520, 0.0736, 0.0907, 0.1015, -0.0643, -0.0617, -0.0536, -0.0411, -0.0294, -0.0294, -0.0465, -0.0566,
  -0.0616, -0.0603, -0.0540, -0.0403, -0.0217, 0.0000, 0.0000, 0.0203, 0.0060, -0.0103, -0.0154, -0.0214,
  -0.0288, -0.0385, -0.0521, -0.0594, -0.0626, -0.0008, -0.0087, -0.0154, -0.0261, -0.0385, -0.0521, -0.0612,
  -0.0645, -0.0601, -0.0560, -0.0478, -0.0374, -0.0294, -0.0294, -0.0157, -0.0001, 0.0098, 0.0210, 0.0300,
  0.0370, 0.0511, 0.0597, 0.0709, 0.0798, 0.0872, 0.0991, 0.1064, 0.1080, 0.1035, 0.0939, 0.0838,
  0.0760, 0.0760, 0.0603, 0.0460, 0.0333, 0.0222, 0.0088
};
int ess_logo_ntriangles = 437;
int ess_logo_triangles[] = {
  18, 19, 20, 18, 20, 21, 21, 22, 23, 21, 23, 24, 24, 25, 26, 24, 26, 27,
  27, 28, 29, 27, 29, 30, 30, 31, 32, 32, 33, 34, 32, 34, 35, 32, 35, 36,
  32, 36, 37, 32, 37, 38, 30, 32, 38, 30, 38, 39, 30, 39, 40, 30, 40, 41,
  27, 30, 41, 27, 41, 42, 27, 42, 43, 27, 43, 44, 24, 27, 44, 24, 44, 45,
  24, 45, 46, 24, 46, 47, 21, 24, 47, 21, 47, 48, 21, 48, 49, 21, 49, 50,
  18, 21, 50, 18, 50, 51, 18, 51, 52, 18, 52, 53, 18, 53, 54, 17, 18, 54,
  17, 54, 55, 17, 55, 56, 16, 17, 56, 16, 56, 57, 16, 57, 58, 15, 16, 58,
  15, 58, 59, 14, 15, 59, 14, 59, 60, 13, 14, 60, 13, 60, 61, 12, 13, 61,
  12, 61, 62, 11, 12, 62, 11, 62, 63, 10, 11, 63, 10, 63, 64, 9, 10, 64,
  9, 64, 65, 8, 9, 65, 8, 65, 66, 7, 8, 66, 7, 66, 67, 6, 7, 67,
  6, 67, 68, 6, 68, 69, 5, 6, 69, 5, 69, 70, 4, 5, 70, 3, 4, 70,
  3, 70, 71, 2, 3, 71, 2, 71, 72, 1, 2, 72, 0, 1, 72, 0, 72, 73,
  89, 0, 73, 89, 73, 74, 89, 74, 75, 89, 75, 76, 76, 77, 78, 78, 79, 80,
  78, 80, 81, 78, 81, 82, 78, 82, 83, 78, 83, 84, 78, 84, 85, 76, 78, 85,
  76, 85, 86, 76, 86, 87, 76, 87, 88, 76, 88, 89, 190, 90, 91, 122, 123, 124,
  122, 124, 125, 125, 126, 127, 125, 127, 128, 128, 129, 130, 128, 130, 131, 131, 132, 133,
  131, 133, 134, 134, 135, 136, 136, 137, 138, 136, 138, 139, 136, 139, 140, 136, 140, 141,
  136, 141, 142, 134, 136, 142, 134, 142, 143, 134, 143, 144, 134, 144, 145, 131, 134, 145,
  131, 145, 146, 131, 146, 147, 131, 147, 148, 128, 131, 148, 128, 148, 149, 128, 149, 150,
  128, 150, 151, 125, 128, 151, 125, 151, 152, 125, 152, 153, 125, 153, 154, 122, 125, 154,
  122, 154, 155, 122, 155, 156, 122, 156, 157, 121, 122, 157, 121, 157, 158, 121, 158, 159,
  120, 121, 159, 120, 159, 160, 119, 120, 160, 119, 160, 161, 118, 119, 161, 118, 161, 162,
  117, 118, 162, 116, 117, 162, 116, 162, 163, 116, 163, 164, 115, 116, 164, 115, 164, 165,
  114, 115, 165, 113, 114, 165, 113, 165, 166, 112, 113, 166, 112, 166, 167, 111, 112, 167,
  111, 167, 168, 110, 111, 168, 110, 168, 169, 109, 110, 169, 109, 169, 170, 108, 109, 170,
  108, 170, 171, 107, 108, 171, 107, 171, 172, 106, 107, 172, 106, 172, 173, 105, 106, 173,
  105, 173, 174, 104, 105, 174, 104, 174, 175, 103, 104, 175, 103, 175, 176, 102, 103, 176,
  102, 176, 177, 101, 102, 177, 101, 177, 178, 100, 101, 178, 100, 178, 179, 99, 100, 179,
  99, 179, 180, 98, 99, 180, 98, 180, 181, 97, 98, 181, 97, 181, 182, 96, 97, 182,
  96, 182, 183, 95, 96, 183, 95, 183, 184, 94, 95, 184, 94, 184, 185, 93, 94, 185,
  93, 185, 186, 92, 93, 186, 92, 186, 187, 91, 92, 187, 190, 91, 187, 190, 187, 188,
  188, 189, 190, 277, 191, 192, 277, 192, 193, 193, 194, 195, 193, 195, 196, 193, 196, 197,
  197, 198, 199, 197, 199, 200, 197, 200, 201, 197, 201, 202, 193, 197, 202, 193, 202, 203,
  193, 203, 204, 193, 204, 205, 277, 193, 205, 277, 205, 206, 277, 206, 207, 277, 207, 208,
  224, 225, 226, 224, 226, 227, 227, 228, 229, 227, 229, 230, 230, 231, 232, 230, 232, 233,
  233, 234, 235, 233, 235, 236, 236, 237, 238, 238, 239, 240, 238, 240, 241, 238, 241, 242,
  238, 242, 243, 238, 243, 244, 236, 238, 244, 236, 244, 245, 236, 245, 246, 236, 246, 247,
  233, 236, 247, 233, 247, 248, 233, 248, 249, 233, 249, 250, 230, 233, 250, 230, 250, 251,
  230, 251, 252, 230, 252, 253, 227, 230, 253, 227, 253, 254, 227, 254, 255, 227, 255, 256,
  224, 227, 256, 224, 256, 257, 224, 257, 258, 224, 258, 259, 224, 259, 260, 223, 224, 260,
  223, 260, 261, 223, 261, 262, 222, 223, 262, 222, 262, 263, 222, 263, 264, 221, 222, 264,
  221, 264, 265, 220, 221, 265, 220, 265, 266, 219, 220, 266, 219, 266, 267, 218, 219, 267,
  218, 267, 268, 217, 218, 268, 217, 268, 269, 216, 217, 269, 216, 269, 270, 215, 216, 270,
  214, 215, 270, 214, 270, 271, 213, 214, 271, 213, 271, 272, 212, 213, 272, 212, 272, 273,
  211, 212, 273, 210, 211, 273, 210, 273, 274, 209, 210, 274, 209, 274, 275, 208, 209, 275,
  277, 208, 275, 275, 276, 277, 281, 282, 283, 280, 281, 283, 279, 280, 283, 279, 283, 284,
  278, 279, 284, 278, 284, 285, 278, 285, 286, 366, 278, 286, 366, 286, 287, 366, 287, 288,
  366, 288, 289, 366, 289, 290, 366, 290, 291, 366, 291, 292, 366, 292, 293, 366, 293, 294,
  294, 295, 296, 294, 296, 297, 294, 297, 298, 294, 298, 299, 294, 299, 300, 294, 300, 301,
  294, 301, 302, 294, 302, 303, 366, 294, 303, 370, 371, 304, 369, 370, 304, 368, 369, 304,
  367, 368, 304, 367, 304, 305, 375, 367, 305, 375, 305, 306, 375, 306, 307, 375, 307, 308,
  374, 375, 308, 374, 308, 309, 373, 374, 309, 373, 309, 310, 373, 310, 311, 372, 373, 311,
  372, 311, 312, 372, 312, 313, 372, 313, 314, 372, 314, 315, 372, 315, 316, 372, 316, 317,
  371, 372, 317, 371, 317, 318, 371, 318, 319, 319, 320, 321, 319, 321, 322, 319, 322, 323,
  319, 323, 324, 319, 324, 325, 319, 325, 326, 319, 326, 327, 319, 327, 328, 319, 328, 329,
  329, 330, 331, 331, 332, 333, 331, 333, 334, 331, 334, 335, 331, 335, 336, 331, 336, 337,
  331, 337, 338, 338, 397, 398, 338, 398, 399, 338, 399, 400, 338, 400, 376, 331, 338, 376,
  331, 376, 377, 331, 377, 378, 331, 378, 379, 331, 379, 380, 329, 331, 380, 329, 380, 381,
  329, 381, 382, 329, 382, 383, 329, 383, 384, 329, 384, 385, 329, 385, 386, 319, 329, 386,
  319, 386, 387, 319, 387, 388, 319, 388, 389, 371, 319, 389, 371, 389, 390, 304, 371, 390,
  303, 304, 390, 303, 390, 391, 303, 391, 392, 303, 392, 393, 303, 393, 394, 303, 394, 395,
  396, 397, 338, 395, 396, 338, 395, 338, 339, 303, 395, 339, 339, 340, 341, 339, 341, 342,
  339, 342, 343, 339, 343, 344, 339, 344, 345, 339, 345, 346, 339, 346, 347, 347, 405, 406,
  347, 406, 407, 347, 407, 408, 339, 347, 408, 339, 408, 409, 339, 409, 410, 339, 410, 411,
  339, 411, 412, 339, 412, 413, 339, 413, 414, 339, 414, 415, 339, 415, 416, 339, 416, 417,
  339, 417, 418, 339, 418, 419, 339, 419, 420, 339, 420, 421, 339, 421, 422, 339, 422, 423,
  303, 339, 423, 303, 423, 424, 303, 424, 425, 303, 425, 426, 303, 426, 427, 303, 427, 428,
  366, 303, 428, 366, 428, 429, 366, 429, 430, 366, 430, 431, 366, 431, 432, 433, 434, 435,
  433, 435, 436, 433, 436, 437, 433, 437, 438, 433, 438, 401, 433, 401, 402, 433, 402, 403,
  404, 405, 347, 404, 347, 348, 403, 404, 348, 403, 348, 349, 403, 349, 350, 433, 403, 350,
  433, 350, 351, 433, 351, 352, 433, 352, 353, 433, 353, 354, 433, 354, 355, 433, 355, 356,
  356, 357, 358, 433, 356, 358, 433, 358, 359, 433, 359, 360, 433, 360, 361, 433, 361, 362,
  432, 433, 362, 432, 362, 363, 432, 363, 364, 432, 364, 365, 432, 365, 366
};
/* END generated ESS logo data */

/* Solid (--trace=2 only) vertical slab from y=ybot to y=ytop over the n points (px[i], pz[i]),
   as one polyhedron of triangles: two side-wall triangles per edge of the ncont closed contours,
   given in cont[] as (first point, number of points) pairs (holes running opposite to outlines),
   and the ntri cap triangles tri[] (point indices, counter-clockwise seen from above) at the
   top and, reversed, at the bottom */
void butterfly_solid(int n, double *px, double *pz, double ytop, double ybot, int ncont, int *cont, int ntri, int *tri)
{
  int i, j, k, c;
  char *json, *p;
  if (mcdotrace!=2) return;
  /* bounds per vertex / face as in mcdis_polygon */
  json = malloc(64 + (size_t)2*n*48 + (size_t)(2*n+2*ntri)*64);
  if (!json) {
    fprintf(stderr, "Error allocating butterfly_solid polyhedron\n");
    return;
  }
  p = json + sprintf(json, "{ \"vertices\": [");
  /* vertex i at the top, vertex n+i at the bottom */
  for (i=0; i<2*n; i++)
    p += sprintf(p, "%s[%g, %g, %g]", i ? ", " : "", px[i%n], i<n ? ytop : ybot, pz[i%n]);
  p += sprintf(p, "], \"faces\": [");
  for (c=0; c<ncont; c++) {
    for (j=0; j<cont[2*c+1]; j++) {
      i = cont[2*c] + j; k = cont[2*c] + (j+1)%cont[2*c+1];
      p += sprintf(p, "%s{ \"face\": [%d, %d, %d]}, { \"face\": [%d, %d, %d]}", (c || j) ? ", " : "", i, n+k, k, i, n+i, n+k);
    }
  }
  for (j=0; j<ntri; j++)
    p += sprintf(p, ", { \"face\": [%d, %d, %d]}, { \"face\": [%d, %d, %d]}",
                 tri[3*j], tri[3*j+1], tri[3*j+2], n+tri[3*j], n+tri[3*j+2], n+tri[3*j+1]);
  sprintf(p, "]}");
  mcdis_polyhedron(json);
  free(json);
}

/* Solid (--trace=2 only) vertical prism from y=-h to y=h over the closed ring of the n (at most 64)
   points (px[ring[i]], pz[ring[i]]), with top / bottom faces as fans over nsub convex sub-polygons,
   each given in sub[] as (first ring index, number of vertices), wrapping around the ring */
void butterfly_prism(int n, int *ring, double *px, double *pz, double h, int nsub, int *sub)
{
  double rx[64], rz[64];
  int tri[3*64], cont[2], ntri=0, i, k, s;
  if (n>64) return;
  for (i=0; i<n; i++) {
    rx[i] = px[ring[i]]; rz[i] = pz[ring[i]];
  }
  for (s=0; s<nsub; s++) {
    for (k=1; k<sub[2*s+1]-1 && ntri<64; k++) {
      tri[3*ntri] = sub[2*s]; tri[3*ntri+1] = (sub[2*s]+k)%n; tri[3*ntri+2] = (sub[2*s]+k+1)%n;
      ntri++;
    }
  }
  cont[0] = 0; cont[1] = n;
  butterfly_solid(n, rx, rz, h, -h, 1, cont, ntri, tri);
}

/* ESS emblem as a slab of diameter D and thickness t centred at (x,y,z), lying in the horizontal
   plane with the emblem's "up" along (nx,0,nz): a solid at --trace=2, outlines otherwise */
void butterfly_ess_logo(double x, double y, double z, double D, double t, double nx, double nz)
{
  int i, c, j, k;
  double *lx = malloc(ess_logo_npoints*sizeof(double)), *lz = malloc(ess_logo_npoints*sizeof(double));
  if (!lx || !lz) {
    free(lx); free(lz);
    return;
  }
  /* emblem "right" is (-nz,0,nx), so that it reads correctly seen from above */
  for (i=0; i<ess_logo_npoints; i++) {
    lx[i] = x + D*(-ess_logo_u[i]*nz + ess_logo_v[i]*nx);
    lz[i] = z + D*( ess_logo_u[i]*nx + ess_logo_v[i]*nz);
  }
  if (mcdotrace==2) {
    butterfly_solid(ess_logo_npoints, lx, lz, y+t/2, y-t/2, ess_logo_ncontours, ess_logo_contours,
                    ess_logo_ntriangles, ess_logo_triangles);
  } else {
    for (c=0; c<ess_logo_ncontours; c++) {
      for (j=0; j<ess_logo_contours[2*c+1]; j++) {
        i = ess_logo_contours[2*c] + j; k = ess_logo_contours[2*c] + (j+1)%ess_logo_contours[2*c+1];
        mcdis_line(lx[i], y, lz[i], lx[k], y, lz[k]);
      }
    }
  }
  free(lx); free(lz);
}

void butterfly_geometry(double Bdelta_y, int Bjmax, double Bcx, double Bcz,
  double Borientation_angle, double *BBeamlines, double Btx, double Bty, double Btz,
  double BrC1_x, double BrC1_z, 
  double BrC2_x, double BrC2_z, 
  double BrC3_x, double BrC3_z,  
  double BrT1_x, double BrT1_z, 
  double BrT2_x, double BrT2_z, 
  double BrT3_x, double BrT3_z,
  double Br11, double Br12, double Br21, double Br22,
  double Bfoc_XW, double Bfoc_YH, int Blogo)
{
  double y0;
  int j;
  double rAx,rAz,rBx,rBz;

  /* Arrow indicating proton beam direction, in dark grey and in the style of Arm: a thin
     cylinder from 6 m upstream and a cone with its tip 13.7 cm below the moderator centre */
  double ax,az,bx,bz,bbx,bbz,cBcx,cBcz;
  double ay=-0.137, anx, anz, al;
  az = -0.0925-Bcz;
  ax = 0.0585-Bcx;
  bz = -0.0925-Bcz;
  bx = 0.0585+6-Bcx;
  /* rAx,0,rAz is the centre of the moderator */
  rAx = Br11*(az) + Br12*(ax);
  rAz = Br21*(az) + Br22*(ax);
  rBx = Br11*(bz) + Br12*(bx);
  rBz = Br21*(bz) + Br22*(bx);
  /* unit vector along the proton beam, from upstream towards the target */
  anx = rAx-rBx; anz = rAz-rBz; al = sqrt(anx*anx+anz*anz);
  anx /= al; anz /= al;
  mcdis_pencolor("#a9a9a9");
  mcdis_cylinder((rBx+rAx-0.1*anx)/2, ay, (rBz+rAz-0.1*anz)/2, 0.01, al-0.1, 0, anx, 0, anz);
  mcdis_cone(rAx-0.05*anx, ay, rAz-0.05*anz, 0.03, 0.1, anx, 0, anz);
  mcdis_pencolor("#0099dc"); /* back to ESS blue, as set in ESS_butterfly MCDISPLAY */

  /* ESS emblem in place of the target wheel (2.6 m diameter, 8 cm tungsten bricks): at the
     proton-beam level, centred downstream of the beam impact point such that the arrowhead
     sits inside the nearest swoosh, whose inner edge is at most 1.258 m from the emblem centre
     within the 3 cm cone radius: 1.258 - 0.02 margin - 0.1 cone length = 1.138 m */
  if (Blogo)
    butterfly_ess_logo(rAx+1.138*anx, ay, rAz+1.138*anz, 2.6, 0.08, anx, anz);

  /* 120 degree "end of sector" lines */
  bbz = 2 * cos(DEG2RAD*61);
  bbx = 2 * sin(DEG2RAD*61);
  cBcz = 2 * cos(-DEG2RAD*61);
  cBcx = 2 * sin(-DEG2RAD*61);
  rBx = Br11*(bbz) + Br12*(bbx);
  rBz = Br21*(bbz) + Br22*(bbx);
  mcdis_dashed_line(rAx, 0, rAz, rBx+rAx, 0, rBz+rAz,51);
  rBx = Br11*(cBcz) + Br12*(cBcx);
  rBz = Br21*(cBcz) + Br22*(cBcx);
  mcdis_dashed_line(rAx, 0, rAz, rBx+rAx, 0, rBz+rAz,51);
  bbz = 2 * cos(DEG2RAD*119);
  bbx = 2 * sin(DEG2RAD*119);
  cBcz = 2 * cos(-DEG2RAD*119);
  cBcx = 2 * sin(-DEG2RAD*119);
  rBx = Br11*(bbz) + Br12*(bbx);
  rBz = Br21*(bbz) + Br22*(bbx);
  mcdis_dashed_line(rAx, 0, rAz, rBx+rAx, 0, rBz+rAz,51);
  rBx = Br11*(cBcz) + Br12*(cBcx);
  rBz = Br21*(cBcz) + Br22*(cBcx);
  mcdis_dashed_line(rAx, 0, rAz, rBx+rAx, 0, rBz+rAz,51);
  /* Open vertical cylinders (no lids, a ring of rectangles) indicating the extent of
     the "empty" zone where optics is not allowed (r=2 m, h=0.2 m) and of the target
     monolith (r=5.5 m, h=2 m) */
  double cyl_r[2]={2.0, 5.5}, cyl_halfh[2]={0.1, 1.0};
  double px1, pz1, px2, pz2, py;
  int k;
  for (j=0; j<2; j++) {
    py = cyl_halfh[j];
    for (k=0; k<36; k++) {
      px1 = rAx + cyl_r[j]*cos(2*PI*k/36);     pz1 = rAz + cyl_r[j]*sin(2*PI*k/36);
      px2 = rAx + cyl_r[j]*cos(2*PI*(k+1)/36); pz2 = rAz + cyl_r[j]*sin(2*PI*(k+1)/36);
      mcdis_polygon(4, px1, -py, pz1, px2, -py, pz2, px2, py, pz2, px1, py, pz1);
    }
  }

  /* Beamport "plug" dimensions */
  double w1=0.206/2.0, w2=0.276/2.0, l1=2.0+rAz, l2=2.0+rAz+1.75, l3=2.0+rAz+3.5;
  mcdis_line(w1, 0, l1, w1, 0, l2);
  mcdis_line(-w1, 0, l1, -w1, 0, l2);
  mcdis_line(w1, 0, l2, w2, 0, l2);
  mcdis_line(-w1, 0, l2, -w2, 0, l2);
  mcdis_line(w2, 0, l2, w2, 0, l3);
  mcdis_line(-w2, 0, l2, -w2, 0, l3);

  /* Draw all the BBeamlines in "this sector" +1 */
  double xx1, yy1, zz1, xx2, yy2, zz2, delta_omega, b11, b12, b21, b22;
  for (j=0; j<Bjmax+1; j++) {
    delta_omega = Borientation_angle - BBeamlines[j];
    b11 = cos(DEG2RAD*delta_omega);
    b12 = -sin(DEG2RAD*delta_omega);
    b21 = sin(DEG2RAD*delta_omega);
    b22 = cos(DEG2RAD*delta_omega);
    xx1 = b11*(w1) + b12*(l1);
    zz1 = b21*(w1) + b22*(l1);
    xx2 = b11*(w1) + b12*(l2);
    zz2 = b21*(w1) + b22*(l2);
    mcdis_dashed_line(xx1, 0, zz1, xx2, 0, zz2, 11);
    xx1 = b11*(-w1) + b12*(l1);
    zz1 = b21*(-w1) + b22*(l1);
    xx2 = b11*(-w1) + b12*(l2);
    zz2 = b21*(-w1) + b22*(l2);
    mcdis_dashed_line(xx1, 0, zz1, xx2, 0, zz2, 11);
    xx1 = b11*(w2) + b12*(l2);
    zz1 = b21*(w2) + b22*(l2);
    xx2 = b11*(w2) + b12*(l3);
    zz2 = b21*(w2) + b22*(l3);
    mcdis_dashed_line(xx1, 0, zz1, xx2, 0, zz2, 11);
    xx1 = b11*(-w2) + b12*(l2);
    zz1 = b21*(-w2) + b22*(l2);
    xx2 = b11*(-w2) + b12*(l3);
    zz2 = b21*(-w2) + b22*(l3);
    mcdis_dashed_line(xx1, 0, zz1, xx2, 0, zz2, 11);
  }

  /* Show instrument axis... */
  mcdis_dashed_line(0,0,0,0,0,2+rAz,21);

  /* Draw up the "focusing rectangle" */ 
  /* Horizontal direction vector @ focusing area */
  vec_prod(xx1,yy1,zz1,Btx,Bty,Btz,0.0,1.0,0.0);
  NORM(xx1,yy1,zz1);
  vec_prod(xx2,yy2,zz2,Btx,Bty,Btz,xx1,yy1,zz1);
  NORM(xx2,yy2,zz2);
  xx1*=Bfoc_XW/2.0; yy1*=Bfoc_XW/2.0; zz1*=Bfoc_XW/2.0;
  xx2*=Bfoc_YH/2.0; yy2*=Bfoc_YH/2.0; zz2*=Bfoc_YH/2.0;
  printf("Normal vectors pointing in directions\n %g %g %g and \n %g %g %g \n",xx1,yy1,zz1,xx2,yy2,zz2);
  mcdis_dashed_line(Btx -xx1 -xx2, Bty -yy1 -yy2, Btz -zz1 -zz2,
	      Btx +xx1 -xx2, Bty +yy1 -yy2, Btz +zz1 -zz2,5);
  mcdis_dashed_line(Btx -xx1 +xx2, Bty -yy1 +yy2, Btz -zz1 +zz2,
	      Btx +xx1 +xx2, Bty +yy1 +yy2, Btz +zz1 +zz2,5);

  mcdis_dashed_line(Btx -xx1 -xx2, Bty -yy1 -yy2, Btz -zz1 -zz2,
	      Btx -xx1 +xx2, Bty -yy1 +yy2, Btz -zz1 +zz2,5);
  mcdis_dashed_line(Btx +xx1 -xx2, Bty +yy1 -yy2, Btz +zz1 -zz2,
	      Btx +xx1 +xx2, Bty +yy1 +yy2, Btz +zz1 +zz2,5);

  /* Draw the two butterfly shapes at top and bottom level, last so they stay on top:
     first the cold wings (pass 0), then the thermal "boxes" between them (pass 1),
     i.e. outline segments 31-35 and 60-63,0-1 */
  int pass, thermal;
  /* Rotated outline points 0-63, plus the tips of the two thermal/cold "border" Vs as 64 and 65 */
  double ox[66], oz[66];
  for (j=0; j<66; j++) {
    rBz = j<64 ? butterfly_z[j] : (j==64 ? butterfly_e_z1[1] : butterfly_e_z2[1]);
    rBx = j<64 ? butterfly_x[j] : (j==64 ? butterfly_e_x1[1] : butterfly_e_x2[1]);
    ox[j] = Br11*(rBz-Bcz) + Br12*(rBx-Bcx);
    oz[j] = Br21*(rBz-Bcz) + Br22*(rBx-Bcx);
  }
  /* Solid cold wings: ring 1-31, 64, 35-60, 65, with the caps split at the border Vs into
     two convex halves; solid thermal boxes: two chevrons, each as two convex quads */
  int cold_ring[59], cold_sub[]={58, 33, 31, 28}, therm_sub[]={5, 4, 2, 4};
  int therm_top[]={31, 32, 33, 34, 35, 64}, therm_bot[]={60, 61, 62, 0, 1, 65};
  for (j=0; j<31; j++) cold_ring[j] = j+1;
  cold_ring[31] = 64;
  for (j=32; j<58; j++) cold_ring[j] = j+3;
  cold_ring[58] = 65;
  for (pass=0; pass<2; pass++) {
    mcdis_pencolor(pass ? "#ff0000" : "#66ccff");
    if (pass) {
      butterfly_prism(6, therm_top, ox, oz, Bdelta_y, 2, therm_sub);
      butterfly_prism(6, therm_bot, ox, oz, Bdelta_y, 2, therm_sub);
    } else {
      butterfly_prism(59, cold_ring, ox, oz, Bdelta_y, 2, cold_sub);
    }
    for (y0=-Bdelta_y; y0<2*Bdelta_y; y0+=2*Bdelta_y) {
      for (j=0; j<63; j++) {
        thermal = (j==0 || (j>=31 && j<35) || j>=60);
        if (thermal != pass) continue;

        rAx = Br11*(butterfly_z[j]-Bcz) + Br12*(butterfly_x[j]-Bcx);
        rAz = Br21*(butterfly_z[j]-Bcz) + Br22*(butterfly_x[j]-Bcx);

        rBx = Br11*(butterfly_z[j+1]-Bcz) + Br12*(butterfly_x[j+1]-Bcx);
        rBz = Br21*(butterfly_z[j+1]-Bcz) + Br22*(butterfly_x[j+1]-Bcx);

        mcdis_line(rAx, y0, rAz, rBx, y0, rBz);
      }
    }
  }

  /* Draw the "border" between the thermal and cold areas, closing the thermal outline */
  for (y0=-Bdelta_y; y0<2*Bdelta_y; y0+=2*Bdelta_y) {
    for (j=0; j<2; j++) {
      
      rAx = Br11*(butterfly_e_z1[j]-Bcz) + Br12*(butterfly_e_x1[j]-Bcx);
      rAz = Br21*(butterfly_e_z1[j]-Bcz) + Br22*(butterfly_e_x1[j]-Bcx);

      rBx = Br11*(butterfly_e_z1[j+1]-Bcz) + Br12*(butterfly_e_x1[j+1]-Bcx);
      rBz = Br21*(butterfly_e_z1[j+1]-Bcz) + Br22*(butterfly_e_x1[j+1]-Bcx);

      mcdis_line(rAx, y0, rAz, rBx, y0, rBz);

      rAx = Br11*(butterfly_e_z2[j]-Bcz) + Br12*(butterfly_e_x2[j]-Bcx);
      rAz = Br21*(butterfly_e_z2[j]-Bcz) + Br22*(butterfly_e_x2[j]-Bcx);

      rBx = Br11*(butterfly_e_z2[j+1]-Bcz) + Br12*(butterfly_e_x2[j+1]-Bcx);
      rBz = Br21*(butterfly_e_z2[j+1]-Bcz) + Br22*(butterfly_e_x2[j+1]-Bcx);

      mcdis_line(rAx, y0, rAz, rBx, y0, rBz);
    }
  }

  /* Indicate the emission planes of cold/thermal moderator, in yellow */
  mcdis_pencolor("#ffff00");
  for (y0=-Bdelta_y; y0<2*Bdelta_y; y0+=2*Bdelta_y) {
    mcdis_dashed_line(BrC1_x, y0, BrC1_z, BrC2_x, y0, BrC2_z, 11);
    mcdis_dashed_line(BrC1_x, y0, BrC1_z, BrC3_x, y0, BrC3_z, 11);
    mcdis_dashed_line(BrT1_x, y0, BrT1_z, BrT2_x, y0, BrT2_z, 11);
    mcdis_dashed_line(BrT1_x, y0, BrT1_z, BrT3_x, y0, BrT3_z, 11);
  }
  mcdis_dashed_line(BrC1_x, -Bdelta_y, BrC1_z, BrC1_x, Bdelta_y, BrC1_z, 11);
  mcdis_dashed_line(BrC2_x, -Bdelta_y, BrC2_z, BrC2_x, Bdelta_y, BrC2_z, 11);
  mcdis_dashed_line(BrC3_x, -Bdelta_y, BrC3_z, BrC3_x, Bdelta_y, BrC3_z, 11);
  mcdis_dashed_line(BrT1_x, -Bdelta_y, BrT1_z, BrT1_x, Bdelta_y, BrT1_z, 11);
  mcdis_dashed_line(BrT2_x, -Bdelta_y, BrT2_z, BrT2_x, Bdelta_y, BrT2_z, 11);
  mcdis_dashed_line(BrT3_x, -Bdelta_y, BrT3_z, BrT3_x, Bdelta_y, BrT3_z, 11);
}
