#include "pose.h"

namespace {
#define DIM 18
#define EDIM 18
#define MEDIM 18
typedef void (*Hfun)(double *, double *, double *);
const static double MAHA_THRESH_4 = 7.814727903251177;
const static double MAHA_THRESH_10 = 7.814727903251177;
const static double MAHA_THRESH_13 = 7.814727903251177;
const static double MAHA_THRESH_14 = 7.814727903251177;

/******************************************************************************
 *                      Code generated with SymPy 1.14.0                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_3243830430541554586) {
   out_3243830430541554586[0] = delta_x[0] + nom_x[0];
   out_3243830430541554586[1] = delta_x[1] + nom_x[1];
   out_3243830430541554586[2] = delta_x[2] + nom_x[2];
   out_3243830430541554586[3] = delta_x[3] + nom_x[3];
   out_3243830430541554586[4] = delta_x[4] + nom_x[4];
   out_3243830430541554586[5] = delta_x[5] + nom_x[5];
   out_3243830430541554586[6] = delta_x[6] + nom_x[6];
   out_3243830430541554586[7] = delta_x[7] + nom_x[7];
   out_3243830430541554586[8] = delta_x[8] + nom_x[8];
   out_3243830430541554586[9] = delta_x[9] + nom_x[9];
   out_3243830430541554586[10] = delta_x[10] + nom_x[10];
   out_3243830430541554586[11] = delta_x[11] + nom_x[11];
   out_3243830430541554586[12] = delta_x[12] + nom_x[12];
   out_3243830430541554586[13] = delta_x[13] + nom_x[13];
   out_3243830430541554586[14] = delta_x[14] + nom_x[14];
   out_3243830430541554586[15] = delta_x[15] + nom_x[15];
   out_3243830430541554586[16] = delta_x[16] + nom_x[16];
   out_3243830430541554586[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_6240387057974568702) {
   out_6240387057974568702[0] = -nom_x[0] + true_x[0];
   out_6240387057974568702[1] = -nom_x[1] + true_x[1];
   out_6240387057974568702[2] = -nom_x[2] + true_x[2];
   out_6240387057974568702[3] = -nom_x[3] + true_x[3];
   out_6240387057974568702[4] = -nom_x[4] + true_x[4];
   out_6240387057974568702[5] = -nom_x[5] + true_x[5];
   out_6240387057974568702[6] = -nom_x[6] + true_x[6];
   out_6240387057974568702[7] = -nom_x[7] + true_x[7];
   out_6240387057974568702[8] = -nom_x[8] + true_x[8];
   out_6240387057974568702[9] = -nom_x[9] + true_x[9];
   out_6240387057974568702[10] = -nom_x[10] + true_x[10];
   out_6240387057974568702[11] = -nom_x[11] + true_x[11];
   out_6240387057974568702[12] = -nom_x[12] + true_x[12];
   out_6240387057974568702[13] = -nom_x[13] + true_x[13];
   out_6240387057974568702[14] = -nom_x[14] + true_x[14];
   out_6240387057974568702[15] = -nom_x[15] + true_x[15];
   out_6240387057974568702[16] = -nom_x[16] + true_x[16];
   out_6240387057974568702[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_8043295050634907169) {
   out_8043295050634907169[0] = 1.0;
   out_8043295050634907169[1] = 0.0;
   out_8043295050634907169[2] = 0.0;
   out_8043295050634907169[3] = 0.0;
   out_8043295050634907169[4] = 0.0;
   out_8043295050634907169[5] = 0.0;
   out_8043295050634907169[6] = 0.0;
   out_8043295050634907169[7] = 0.0;
   out_8043295050634907169[8] = 0.0;
   out_8043295050634907169[9] = 0.0;
   out_8043295050634907169[10] = 0.0;
   out_8043295050634907169[11] = 0.0;
   out_8043295050634907169[12] = 0.0;
   out_8043295050634907169[13] = 0.0;
   out_8043295050634907169[14] = 0.0;
   out_8043295050634907169[15] = 0.0;
   out_8043295050634907169[16] = 0.0;
   out_8043295050634907169[17] = 0.0;
   out_8043295050634907169[18] = 0.0;
   out_8043295050634907169[19] = 1.0;
   out_8043295050634907169[20] = 0.0;
   out_8043295050634907169[21] = 0.0;
   out_8043295050634907169[22] = 0.0;
   out_8043295050634907169[23] = 0.0;
   out_8043295050634907169[24] = 0.0;
   out_8043295050634907169[25] = 0.0;
   out_8043295050634907169[26] = 0.0;
   out_8043295050634907169[27] = 0.0;
   out_8043295050634907169[28] = 0.0;
   out_8043295050634907169[29] = 0.0;
   out_8043295050634907169[30] = 0.0;
   out_8043295050634907169[31] = 0.0;
   out_8043295050634907169[32] = 0.0;
   out_8043295050634907169[33] = 0.0;
   out_8043295050634907169[34] = 0.0;
   out_8043295050634907169[35] = 0.0;
   out_8043295050634907169[36] = 0.0;
   out_8043295050634907169[37] = 0.0;
   out_8043295050634907169[38] = 1.0;
   out_8043295050634907169[39] = 0.0;
   out_8043295050634907169[40] = 0.0;
   out_8043295050634907169[41] = 0.0;
   out_8043295050634907169[42] = 0.0;
   out_8043295050634907169[43] = 0.0;
   out_8043295050634907169[44] = 0.0;
   out_8043295050634907169[45] = 0.0;
   out_8043295050634907169[46] = 0.0;
   out_8043295050634907169[47] = 0.0;
   out_8043295050634907169[48] = 0.0;
   out_8043295050634907169[49] = 0.0;
   out_8043295050634907169[50] = 0.0;
   out_8043295050634907169[51] = 0.0;
   out_8043295050634907169[52] = 0.0;
   out_8043295050634907169[53] = 0.0;
   out_8043295050634907169[54] = 0.0;
   out_8043295050634907169[55] = 0.0;
   out_8043295050634907169[56] = 0.0;
   out_8043295050634907169[57] = 1.0;
   out_8043295050634907169[58] = 0.0;
   out_8043295050634907169[59] = 0.0;
   out_8043295050634907169[60] = 0.0;
   out_8043295050634907169[61] = 0.0;
   out_8043295050634907169[62] = 0.0;
   out_8043295050634907169[63] = 0.0;
   out_8043295050634907169[64] = 0.0;
   out_8043295050634907169[65] = 0.0;
   out_8043295050634907169[66] = 0.0;
   out_8043295050634907169[67] = 0.0;
   out_8043295050634907169[68] = 0.0;
   out_8043295050634907169[69] = 0.0;
   out_8043295050634907169[70] = 0.0;
   out_8043295050634907169[71] = 0.0;
   out_8043295050634907169[72] = 0.0;
   out_8043295050634907169[73] = 0.0;
   out_8043295050634907169[74] = 0.0;
   out_8043295050634907169[75] = 0.0;
   out_8043295050634907169[76] = 1.0;
   out_8043295050634907169[77] = 0.0;
   out_8043295050634907169[78] = 0.0;
   out_8043295050634907169[79] = 0.0;
   out_8043295050634907169[80] = 0.0;
   out_8043295050634907169[81] = 0.0;
   out_8043295050634907169[82] = 0.0;
   out_8043295050634907169[83] = 0.0;
   out_8043295050634907169[84] = 0.0;
   out_8043295050634907169[85] = 0.0;
   out_8043295050634907169[86] = 0.0;
   out_8043295050634907169[87] = 0.0;
   out_8043295050634907169[88] = 0.0;
   out_8043295050634907169[89] = 0.0;
   out_8043295050634907169[90] = 0.0;
   out_8043295050634907169[91] = 0.0;
   out_8043295050634907169[92] = 0.0;
   out_8043295050634907169[93] = 0.0;
   out_8043295050634907169[94] = 0.0;
   out_8043295050634907169[95] = 1.0;
   out_8043295050634907169[96] = 0.0;
   out_8043295050634907169[97] = 0.0;
   out_8043295050634907169[98] = 0.0;
   out_8043295050634907169[99] = 0.0;
   out_8043295050634907169[100] = 0.0;
   out_8043295050634907169[101] = 0.0;
   out_8043295050634907169[102] = 0.0;
   out_8043295050634907169[103] = 0.0;
   out_8043295050634907169[104] = 0.0;
   out_8043295050634907169[105] = 0.0;
   out_8043295050634907169[106] = 0.0;
   out_8043295050634907169[107] = 0.0;
   out_8043295050634907169[108] = 0.0;
   out_8043295050634907169[109] = 0.0;
   out_8043295050634907169[110] = 0.0;
   out_8043295050634907169[111] = 0.0;
   out_8043295050634907169[112] = 0.0;
   out_8043295050634907169[113] = 0.0;
   out_8043295050634907169[114] = 1.0;
   out_8043295050634907169[115] = 0.0;
   out_8043295050634907169[116] = 0.0;
   out_8043295050634907169[117] = 0.0;
   out_8043295050634907169[118] = 0.0;
   out_8043295050634907169[119] = 0.0;
   out_8043295050634907169[120] = 0.0;
   out_8043295050634907169[121] = 0.0;
   out_8043295050634907169[122] = 0.0;
   out_8043295050634907169[123] = 0.0;
   out_8043295050634907169[124] = 0.0;
   out_8043295050634907169[125] = 0.0;
   out_8043295050634907169[126] = 0.0;
   out_8043295050634907169[127] = 0.0;
   out_8043295050634907169[128] = 0.0;
   out_8043295050634907169[129] = 0.0;
   out_8043295050634907169[130] = 0.0;
   out_8043295050634907169[131] = 0.0;
   out_8043295050634907169[132] = 0.0;
   out_8043295050634907169[133] = 1.0;
   out_8043295050634907169[134] = 0.0;
   out_8043295050634907169[135] = 0.0;
   out_8043295050634907169[136] = 0.0;
   out_8043295050634907169[137] = 0.0;
   out_8043295050634907169[138] = 0.0;
   out_8043295050634907169[139] = 0.0;
   out_8043295050634907169[140] = 0.0;
   out_8043295050634907169[141] = 0.0;
   out_8043295050634907169[142] = 0.0;
   out_8043295050634907169[143] = 0.0;
   out_8043295050634907169[144] = 0.0;
   out_8043295050634907169[145] = 0.0;
   out_8043295050634907169[146] = 0.0;
   out_8043295050634907169[147] = 0.0;
   out_8043295050634907169[148] = 0.0;
   out_8043295050634907169[149] = 0.0;
   out_8043295050634907169[150] = 0.0;
   out_8043295050634907169[151] = 0.0;
   out_8043295050634907169[152] = 1.0;
   out_8043295050634907169[153] = 0.0;
   out_8043295050634907169[154] = 0.0;
   out_8043295050634907169[155] = 0.0;
   out_8043295050634907169[156] = 0.0;
   out_8043295050634907169[157] = 0.0;
   out_8043295050634907169[158] = 0.0;
   out_8043295050634907169[159] = 0.0;
   out_8043295050634907169[160] = 0.0;
   out_8043295050634907169[161] = 0.0;
   out_8043295050634907169[162] = 0.0;
   out_8043295050634907169[163] = 0.0;
   out_8043295050634907169[164] = 0.0;
   out_8043295050634907169[165] = 0.0;
   out_8043295050634907169[166] = 0.0;
   out_8043295050634907169[167] = 0.0;
   out_8043295050634907169[168] = 0.0;
   out_8043295050634907169[169] = 0.0;
   out_8043295050634907169[170] = 0.0;
   out_8043295050634907169[171] = 1.0;
   out_8043295050634907169[172] = 0.0;
   out_8043295050634907169[173] = 0.0;
   out_8043295050634907169[174] = 0.0;
   out_8043295050634907169[175] = 0.0;
   out_8043295050634907169[176] = 0.0;
   out_8043295050634907169[177] = 0.0;
   out_8043295050634907169[178] = 0.0;
   out_8043295050634907169[179] = 0.0;
   out_8043295050634907169[180] = 0.0;
   out_8043295050634907169[181] = 0.0;
   out_8043295050634907169[182] = 0.0;
   out_8043295050634907169[183] = 0.0;
   out_8043295050634907169[184] = 0.0;
   out_8043295050634907169[185] = 0.0;
   out_8043295050634907169[186] = 0.0;
   out_8043295050634907169[187] = 0.0;
   out_8043295050634907169[188] = 0.0;
   out_8043295050634907169[189] = 0.0;
   out_8043295050634907169[190] = 1.0;
   out_8043295050634907169[191] = 0.0;
   out_8043295050634907169[192] = 0.0;
   out_8043295050634907169[193] = 0.0;
   out_8043295050634907169[194] = 0.0;
   out_8043295050634907169[195] = 0.0;
   out_8043295050634907169[196] = 0.0;
   out_8043295050634907169[197] = 0.0;
   out_8043295050634907169[198] = 0.0;
   out_8043295050634907169[199] = 0.0;
   out_8043295050634907169[200] = 0.0;
   out_8043295050634907169[201] = 0.0;
   out_8043295050634907169[202] = 0.0;
   out_8043295050634907169[203] = 0.0;
   out_8043295050634907169[204] = 0.0;
   out_8043295050634907169[205] = 0.0;
   out_8043295050634907169[206] = 0.0;
   out_8043295050634907169[207] = 0.0;
   out_8043295050634907169[208] = 0.0;
   out_8043295050634907169[209] = 1.0;
   out_8043295050634907169[210] = 0.0;
   out_8043295050634907169[211] = 0.0;
   out_8043295050634907169[212] = 0.0;
   out_8043295050634907169[213] = 0.0;
   out_8043295050634907169[214] = 0.0;
   out_8043295050634907169[215] = 0.0;
   out_8043295050634907169[216] = 0.0;
   out_8043295050634907169[217] = 0.0;
   out_8043295050634907169[218] = 0.0;
   out_8043295050634907169[219] = 0.0;
   out_8043295050634907169[220] = 0.0;
   out_8043295050634907169[221] = 0.0;
   out_8043295050634907169[222] = 0.0;
   out_8043295050634907169[223] = 0.0;
   out_8043295050634907169[224] = 0.0;
   out_8043295050634907169[225] = 0.0;
   out_8043295050634907169[226] = 0.0;
   out_8043295050634907169[227] = 0.0;
   out_8043295050634907169[228] = 1.0;
   out_8043295050634907169[229] = 0.0;
   out_8043295050634907169[230] = 0.0;
   out_8043295050634907169[231] = 0.0;
   out_8043295050634907169[232] = 0.0;
   out_8043295050634907169[233] = 0.0;
   out_8043295050634907169[234] = 0.0;
   out_8043295050634907169[235] = 0.0;
   out_8043295050634907169[236] = 0.0;
   out_8043295050634907169[237] = 0.0;
   out_8043295050634907169[238] = 0.0;
   out_8043295050634907169[239] = 0.0;
   out_8043295050634907169[240] = 0.0;
   out_8043295050634907169[241] = 0.0;
   out_8043295050634907169[242] = 0.0;
   out_8043295050634907169[243] = 0.0;
   out_8043295050634907169[244] = 0.0;
   out_8043295050634907169[245] = 0.0;
   out_8043295050634907169[246] = 0.0;
   out_8043295050634907169[247] = 1.0;
   out_8043295050634907169[248] = 0.0;
   out_8043295050634907169[249] = 0.0;
   out_8043295050634907169[250] = 0.0;
   out_8043295050634907169[251] = 0.0;
   out_8043295050634907169[252] = 0.0;
   out_8043295050634907169[253] = 0.0;
   out_8043295050634907169[254] = 0.0;
   out_8043295050634907169[255] = 0.0;
   out_8043295050634907169[256] = 0.0;
   out_8043295050634907169[257] = 0.0;
   out_8043295050634907169[258] = 0.0;
   out_8043295050634907169[259] = 0.0;
   out_8043295050634907169[260] = 0.0;
   out_8043295050634907169[261] = 0.0;
   out_8043295050634907169[262] = 0.0;
   out_8043295050634907169[263] = 0.0;
   out_8043295050634907169[264] = 0.0;
   out_8043295050634907169[265] = 0.0;
   out_8043295050634907169[266] = 1.0;
   out_8043295050634907169[267] = 0.0;
   out_8043295050634907169[268] = 0.0;
   out_8043295050634907169[269] = 0.0;
   out_8043295050634907169[270] = 0.0;
   out_8043295050634907169[271] = 0.0;
   out_8043295050634907169[272] = 0.0;
   out_8043295050634907169[273] = 0.0;
   out_8043295050634907169[274] = 0.0;
   out_8043295050634907169[275] = 0.0;
   out_8043295050634907169[276] = 0.0;
   out_8043295050634907169[277] = 0.0;
   out_8043295050634907169[278] = 0.0;
   out_8043295050634907169[279] = 0.0;
   out_8043295050634907169[280] = 0.0;
   out_8043295050634907169[281] = 0.0;
   out_8043295050634907169[282] = 0.0;
   out_8043295050634907169[283] = 0.0;
   out_8043295050634907169[284] = 0.0;
   out_8043295050634907169[285] = 1.0;
   out_8043295050634907169[286] = 0.0;
   out_8043295050634907169[287] = 0.0;
   out_8043295050634907169[288] = 0.0;
   out_8043295050634907169[289] = 0.0;
   out_8043295050634907169[290] = 0.0;
   out_8043295050634907169[291] = 0.0;
   out_8043295050634907169[292] = 0.0;
   out_8043295050634907169[293] = 0.0;
   out_8043295050634907169[294] = 0.0;
   out_8043295050634907169[295] = 0.0;
   out_8043295050634907169[296] = 0.0;
   out_8043295050634907169[297] = 0.0;
   out_8043295050634907169[298] = 0.0;
   out_8043295050634907169[299] = 0.0;
   out_8043295050634907169[300] = 0.0;
   out_8043295050634907169[301] = 0.0;
   out_8043295050634907169[302] = 0.0;
   out_8043295050634907169[303] = 0.0;
   out_8043295050634907169[304] = 1.0;
   out_8043295050634907169[305] = 0.0;
   out_8043295050634907169[306] = 0.0;
   out_8043295050634907169[307] = 0.0;
   out_8043295050634907169[308] = 0.0;
   out_8043295050634907169[309] = 0.0;
   out_8043295050634907169[310] = 0.0;
   out_8043295050634907169[311] = 0.0;
   out_8043295050634907169[312] = 0.0;
   out_8043295050634907169[313] = 0.0;
   out_8043295050634907169[314] = 0.0;
   out_8043295050634907169[315] = 0.0;
   out_8043295050634907169[316] = 0.0;
   out_8043295050634907169[317] = 0.0;
   out_8043295050634907169[318] = 0.0;
   out_8043295050634907169[319] = 0.0;
   out_8043295050634907169[320] = 0.0;
   out_8043295050634907169[321] = 0.0;
   out_8043295050634907169[322] = 0.0;
   out_8043295050634907169[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_1922886578098749649) {
   out_1922886578098749649[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_1922886578098749649[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_1922886578098749649[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_1922886578098749649[3] = dt*state[12] + state[3];
   out_1922886578098749649[4] = dt*state[13] + state[4];
   out_1922886578098749649[5] = dt*state[14] + state[5];
   out_1922886578098749649[6] = state[6];
   out_1922886578098749649[7] = state[7];
   out_1922886578098749649[8] = state[8];
   out_1922886578098749649[9] = state[9];
   out_1922886578098749649[10] = state[10];
   out_1922886578098749649[11] = state[11];
   out_1922886578098749649[12] = state[12];
   out_1922886578098749649[13] = state[13];
   out_1922886578098749649[14] = state[14];
   out_1922886578098749649[15] = state[15];
   out_1922886578098749649[16] = state[16];
   out_1922886578098749649[17] = state[17];
}
void F_fun(double *state, double dt, double *out_3887137653751174596) {
   out_3887137653751174596[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_3887137653751174596[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_3887137653751174596[2] = 0;
   out_3887137653751174596[3] = 0;
   out_3887137653751174596[4] = 0;
   out_3887137653751174596[5] = 0;
   out_3887137653751174596[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_3887137653751174596[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_3887137653751174596[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_3887137653751174596[9] = 0;
   out_3887137653751174596[10] = 0;
   out_3887137653751174596[11] = 0;
   out_3887137653751174596[12] = 0;
   out_3887137653751174596[13] = 0;
   out_3887137653751174596[14] = 0;
   out_3887137653751174596[15] = 0;
   out_3887137653751174596[16] = 0;
   out_3887137653751174596[17] = 0;
   out_3887137653751174596[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_3887137653751174596[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_3887137653751174596[20] = 0;
   out_3887137653751174596[21] = 0;
   out_3887137653751174596[22] = 0;
   out_3887137653751174596[23] = 0;
   out_3887137653751174596[24] = 0;
   out_3887137653751174596[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_3887137653751174596[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_3887137653751174596[27] = 0;
   out_3887137653751174596[28] = 0;
   out_3887137653751174596[29] = 0;
   out_3887137653751174596[30] = 0;
   out_3887137653751174596[31] = 0;
   out_3887137653751174596[32] = 0;
   out_3887137653751174596[33] = 0;
   out_3887137653751174596[34] = 0;
   out_3887137653751174596[35] = 0;
   out_3887137653751174596[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_3887137653751174596[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_3887137653751174596[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_3887137653751174596[39] = 0;
   out_3887137653751174596[40] = 0;
   out_3887137653751174596[41] = 0;
   out_3887137653751174596[42] = 0;
   out_3887137653751174596[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_3887137653751174596[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_3887137653751174596[45] = 0;
   out_3887137653751174596[46] = 0;
   out_3887137653751174596[47] = 0;
   out_3887137653751174596[48] = 0;
   out_3887137653751174596[49] = 0;
   out_3887137653751174596[50] = 0;
   out_3887137653751174596[51] = 0;
   out_3887137653751174596[52] = 0;
   out_3887137653751174596[53] = 0;
   out_3887137653751174596[54] = 0;
   out_3887137653751174596[55] = 0;
   out_3887137653751174596[56] = 0;
   out_3887137653751174596[57] = 1;
   out_3887137653751174596[58] = 0;
   out_3887137653751174596[59] = 0;
   out_3887137653751174596[60] = 0;
   out_3887137653751174596[61] = 0;
   out_3887137653751174596[62] = 0;
   out_3887137653751174596[63] = 0;
   out_3887137653751174596[64] = 0;
   out_3887137653751174596[65] = 0;
   out_3887137653751174596[66] = dt;
   out_3887137653751174596[67] = 0;
   out_3887137653751174596[68] = 0;
   out_3887137653751174596[69] = 0;
   out_3887137653751174596[70] = 0;
   out_3887137653751174596[71] = 0;
   out_3887137653751174596[72] = 0;
   out_3887137653751174596[73] = 0;
   out_3887137653751174596[74] = 0;
   out_3887137653751174596[75] = 0;
   out_3887137653751174596[76] = 1;
   out_3887137653751174596[77] = 0;
   out_3887137653751174596[78] = 0;
   out_3887137653751174596[79] = 0;
   out_3887137653751174596[80] = 0;
   out_3887137653751174596[81] = 0;
   out_3887137653751174596[82] = 0;
   out_3887137653751174596[83] = 0;
   out_3887137653751174596[84] = 0;
   out_3887137653751174596[85] = dt;
   out_3887137653751174596[86] = 0;
   out_3887137653751174596[87] = 0;
   out_3887137653751174596[88] = 0;
   out_3887137653751174596[89] = 0;
   out_3887137653751174596[90] = 0;
   out_3887137653751174596[91] = 0;
   out_3887137653751174596[92] = 0;
   out_3887137653751174596[93] = 0;
   out_3887137653751174596[94] = 0;
   out_3887137653751174596[95] = 1;
   out_3887137653751174596[96] = 0;
   out_3887137653751174596[97] = 0;
   out_3887137653751174596[98] = 0;
   out_3887137653751174596[99] = 0;
   out_3887137653751174596[100] = 0;
   out_3887137653751174596[101] = 0;
   out_3887137653751174596[102] = 0;
   out_3887137653751174596[103] = 0;
   out_3887137653751174596[104] = dt;
   out_3887137653751174596[105] = 0;
   out_3887137653751174596[106] = 0;
   out_3887137653751174596[107] = 0;
   out_3887137653751174596[108] = 0;
   out_3887137653751174596[109] = 0;
   out_3887137653751174596[110] = 0;
   out_3887137653751174596[111] = 0;
   out_3887137653751174596[112] = 0;
   out_3887137653751174596[113] = 0;
   out_3887137653751174596[114] = 1;
   out_3887137653751174596[115] = 0;
   out_3887137653751174596[116] = 0;
   out_3887137653751174596[117] = 0;
   out_3887137653751174596[118] = 0;
   out_3887137653751174596[119] = 0;
   out_3887137653751174596[120] = 0;
   out_3887137653751174596[121] = 0;
   out_3887137653751174596[122] = 0;
   out_3887137653751174596[123] = 0;
   out_3887137653751174596[124] = 0;
   out_3887137653751174596[125] = 0;
   out_3887137653751174596[126] = 0;
   out_3887137653751174596[127] = 0;
   out_3887137653751174596[128] = 0;
   out_3887137653751174596[129] = 0;
   out_3887137653751174596[130] = 0;
   out_3887137653751174596[131] = 0;
   out_3887137653751174596[132] = 0;
   out_3887137653751174596[133] = 1;
   out_3887137653751174596[134] = 0;
   out_3887137653751174596[135] = 0;
   out_3887137653751174596[136] = 0;
   out_3887137653751174596[137] = 0;
   out_3887137653751174596[138] = 0;
   out_3887137653751174596[139] = 0;
   out_3887137653751174596[140] = 0;
   out_3887137653751174596[141] = 0;
   out_3887137653751174596[142] = 0;
   out_3887137653751174596[143] = 0;
   out_3887137653751174596[144] = 0;
   out_3887137653751174596[145] = 0;
   out_3887137653751174596[146] = 0;
   out_3887137653751174596[147] = 0;
   out_3887137653751174596[148] = 0;
   out_3887137653751174596[149] = 0;
   out_3887137653751174596[150] = 0;
   out_3887137653751174596[151] = 0;
   out_3887137653751174596[152] = 1;
   out_3887137653751174596[153] = 0;
   out_3887137653751174596[154] = 0;
   out_3887137653751174596[155] = 0;
   out_3887137653751174596[156] = 0;
   out_3887137653751174596[157] = 0;
   out_3887137653751174596[158] = 0;
   out_3887137653751174596[159] = 0;
   out_3887137653751174596[160] = 0;
   out_3887137653751174596[161] = 0;
   out_3887137653751174596[162] = 0;
   out_3887137653751174596[163] = 0;
   out_3887137653751174596[164] = 0;
   out_3887137653751174596[165] = 0;
   out_3887137653751174596[166] = 0;
   out_3887137653751174596[167] = 0;
   out_3887137653751174596[168] = 0;
   out_3887137653751174596[169] = 0;
   out_3887137653751174596[170] = 0;
   out_3887137653751174596[171] = 1;
   out_3887137653751174596[172] = 0;
   out_3887137653751174596[173] = 0;
   out_3887137653751174596[174] = 0;
   out_3887137653751174596[175] = 0;
   out_3887137653751174596[176] = 0;
   out_3887137653751174596[177] = 0;
   out_3887137653751174596[178] = 0;
   out_3887137653751174596[179] = 0;
   out_3887137653751174596[180] = 0;
   out_3887137653751174596[181] = 0;
   out_3887137653751174596[182] = 0;
   out_3887137653751174596[183] = 0;
   out_3887137653751174596[184] = 0;
   out_3887137653751174596[185] = 0;
   out_3887137653751174596[186] = 0;
   out_3887137653751174596[187] = 0;
   out_3887137653751174596[188] = 0;
   out_3887137653751174596[189] = 0;
   out_3887137653751174596[190] = 1;
   out_3887137653751174596[191] = 0;
   out_3887137653751174596[192] = 0;
   out_3887137653751174596[193] = 0;
   out_3887137653751174596[194] = 0;
   out_3887137653751174596[195] = 0;
   out_3887137653751174596[196] = 0;
   out_3887137653751174596[197] = 0;
   out_3887137653751174596[198] = 0;
   out_3887137653751174596[199] = 0;
   out_3887137653751174596[200] = 0;
   out_3887137653751174596[201] = 0;
   out_3887137653751174596[202] = 0;
   out_3887137653751174596[203] = 0;
   out_3887137653751174596[204] = 0;
   out_3887137653751174596[205] = 0;
   out_3887137653751174596[206] = 0;
   out_3887137653751174596[207] = 0;
   out_3887137653751174596[208] = 0;
   out_3887137653751174596[209] = 1;
   out_3887137653751174596[210] = 0;
   out_3887137653751174596[211] = 0;
   out_3887137653751174596[212] = 0;
   out_3887137653751174596[213] = 0;
   out_3887137653751174596[214] = 0;
   out_3887137653751174596[215] = 0;
   out_3887137653751174596[216] = 0;
   out_3887137653751174596[217] = 0;
   out_3887137653751174596[218] = 0;
   out_3887137653751174596[219] = 0;
   out_3887137653751174596[220] = 0;
   out_3887137653751174596[221] = 0;
   out_3887137653751174596[222] = 0;
   out_3887137653751174596[223] = 0;
   out_3887137653751174596[224] = 0;
   out_3887137653751174596[225] = 0;
   out_3887137653751174596[226] = 0;
   out_3887137653751174596[227] = 0;
   out_3887137653751174596[228] = 1;
   out_3887137653751174596[229] = 0;
   out_3887137653751174596[230] = 0;
   out_3887137653751174596[231] = 0;
   out_3887137653751174596[232] = 0;
   out_3887137653751174596[233] = 0;
   out_3887137653751174596[234] = 0;
   out_3887137653751174596[235] = 0;
   out_3887137653751174596[236] = 0;
   out_3887137653751174596[237] = 0;
   out_3887137653751174596[238] = 0;
   out_3887137653751174596[239] = 0;
   out_3887137653751174596[240] = 0;
   out_3887137653751174596[241] = 0;
   out_3887137653751174596[242] = 0;
   out_3887137653751174596[243] = 0;
   out_3887137653751174596[244] = 0;
   out_3887137653751174596[245] = 0;
   out_3887137653751174596[246] = 0;
   out_3887137653751174596[247] = 1;
   out_3887137653751174596[248] = 0;
   out_3887137653751174596[249] = 0;
   out_3887137653751174596[250] = 0;
   out_3887137653751174596[251] = 0;
   out_3887137653751174596[252] = 0;
   out_3887137653751174596[253] = 0;
   out_3887137653751174596[254] = 0;
   out_3887137653751174596[255] = 0;
   out_3887137653751174596[256] = 0;
   out_3887137653751174596[257] = 0;
   out_3887137653751174596[258] = 0;
   out_3887137653751174596[259] = 0;
   out_3887137653751174596[260] = 0;
   out_3887137653751174596[261] = 0;
   out_3887137653751174596[262] = 0;
   out_3887137653751174596[263] = 0;
   out_3887137653751174596[264] = 0;
   out_3887137653751174596[265] = 0;
   out_3887137653751174596[266] = 1;
   out_3887137653751174596[267] = 0;
   out_3887137653751174596[268] = 0;
   out_3887137653751174596[269] = 0;
   out_3887137653751174596[270] = 0;
   out_3887137653751174596[271] = 0;
   out_3887137653751174596[272] = 0;
   out_3887137653751174596[273] = 0;
   out_3887137653751174596[274] = 0;
   out_3887137653751174596[275] = 0;
   out_3887137653751174596[276] = 0;
   out_3887137653751174596[277] = 0;
   out_3887137653751174596[278] = 0;
   out_3887137653751174596[279] = 0;
   out_3887137653751174596[280] = 0;
   out_3887137653751174596[281] = 0;
   out_3887137653751174596[282] = 0;
   out_3887137653751174596[283] = 0;
   out_3887137653751174596[284] = 0;
   out_3887137653751174596[285] = 1;
   out_3887137653751174596[286] = 0;
   out_3887137653751174596[287] = 0;
   out_3887137653751174596[288] = 0;
   out_3887137653751174596[289] = 0;
   out_3887137653751174596[290] = 0;
   out_3887137653751174596[291] = 0;
   out_3887137653751174596[292] = 0;
   out_3887137653751174596[293] = 0;
   out_3887137653751174596[294] = 0;
   out_3887137653751174596[295] = 0;
   out_3887137653751174596[296] = 0;
   out_3887137653751174596[297] = 0;
   out_3887137653751174596[298] = 0;
   out_3887137653751174596[299] = 0;
   out_3887137653751174596[300] = 0;
   out_3887137653751174596[301] = 0;
   out_3887137653751174596[302] = 0;
   out_3887137653751174596[303] = 0;
   out_3887137653751174596[304] = 1;
   out_3887137653751174596[305] = 0;
   out_3887137653751174596[306] = 0;
   out_3887137653751174596[307] = 0;
   out_3887137653751174596[308] = 0;
   out_3887137653751174596[309] = 0;
   out_3887137653751174596[310] = 0;
   out_3887137653751174596[311] = 0;
   out_3887137653751174596[312] = 0;
   out_3887137653751174596[313] = 0;
   out_3887137653751174596[314] = 0;
   out_3887137653751174596[315] = 0;
   out_3887137653751174596[316] = 0;
   out_3887137653751174596[317] = 0;
   out_3887137653751174596[318] = 0;
   out_3887137653751174596[319] = 0;
   out_3887137653751174596[320] = 0;
   out_3887137653751174596[321] = 0;
   out_3887137653751174596[322] = 0;
   out_3887137653751174596[323] = 1;
}
void h_4(double *state, double *unused, double *out_6757832056893505158) {
   out_6757832056893505158[0] = state[6] + state[9];
   out_6757832056893505158[1] = state[7] + state[10];
   out_6757832056893505158[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_4271487840790999597) {
   out_4271487840790999597[0] = 0;
   out_4271487840790999597[1] = 0;
   out_4271487840790999597[2] = 0;
   out_4271487840790999597[3] = 0;
   out_4271487840790999597[4] = 0;
   out_4271487840790999597[5] = 0;
   out_4271487840790999597[6] = 1;
   out_4271487840790999597[7] = 0;
   out_4271487840790999597[8] = 0;
   out_4271487840790999597[9] = 1;
   out_4271487840790999597[10] = 0;
   out_4271487840790999597[11] = 0;
   out_4271487840790999597[12] = 0;
   out_4271487840790999597[13] = 0;
   out_4271487840790999597[14] = 0;
   out_4271487840790999597[15] = 0;
   out_4271487840790999597[16] = 0;
   out_4271487840790999597[17] = 0;
   out_4271487840790999597[18] = 0;
   out_4271487840790999597[19] = 0;
   out_4271487840790999597[20] = 0;
   out_4271487840790999597[21] = 0;
   out_4271487840790999597[22] = 0;
   out_4271487840790999597[23] = 0;
   out_4271487840790999597[24] = 0;
   out_4271487840790999597[25] = 1;
   out_4271487840790999597[26] = 0;
   out_4271487840790999597[27] = 0;
   out_4271487840790999597[28] = 1;
   out_4271487840790999597[29] = 0;
   out_4271487840790999597[30] = 0;
   out_4271487840790999597[31] = 0;
   out_4271487840790999597[32] = 0;
   out_4271487840790999597[33] = 0;
   out_4271487840790999597[34] = 0;
   out_4271487840790999597[35] = 0;
   out_4271487840790999597[36] = 0;
   out_4271487840790999597[37] = 0;
   out_4271487840790999597[38] = 0;
   out_4271487840790999597[39] = 0;
   out_4271487840790999597[40] = 0;
   out_4271487840790999597[41] = 0;
   out_4271487840790999597[42] = 0;
   out_4271487840790999597[43] = 0;
   out_4271487840790999597[44] = 1;
   out_4271487840790999597[45] = 0;
   out_4271487840790999597[46] = 0;
   out_4271487840790999597[47] = 1;
   out_4271487840790999597[48] = 0;
   out_4271487840790999597[49] = 0;
   out_4271487840790999597[50] = 0;
   out_4271487840790999597[51] = 0;
   out_4271487840790999597[52] = 0;
   out_4271487840790999597[53] = 0;
}
void h_10(double *state, double *unused, double *out_1896753776489155926) {
   out_1896753776489155926[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_1896753776489155926[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_1896753776489155926[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_5115826266223761764) {
   out_5115826266223761764[0] = 0;
   out_5115826266223761764[1] = 9.8100000000000005*cos(state[1]);
   out_5115826266223761764[2] = 0;
   out_5115826266223761764[3] = 0;
   out_5115826266223761764[4] = -state[8];
   out_5115826266223761764[5] = state[7];
   out_5115826266223761764[6] = 0;
   out_5115826266223761764[7] = state[5];
   out_5115826266223761764[8] = -state[4];
   out_5115826266223761764[9] = 0;
   out_5115826266223761764[10] = 0;
   out_5115826266223761764[11] = 0;
   out_5115826266223761764[12] = 1;
   out_5115826266223761764[13] = 0;
   out_5115826266223761764[14] = 0;
   out_5115826266223761764[15] = 1;
   out_5115826266223761764[16] = 0;
   out_5115826266223761764[17] = 0;
   out_5115826266223761764[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_5115826266223761764[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_5115826266223761764[20] = 0;
   out_5115826266223761764[21] = state[8];
   out_5115826266223761764[22] = 0;
   out_5115826266223761764[23] = -state[6];
   out_5115826266223761764[24] = -state[5];
   out_5115826266223761764[25] = 0;
   out_5115826266223761764[26] = state[3];
   out_5115826266223761764[27] = 0;
   out_5115826266223761764[28] = 0;
   out_5115826266223761764[29] = 0;
   out_5115826266223761764[30] = 0;
   out_5115826266223761764[31] = 1;
   out_5115826266223761764[32] = 0;
   out_5115826266223761764[33] = 0;
   out_5115826266223761764[34] = 1;
   out_5115826266223761764[35] = 0;
   out_5115826266223761764[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_5115826266223761764[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_5115826266223761764[38] = 0;
   out_5115826266223761764[39] = -state[7];
   out_5115826266223761764[40] = state[6];
   out_5115826266223761764[41] = 0;
   out_5115826266223761764[42] = state[4];
   out_5115826266223761764[43] = -state[3];
   out_5115826266223761764[44] = 0;
   out_5115826266223761764[45] = 0;
   out_5115826266223761764[46] = 0;
   out_5115826266223761764[47] = 0;
   out_5115826266223761764[48] = 0;
   out_5115826266223761764[49] = 0;
   out_5115826266223761764[50] = 1;
   out_5115826266223761764[51] = 0;
   out_5115826266223761764[52] = 0;
   out_5115826266223761764[53] = 1;
}
void h_13(double *state, double *unused, double *out_3373504601145192164) {
   out_3373504601145192164[0] = state[3];
   out_3373504601145192164[1] = state[4];
   out_3373504601145192164[2] = state[5];
}
void H_13(double *state, double *unused, double *out_3706885921109155493) {
   out_3706885921109155493[0] = 0;
   out_3706885921109155493[1] = 0;
   out_3706885921109155493[2] = 0;
   out_3706885921109155493[3] = 1;
   out_3706885921109155493[4] = 0;
   out_3706885921109155493[5] = 0;
   out_3706885921109155493[6] = 0;
   out_3706885921109155493[7] = 0;
   out_3706885921109155493[8] = 0;
   out_3706885921109155493[9] = 0;
   out_3706885921109155493[10] = 0;
   out_3706885921109155493[11] = 0;
   out_3706885921109155493[12] = 0;
   out_3706885921109155493[13] = 0;
   out_3706885921109155493[14] = 0;
   out_3706885921109155493[15] = 0;
   out_3706885921109155493[16] = 0;
   out_3706885921109155493[17] = 0;
   out_3706885921109155493[18] = 0;
   out_3706885921109155493[19] = 0;
   out_3706885921109155493[20] = 0;
   out_3706885921109155493[21] = 0;
   out_3706885921109155493[22] = 1;
   out_3706885921109155493[23] = 0;
   out_3706885921109155493[24] = 0;
   out_3706885921109155493[25] = 0;
   out_3706885921109155493[26] = 0;
   out_3706885921109155493[27] = 0;
   out_3706885921109155493[28] = 0;
   out_3706885921109155493[29] = 0;
   out_3706885921109155493[30] = 0;
   out_3706885921109155493[31] = 0;
   out_3706885921109155493[32] = 0;
   out_3706885921109155493[33] = 0;
   out_3706885921109155493[34] = 0;
   out_3706885921109155493[35] = 0;
   out_3706885921109155493[36] = 0;
   out_3706885921109155493[37] = 0;
   out_3706885921109155493[38] = 0;
   out_3706885921109155493[39] = 0;
   out_3706885921109155493[40] = 0;
   out_3706885921109155493[41] = 1;
   out_3706885921109155493[42] = 0;
   out_3706885921109155493[43] = 0;
   out_3706885921109155493[44] = 0;
   out_3706885921109155493[45] = 0;
   out_3706885921109155493[46] = 0;
   out_3706885921109155493[47] = 0;
   out_3706885921109155493[48] = 0;
   out_3706885921109155493[49] = 0;
   out_3706885921109155493[50] = 0;
   out_3706885921109155493[51] = 0;
   out_3706885921109155493[52] = 0;
   out_3706885921109155493[53] = 0;
}
void h_14(double *state, double *unused, double *out_6472387792798353966) {
   out_6472387792798353966[0] = state[6];
   out_6472387792798353966[1] = state[7];
   out_6472387792798353966[2] = state[8];
}
void H_14(double *state, double *unused, double *out_7354276273086371893) {
   out_7354276273086371893[0] = 0;
   out_7354276273086371893[1] = 0;
   out_7354276273086371893[2] = 0;
   out_7354276273086371893[3] = 0;
   out_7354276273086371893[4] = 0;
   out_7354276273086371893[5] = 0;
   out_7354276273086371893[6] = 1;
   out_7354276273086371893[7] = 0;
   out_7354276273086371893[8] = 0;
   out_7354276273086371893[9] = 0;
   out_7354276273086371893[10] = 0;
   out_7354276273086371893[11] = 0;
   out_7354276273086371893[12] = 0;
   out_7354276273086371893[13] = 0;
   out_7354276273086371893[14] = 0;
   out_7354276273086371893[15] = 0;
   out_7354276273086371893[16] = 0;
   out_7354276273086371893[17] = 0;
   out_7354276273086371893[18] = 0;
   out_7354276273086371893[19] = 0;
   out_7354276273086371893[20] = 0;
   out_7354276273086371893[21] = 0;
   out_7354276273086371893[22] = 0;
   out_7354276273086371893[23] = 0;
   out_7354276273086371893[24] = 0;
   out_7354276273086371893[25] = 1;
   out_7354276273086371893[26] = 0;
   out_7354276273086371893[27] = 0;
   out_7354276273086371893[28] = 0;
   out_7354276273086371893[29] = 0;
   out_7354276273086371893[30] = 0;
   out_7354276273086371893[31] = 0;
   out_7354276273086371893[32] = 0;
   out_7354276273086371893[33] = 0;
   out_7354276273086371893[34] = 0;
   out_7354276273086371893[35] = 0;
   out_7354276273086371893[36] = 0;
   out_7354276273086371893[37] = 0;
   out_7354276273086371893[38] = 0;
   out_7354276273086371893[39] = 0;
   out_7354276273086371893[40] = 0;
   out_7354276273086371893[41] = 0;
   out_7354276273086371893[42] = 0;
   out_7354276273086371893[43] = 0;
   out_7354276273086371893[44] = 1;
   out_7354276273086371893[45] = 0;
   out_7354276273086371893[46] = 0;
   out_7354276273086371893[47] = 0;
   out_7354276273086371893[48] = 0;
   out_7354276273086371893[49] = 0;
   out_7354276273086371893[50] = 0;
   out_7354276273086371893[51] = 0;
   out_7354276273086371893[52] = 0;
   out_7354276273086371893[53] = 0;
}
#include <eigen3/Eigen/Dense>
#include <iostream>

typedef Eigen::Matrix<double, DIM, DIM, Eigen::RowMajor> DDM;
typedef Eigen::Matrix<double, EDIM, EDIM, Eigen::RowMajor> EEM;
typedef Eigen::Matrix<double, DIM, EDIM, Eigen::RowMajor> DEM;

void predict(double *in_x, double *in_P, double *in_Q, double dt) {
  typedef Eigen::Matrix<double, MEDIM, MEDIM, Eigen::RowMajor> RRM;

  double nx[DIM] = {0};
  double in_F[EDIM*EDIM] = {0};

  // functions from sympy
  f_fun(in_x, dt, nx);
  F_fun(in_x, dt, in_F);


  EEM F(in_F);
  EEM P(in_P);
  EEM Q(in_Q);

  RRM F_main = F.topLeftCorner(MEDIM, MEDIM);
  P.topLeftCorner(MEDIM, MEDIM) = (F_main * P.topLeftCorner(MEDIM, MEDIM)) * F_main.transpose();
  P.topRightCorner(MEDIM, EDIM - MEDIM) = F_main * P.topRightCorner(MEDIM, EDIM - MEDIM);
  P.bottomLeftCorner(EDIM - MEDIM, MEDIM) = P.bottomLeftCorner(EDIM - MEDIM, MEDIM) * F_main.transpose();

  P = P + dt*Q;

  // copy out state
  memcpy(in_x, nx, DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
}

// note: extra_args dim only correct when null space projecting
// otherwise 1
template <int ZDIM, int EADIM, bool MAHA_TEST>
void update(double *in_x, double *in_P, Hfun h_fun, Hfun H_fun, Hfun Hea_fun, double *in_z, double *in_R, double *in_ea, double MAHA_THRESHOLD) {
  typedef Eigen::Matrix<double, ZDIM, ZDIM, Eigen::RowMajor> ZZM;
  typedef Eigen::Matrix<double, ZDIM, DIM, Eigen::RowMajor> ZDM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, EDIM, Eigen::RowMajor> XEM;
  //typedef Eigen::Matrix<double, EDIM, ZDIM, Eigen::RowMajor> EZM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> X1M;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> XXM;

  double in_hx[ZDIM] = {0};
  double in_H[ZDIM * DIM] = {0};
  double in_H_mod[EDIM * DIM] = {0};
  double delta_x[EDIM] = {0};
  double x_new[DIM] = {0};


  // state x, P
  Eigen::Matrix<double, ZDIM, 1> z(in_z);
  EEM P(in_P);
  ZZM pre_R(in_R);

  // functions from sympy
  h_fun(in_x, in_ea, in_hx);
  H_fun(in_x, in_ea, in_H);
  ZDM pre_H(in_H);

  // get y (y = z - hx)
  Eigen::Matrix<double, ZDIM, 1> pre_y(in_hx); pre_y = z - pre_y;
  X1M y; XXM H; XXM R;
  if (Hea_fun){
    typedef Eigen::Matrix<double, ZDIM, EADIM, Eigen::RowMajor> ZAM;
    double in_Hea[ZDIM * EADIM] = {0};
    Hea_fun(in_x, in_ea, in_Hea);
    ZAM Hea(in_Hea);
    XXM A = Hea.transpose().fullPivLu().kernel();


    y = A.transpose() * pre_y;
    H = A.transpose() * pre_H;
    R = A.transpose() * pre_R * A;
  } else {
    y = pre_y;
    H = pre_H;
    R = pre_R;
  }
  // get modified H
  H_mod_fun(in_x, in_H_mod);
  DEM H_mod(in_H_mod);
  XEM H_err = H * H_mod;

  // Do mahalobis distance test
  if (MAHA_TEST){
    XXM a = (H_err * P * H_err.transpose() + R).inverse();
    double maha_dist = y.transpose() * a * y;
    if (maha_dist > MAHA_THRESHOLD){
      R = 1.0e16 * R;
    }
  }

  // Outlier resilient weighting
  double weight = 1;//(1.5)/(1 + y.squaredNorm()/R.sum());

  // kalman gains and I_KH
  XXM S = ((H_err * P) * H_err.transpose()) + R/weight;
  XEM KT = S.fullPivLu().solve(H_err * P.transpose());
  //EZM K = KT.transpose(); TODO: WHY DOES THIS NOT COMPILE?
  //EZM K = S.fullPivLu().solve(H_err * P.transpose()).transpose();
  //std::cout << "Here is the matrix rot:\n" << K << std::endl;
  EEM I_KH = Eigen::Matrix<double, EDIM, EDIM>::Identity() - (KT.transpose() * H_err);

  // update state by injecting dx
  Eigen::Matrix<double, EDIM, 1> dx(delta_x);
  dx  = (KT.transpose() * y);
  memcpy(delta_x, dx.data(), EDIM * sizeof(double));
  err_fun(in_x, delta_x, x_new);
  Eigen::Matrix<double, DIM, 1> x(x_new);

  // update cov
  P = ((I_KH * P) * I_KH.transpose()) + ((KT.transpose() * R) * KT);

  // copy out state
  memcpy(in_x, x.data(), DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
  memcpy(in_z, y.data(), y.rows() * sizeof(double));
}




}
extern "C" {

void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_4, H_4, NULL, in_z, in_R, in_ea, MAHA_THRESH_4);
}
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_10, H_10, NULL, in_z, in_R, in_ea, MAHA_THRESH_10);
}
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_13, H_13, NULL, in_z, in_R, in_ea, MAHA_THRESH_13);
}
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_14, H_14, NULL, in_z, in_R, in_ea, MAHA_THRESH_14);
}
void pose_err_fun(double *nom_x, double *delta_x, double *out_3243830430541554586) {
  err_fun(nom_x, delta_x, out_3243830430541554586);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_6240387057974568702) {
  inv_err_fun(nom_x, true_x, out_6240387057974568702);
}
void pose_H_mod_fun(double *state, double *out_8043295050634907169) {
  H_mod_fun(state, out_8043295050634907169);
}
void pose_f_fun(double *state, double dt, double *out_1922886578098749649) {
  f_fun(state,  dt, out_1922886578098749649);
}
void pose_F_fun(double *state, double dt, double *out_3887137653751174596) {
  F_fun(state,  dt, out_3887137653751174596);
}
void pose_h_4(double *state, double *unused, double *out_6757832056893505158) {
  h_4(state, unused, out_6757832056893505158);
}
void pose_H_4(double *state, double *unused, double *out_4271487840790999597) {
  H_4(state, unused, out_4271487840790999597);
}
void pose_h_10(double *state, double *unused, double *out_1896753776489155926) {
  h_10(state, unused, out_1896753776489155926);
}
void pose_H_10(double *state, double *unused, double *out_5115826266223761764) {
  H_10(state, unused, out_5115826266223761764);
}
void pose_h_13(double *state, double *unused, double *out_3373504601145192164) {
  h_13(state, unused, out_3373504601145192164);
}
void pose_H_13(double *state, double *unused, double *out_3706885921109155493) {
  H_13(state, unused, out_3706885921109155493);
}
void pose_h_14(double *state, double *unused, double *out_6472387792798353966) {
  h_14(state, unused, out_6472387792798353966);
}
void pose_H_14(double *state, double *unused, double *out_7354276273086371893) {
  H_14(state, unused, out_7354276273086371893);
}
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
}

const EKF pose = {
  .name = "pose",
  .kinds = { 4, 10, 13, 14 },
  .feature_kinds = {  },
  .f_fun = pose_f_fun,
  .F_fun = pose_F_fun,
  .err_fun = pose_err_fun,
  .inv_err_fun = pose_inv_err_fun,
  .H_mod_fun = pose_H_mod_fun,
  .predict = pose_predict,
  .hs = {
    { 4, pose_h_4 },
    { 10, pose_h_10 },
    { 13, pose_h_13 },
    { 14, pose_h_14 },
  },
  .Hs = {
    { 4, pose_H_4 },
    { 10, pose_H_10 },
    { 13, pose_H_13 },
    { 14, pose_H_14 },
  },
  .updates = {
    { 4, pose_update_4 },
    { 10, pose_update_10 },
    { 13, pose_update_13 },
    { 14, pose_update_14 },
  },
  .Hes = {
  },
  .sets = {
  },
  .extra_routines = {
  },
};

ekf_lib_init(pose)
