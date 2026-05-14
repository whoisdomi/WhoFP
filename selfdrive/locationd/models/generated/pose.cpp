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
void err_fun(double *nom_x, double *delta_x, double *out_4095137122478409328) {
   out_4095137122478409328[0] = delta_x[0] + nom_x[0];
   out_4095137122478409328[1] = delta_x[1] + nom_x[1];
   out_4095137122478409328[2] = delta_x[2] + nom_x[2];
   out_4095137122478409328[3] = delta_x[3] + nom_x[3];
   out_4095137122478409328[4] = delta_x[4] + nom_x[4];
   out_4095137122478409328[5] = delta_x[5] + nom_x[5];
   out_4095137122478409328[6] = delta_x[6] + nom_x[6];
   out_4095137122478409328[7] = delta_x[7] + nom_x[7];
   out_4095137122478409328[8] = delta_x[8] + nom_x[8];
   out_4095137122478409328[9] = delta_x[9] + nom_x[9];
   out_4095137122478409328[10] = delta_x[10] + nom_x[10];
   out_4095137122478409328[11] = delta_x[11] + nom_x[11];
   out_4095137122478409328[12] = delta_x[12] + nom_x[12];
   out_4095137122478409328[13] = delta_x[13] + nom_x[13];
   out_4095137122478409328[14] = delta_x[14] + nom_x[14];
   out_4095137122478409328[15] = delta_x[15] + nom_x[15];
   out_4095137122478409328[16] = delta_x[16] + nom_x[16];
   out_4095137122478409328[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_698683085994115938) {
   out_698683085994115938[0] = -nom_x[0] + true_x[0];
   out_698683085994115938[1] = -nom_x[1] + true_x[1];
   out_698683085994115938[2] = -nom_x[2] + true_x[2];
   out_698683085994115938[3] = -nom_x[3] + true_x[3];
   out_698683085994115938[4] = -nom_x[4] + true_x[4];
   out_698683085994115938[5] = -nom_x[5] + true_x[5];
   out_698683085994115938[6] = -nom_x[6] + true_x[6];
   out_698683085994115938[7] = -nom_x[7] + true_x[7];
   out_698683085994115938[8] = -nom_x[8] + true_x[8];
   out_698683085994115938[9] = -nom_x[9] + true_x[9];
   out_698683085994115938[10] = -nom_x[10] + true_x[10];
   out_698683085994115938[11] = -nom_x[11] + true_x[11];
   out_698683085994115938[12] = -nom_x[12] + true_x[12];
   out_698683085994115938[13] = -nom_x[13] + true_x[13];
   out_698683085994115938[14] = -nom_x[14] + true_x[14];
   out_698683085994115938[15] = -nom_x[15] + true_x[15];
   out_698683085994115938[16] = -nom_x[16] + true_x[16];
   out_698683085994115938[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_2509686983776334379) {
   out_2509686983776334379[0] = 1.0;
   out_2509686983776334379[1] = 0.0;
   out_2509686983776334379[2] = 0.0;
   out_2509686983776334379[3] = 0.0;
   out_2509686983776334379[4] = 0.0;
   out_2509686983776334379[5] = 0.0;
   out_2509686983776334379[6] = 0.0;
   out_2509686983776334379[7] = 0.0;
   out_2509686983776334379[8] = 0.0;
   out_2509686983776334379[9] = 0.0;
   out_2509686983776334379[10] = 0.0;
   out_2509686983776334379[11] = 0.0;
   out_2509686983776334379[12] = 0.0;
   out_2509686983776334379[13] = 0.0;
   out_2509686983776334379[14] = 0.0;
   out_2509686983776334379[15] = 0.0;
   out_2509686983776334379[16] = 0.0;
   out_2509686983776334379[17] = 0.0;
   out_2509686983776334379[18] = 0.0;
   out_2509686983776334379[19] = 1.0;
   out_2509686983776334379[20] = 0.0;
   out_2509686983776334379[21] = 0.0;
   out_2509686983776334379[22] = 0.0;
   out_2509686983776334379[23] = 0.0;
   out_2509686983776334379[24] = 0.0;
   out_2509686983776334379[25] = 0.0;
   out_2509686983776334379[26] = 0.0;
   out_2509686983776334379[27] = 0.0;
   out_2509686983776334379[28] = 0.0;
   out_2509686983776334379[29] = 0.0;
   out_2509686983776334379[30] = 0.0;
   out_2509686983776334379[31] = 0.0;
   out_2509686983776334379[32] = 0.0;
   out_2509686983776334379[33] = 0.0;
   out_2509686983776334379[34] = 0.0;
   out_2509686983776334379[35] = 0.0;
   out_2509686983776334379[36] = 0.0;
   out_2509686983776334379[37] = 0.0;
   out_2509686983776334379[38] = 1.0;
   out_2509686983776334379[39] = 0.0;
   out_2509686983776334379[40] = 0.0;
   out_2509686983776334379[41] = 0.0;
   out_2509686983776334379[42] = 0.0;
   out_2509686983776334379[43] = 0.0;
   out_2509686983776334379[44] = 0.0;
   out_2509686983776334379[45] = 0.0;
   out_2509686983776334379[46] = 0.0;
   out_2509686983776334379[47] = 0.0;
   out_2509686983776334379[48] = 0.0;
   out_2509686983776334379[49] = 0.0;
   out_2509686983776334379[50] = 0.0;
   out_2509686983776334379[51] = 0.0;
   out_2509686983776334379[52] = 0.0;
   out_2509686983776334379[53] = 0.0;
   out_2509686983776334379[54] = 0.0;
   out_2509686983776334379[55] = 0.0;
   out_2509686983776334379[56] = 0.0;
   out_2509686983776334379[57] = 1.0;
   out_2509686983776334379[58] = 0.0;
   out_2509686983776334379[59] = 0.0;
   out_2509686983776334379[60] = 0.0;
   out_2509686983776334379[61] = 0.0;
   out_2509686983776334379[62] = 0.0;
   out_2509686983776334379[63] = 0.0;
   out_2509686983776334379[64] = 0.0;
   out_2509686983776334379[65] = 0.0;
   out_2509686983776334379[66] = 0.0;
   out_2509686983776334379[67] = 0.0;
   out_2509686983776334379[68] = 0.0;
   out_2509686983776334379[69] = 0.0;
   out_2509686983776334379[70] = 0.0;
   out_2509686983776334379[71] = 0.0;
   out_2509686983776334379[72] = 0.0;
   out_2509686983776334379[73] = 0.0;
   out_2509686983776334379[74] = 0.0;
   out_2509686983776334379[75] = 0.0;
   out_2509686983776334379[76] = 1.0;
   out_2509686983776334379[77] = 0.0;
   out_2509686983776334379[78] = 0.0;
   out_2509686983776334379[79] = 0.0;
   out_2509686983776334379[80] = 0.0;
   out_2509686983776334379[81] = 0.0;
   out_2509686983776334379[82] = 0.0;
   out_2509686983776334379[83] = 0.0;
   out_2509686983776334379[84] = 0.0;
   out_2509686983776334379[85] = 0.0;
   out_2509686983776334379[86] = 0.0;
   out_2509686983776334379[87] = 0.0;
   out_2509686983776334379[88] = 0.0;
   out_2509686983776334379[89] = 0.0;
   out_2509686983776334379[90] = 0.0;
   out_2509686983776334379[91] = 0.0;
   out_2509686983776334379[92] = 0.0;
   out_2509686983776334379[93] = 0.0;
   out_2509686983776334379[94] = 0.0;
   out_2509686983776334379[95] = 1.0;
   out_2509686983776334379[96] = 0.0;
   out_2509686983776334379[97] = 0.0;
   out_2509686983776334379[98] = 0.0;
   out_2509686983776334379[99] = 0.0;
   out_2509686983776334379[100] = 0.0;
   out_2509686983776334379[101] = 0.0;
   out_2509686983776334379[102] = 0.0;
   out_2509686983776334379[103] = 0.0;
   out_2509686983776334379[104] = 0.0;
   out_2509686983776334379[105] = 0.0;
   out_2509686983776334379[106] = 0.0;
   out_2509686983776334379[107] = 0.0;
   out_2509686983776334379[108] = 0.0;
   out_2509686983776334379[109] = 0.0;
   out_2509686983776334379[110] = 0.0;
   out_2509686983776334379[111] = 0.0;
   out_2509686983776334379[112] = 0.0;
   out_2509686983776334379[113] = 0.0;
   out_2509686983776334379[114] = 1.0;
   out_2509686983776334379[115] = 0.0;
   out_2509686983776334379[116] = 0.0;
   out_2509686983776334379[117] = 0.0;
   out_2509686983776334379[118] = 0.0;
   out_2509686983776334379[119] = 0.0;
   out_2509686983776334379[120] = 0.0;
   out_2509686983776334379[121] = 0.0;
   out_2509686983776334379[122] = 0.0;
   out_2509686983776334379[123] = 0.0;
   out_2509686983776334379[124] = 0.0;
   out_2509686983776334379[125] = 0.0;
   out_2509686983776334379[126] = 0.0;
   out_2509686983776334379[127] = 0.0;
   out_2509686983776334379[128] = 0.0;
   out_2509686983776334379[129] = 0.0;
   out_2509686983776334379[130] = 0.0;
   out_2509686983776334379[131] = 0.0;
   out_2509686983776334379[132] = 0.0;
   out_2509686983776334379[133] = 1.0;
   out_2509686983776334379[134] = 0.0;
   out_2509686983776334379[135] = 0.0;
   out_2509686983776334379[136] = 0.0;
   out_2509686983776334379[137] = 0.0;
   out_2509686983776334379[138] = 0.0;
   out_2509686983776334379[139] = 0.0;
   out_2509686983776334379[140] = 0.0;
   out_2509686983776334379[141] = 0.0;
   out_2509686983776334379[142] = 0.0;
   out_2509686983776334379[143] = 0.0;
   out_2509686983776334379[144] = 0.0;
   out_2509686983776334379[145] = 0.0;
   out_2509686983776334379[146] = 0.0;
   out_2509686983776334379[147] = 0.0;
   out_2509686983776334379[148] = 0.0;
   out_2509686983776334379[149] = 0.0;
   out_2509686983776334379[150] = 0.0;
   out_2509686983776334379[151] = 0.0;
   out_2509686983776334379[152] = 1.0;
   out_2509686983776334379[153] = 0.0;
   out_2509686983776334379[154] = 0.0;
   out_2509686983776334379[155] = 0.0;
   out_2509686983776334379[156] = 0.0;
   out_2509686983776334379[157] = 0.0;
   out_2509686983776334379[158] = 0.0;
   out_2509686983776334379[159] = 0.0;
   out_2509686983776334379[160] = 0.0;
   out_2509686983776334379[161] = 0.0;
   out_2509686983776334379[162] = 0.0;
   out_2509686983776334379[163] = 0.0;
   out_2509686983776334379[164] = 0.0;
   out_2509686983776334379[165] = 0.0;
   out_2509686983776334379[166] = 0.0;
   out_2509686983776334379[167] = 0.0;
   out_2509686983776334379[168] = 0.0;
   out_2509686983776334379[169] = 0.0;
   out_2509686983776334379[170] = 0.0;
   out_2509686983776334379[171] = 1.0;
   out_2509686983776334379[172] = 0.0;
   out_2509686983776334379[173] = 0.0;
   out_2509686983776334379[174] = 0.0;
   out_2509686983776334379[175] = 0.0;
   out_2509686983776334379[176] = 0.0;
   out_2509686983776334379[177] = 0.0;
   out_2509686983776334379[178] = 0.0;
   out_2509686983776334379[179] = 0.0;
   out_2509686983776334379[180] = 0.0;
   out_2509686983776334379[181] = 0.0;
   out_2509686983776334379[182] = 0.0;
   out_2509686983776334379[183] = 0.0;
   out_2509686983776334379[184] = 0.0;
   out_2509686983776334379[185] = 0.0;
   out_2509686983776334379[186] = 0.0;
   out_2509686983776334379[187] = 0.0;
   out_2509686983776334379[188] = 0.0;
   out_2509686983776334379[189] = 0.0;
   out_2509686983776334379[190] = 1.0;
   out_2509686983776334379[191] = 0.0;
   out_2509686983776334379[192] = 0.0;
   out_2509686983776334379[193] = 0.0;
   out_2509686983776334379[194] = 0.0;
   out_2509686983776334379[195] = 0.0;
   out_2509686983776334379[196] = 0.0;
   out_2509686983776334379[197] = 0.0;
   out_2509686983776334379[198] = 0.0;
   out_2509686983776334379[199] = 0.0;
   out_2509686983776334379[200] = 0.0;
   out_2509686983776334379[201] = 0.0;
   out_2509686983776334379[202] = 0.0;
   out_2509686983776334379[203] = 0.0;
   out_2509686983776334379[204] = 0.0;
   out_2509686983776334379[205] = 0.0;
   out_2509686983776334379[206] = 0.0;
   out_2509686983776334379[207] = 0.0;
   out_2509686983776334379[208] = 0.0;
   out_2509686983776334379[209] = 1.0;
   out_2509686983776334379[210] = 0.0;
   out_2509686983776334379[211] = 0.0;
   out_2509686983776334379[212] = 0.0;
   out_2509686983776334379[213] = 0.0;
   out_2509686983776334379[214] = 0.0;
   out_2509686983776334379[215] = 0.0;
   out_2509686983776334379[216] = 0.0;
   out_2509686983776334379[217] = 0.0;
   out_2509686983776334379[218] = 0.0;
   out_2509686983776334379[219] = 0.0;
   out_2509686983776334379[220] = 0.0;
   out_2509686983776334379[221] = 0.0;
   out_2509686983776334379[222] = 0.0;
   out_2509686983776334379[223] = 0.0;
   out_2509686983776334379[224] = 0.0;
   out_2509686983776334379[225] = 0.0;
   out_2509686983776334379[226] = 0.0;
   out_2509686983776334379[227] = 0.0;
   out_2509686983776334379[228] = 1.0;
   out_2509686983776334379[229] = 0.0;
   out_2509686983776334379[230] = 0.0;
   out_2509686983776334379[231] = 0.0;
   out_2509686983776334379[232] = 0.0;
   out_2509686983776334379[233] = 0.0;
   out_2509686983776334379[234] = 0.0;
   out_2509686983776334379[235] = 0.0;
   out_2509686983776334379[236] = 0.0;
   out_2509686983776334379[237] = 0.0;
   out_2509686983776334379[238] = 0.0;
   out_2509686983776334379[239] = 0.0;
   out_2509686983776334379[240] = 0.0;
   out_2509686983776334379[241] = 0.0;
   out_2509686983776334379[242] = 0.0;
   out_2509686983776334379[243] = 0.0;
   out_2509686983776334379[244] = 0.0;
   out_2509686983776334379[245] = 0.0;
   out_2509686983776334379[246] = 0.0;
   out_2509686983776334379[247] = 1.0;
   out_2509686983776334379[248] = 0.0;
   out_2509686983776334379[249] = 0.0;
   out_2509686983776334379[250] = 0.0;
   out_2509686983776334379[251] = 0.0;
   out_2509686983776334379[252] = 0.0;
   out_2509686983776334379[253] = 0.0;
   out_2509686983776334379[254] = 0.0;
   out_2509686983776334379[255] = 0.0;
   out_2509686983776334379[256] = 0.0;
   out_2509686983776334379[257] = 0.0;
   out_2509686983776334379[258] = 0.0;
   out_2509686983776334379[259] = 0.0;
   out_2509686983776334379[260] = 0.0;
   out_2509686983776334379[261] = 0.0;
   out_2509686983776334379[262] = 0.0;
   out_2509686983776334379[263] = 0.0;
   out_2509686983776334379[264] = 0.0;
   out_2509686983776334379[265] = 0.0;
   out_2509686983776334379[266] = 1.0;
   out_2509686983776334379[267] = 0.0;
   out_2509686983776334379[268] = 0.0;
   out_2509686983776334379[269] = 0.0;
   out_2509686983776334379[270] = 0.0;
   out_2509686983776334379[271] = 0.0;
   out_2509686983776334379[272] = 0.0;
   out_2509686983776334379[273] = 0.0;
   out_2509686983776334379[274] = 0.0;
   out_2509686983776334379[275] = 0.0;
   out_2509686983776334379[276] = 0.0;
   out_2509686983776334379[277] = 0.0;
   out_2509686983776334379[278] = 0.0;
   out_2509686983776334379[279] = 0.0;
   out_2509686983776334379[280] = 0.0;
   out_2509686983776334379[281] = 0.0;
   out_2509686983776334379[282] = 0.0;
   out_2509686983776334379[283] = 0.0;
   out_2509686983776334379[284] = 0.0;
   out_2509686983776334379[285] = 1.0;
   out_2509686983776334379[286] = 0.0;
   out_2509686983776334379[287] = 0.0;
   out_2509686983776334379[288] = 0.0;
   out_2509686983776334379[289] = 0.0;
   out_2509686983776334379[290] = 0.0;
   out_2509686983776334379[291] = 0.0;
   out_2509686983776334379[292] = 0.0;
   out_2509686983776334379[293] = 0.0;
   out_2509686983776334379[294] = 0.0;
   out_2509686983776334379[295] = 0.0;
   out_2509686983776334379[296] = 0.0;
   out_2509686983776334379[297] = 0.0;
   out_2509686983776334379[298] = 0.0;
   out_2509686983776334379[299] = 0.0;
   out_2509686983776334379[300] = 0.0;
   out_2509686983776334379[301] = 0.0;
   out_2509686983776334379[302] = 0.0;
   out_2509686983776334379[303] = 0.0;
   out_2509686983776334379[304] = 1.0;
   out_2509686983776334379[305] = 0.0;
   out_2509686983776334379[306] = 0.0;
   out_2509686983776334379[307] = 0.0;
   out_2509686983776334379[308] = 0.0;
   out_2509686983776334379[309] = 0.0;
   out_2509686983776334379[310] = 0.0;
   out_2509686983776334379[311] = 0.0;
   out_2509686983776334379[312] = 0.0;
   out_2509686983776334379[313] = 0.0;
   out_2509686983776334379[314] = 0.0;
   out_2509686983776334379[315] = 0.0;
   out_2509686983776334379[316] = 0.0;
   out_2509686983776334379[317] = 0.0;
   out_2509686983776334379[318] = 0.0;
   out_2509686983776334379[319] = 0.0;
   out_2509686983776334379[320] = 0.0;
   out_2509686983776334379[321] = 0.0;
   out_2509686983776334379[322] = 0.0;
   out_2509686983776334379[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_3854505650261104831) {
   out_3854505650261104831[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_3854505650261104831[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_3854505650261104831[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_3854505650261104831[3] = dt*state[12] + state[3];
   out_3854505650261104831[4] = dt*state[13] + state[4];
   out_3854505650261104831[5] = dt*state[14] + state[5];
   out_3854505650261104831[6] = state[6];
   out_3854505650261104831[7] = state[7];
   out_3854505650261104831[8] = state[8];
   out_3854505650261104831[9] = state[9];
   out_3854505650261104831[10] = state[10];
   out_3854505650261104831[11] = state[11];
   out_3854505650261104831[12] = state[12];
   out_3854505650261104831[13] = state[13];
   out_3854505650261104831[14] = state[14];
   out_3854505650261104831[15] = state[15];
   out_3854505650261104831[16] = state[16];
   out_3854505650261104831[17] = state[17];
}
void F_fun(double *state, double dt, double *out_8250233670229774683) {
   out_8250233670229774683[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_8250233670229774683[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_8250233670229774683[2] = 0;
   out_8250233670229774683[3] = 0;
   out_8250233670229774683[4] = 0;
   out_8250233670229774683[5] = 0;
   out_8250233670229774683[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_8250233670229774683[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_8250233670229774683[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_8250233670229774683[9] = 0;
   out_8250233670229774683[10] = 0;
   out_8250233670229774683[11] = 0;
   out_8250233670229774683[12] = 0;
   out_8250233670229774683[13] = 0;
   out_8250233670229774683[14] = 0;
   out_8250233670229774683[15] = 0;
   out_8250233670229774683[16] = 0;
   out_8250233670229774683[17] = 0;
   out_8250233670229774683[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_8250233670229774683[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_8250233670229774683[20] = 0;
   out_8250233670229774683[21] = 0;
   out_8250233670229774683[22] = 0;
   out_8250233670229774683[23] = 0;
   out_8250233670229774683[24] = 0;
   out_8250233670229774683[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_8250233670229774683[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_8250233670229774683[27] = 0;
   out_8250233670229774683[28] = 0;
   out_8250233670229774683[29] = 0;
   out_8250233670229774683[30] = 0;
   out_8250233670229774683[31] = 0;
   out_8250233670229774683[32] = 0;
   out_8250233670229774683[33] = 0;
   out_8250233670229774683[34] = 0;
   out_8250233670229774683[35] = 0;
   out_8250233670229774683[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_8250233670229774683[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_8250233670229774683[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_8250233670229774683[39] = 0;
   out_8250233670229774683[40] = 0;
   out_8250233670229774683[41] = 0;
   out_8250233670229774683[42] = 0;
   out_8250233670229774683[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_8250233670229774683[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_8250233670229774683[45] = 0;
   out_8250233670229774683[46] = 0;
   out_8250233670229774683[47] = 0;
   out_8250233670229774683[48] = 0;
   out_8250233670229774683[49] = 0;
   out_8250233670229774683[50] = 0;
   out_8250233670229774683[51] = 0;
   out_8250233670229774683[52] = 0;
   out_8250233670229774683[53] = 0;
   out_8250233670229774683[54] = 0;
   out_8250233670229774683[55] = 0;
   out_8250233670229774683[56] = 0;
   out_8250233670229774683[57] = 1;
   out_8250233670229774683[58] = 0;
   out_8250233670229774683[59] = 0;
   out_8250233670229774683[60] = 0;
   out_8250233670229774683[61] = 0;
   out_8250233670229774683[62] = 0;
   out_8250233670229774683[63] = 0;
   out_8250233670229774683[64] = 0;
   out_8250233670229774683[65] = 0;
   out_8250233670229774683[66] = dt;
   out_8250233670229774683[67] = 0;
   out_8250233670229774683[68] = 0;
   out_8250233670229774683[69] = 0;
   out_8250233670229774683[70] = 0;
   out_8250233670229774683[71] = 0;
   out_8250233670229774683[72] = 0;
   out_8250233670229774683[73] = 0;
   out_8250233670229774683[74] = 0;
   out_8250233670229774683[75] = 0;
   out_8250233670229774683[76] = 1;
   out_8250233670229774683[77] = 0;
   out_8250233670229774683[78] = 0;
   out_8250233670229774683[79] = 0;
   out_8250233670229774683[80] = 0;
   out_8250233670229774683[81] = 0;
   out_8250233670229774683[82] = 0;
   out_8250233670229774683[83] = 0;
   out_8250233670229774683[84] = 0;
   out_8250233670229774683[85] = dt;
   out_8250233670229774683[86] = 0;
   out_8250233670229774683[87] = 0;
   out_8250233670229774683[88] = 0;
   out_8250233670229774683[89] = 0;
   out_8250233670229774683[90] = 0;
   out_8250233670229774683[91] = 0;
   out_8250233670229774683[92] = 0;
   out_8250233670229774683[93] = 0;
   out_8250233670229774683[94] = 0;
   out_8250233670229774683[95] = 1;
   out_8250233670229774683[96] = 0;
   out_8250233670229774683[97] = 0;
   out_8250233670229774683[98] = 0;
   out_8250233670229774683[99] = 0;
   out_8250233670229774683[100] = 0;
   out_8250233670229774683[101] = 0;
   out_8250233670229774683[102] = 0;
   out_8250233670229774683[103] = 0;
   out_8250233670229774683[104] = dt;
   out_8250233670229774683[105] = 0;
   out_8250233670229774683[106] = 0;
   out_8250233670229774683[107] = 0;
   out_8250233670229774683[108] = 0;
   out_8250233670229774683[109] = 0;
   out_8250233670229774683[110] = 0;
   out_8250233670229774683[111] = 0;
   out_8250233670229774683[112] = 0;
   out_8250233670229774683[113] = 0;
   out_8250233670229774683[114] = 1;
   out_8250233670229774683[115] = 0;
   out_8250233670229774683[116] = 0;
   out_8250233670229774683[117] = 0;
   out_8250233670229774683[118] = 0;
   out_8250233670229774683[119] = 0;
   out_8250233670229774683[120] = 0;
   out_8250233670229774683[121] = 0;
   out_8250233670229774683[122] = 0;
   out_8250233670229774683[123] = 0;
   out_8250233670229774683[124] = 0;
   out_8250233670229774683[125] = 0;
   out_8250233670229774683[126] = 0;
   out_8250233670229774683[127] = 0;
   out_8250233670229774683[128] = 0;
   out_8250233670229774683[129] = 0;
   out_8250233670229774683[130] = 0;
   out_8250233670229774683[131] = 0;
   out_8250233670229774683[132] = 0;
   out_8250233670229774683[133] = 1;
   out_8250233670229774683[134] = 0;
   out_8250233670229774683[135] = 0;
   out_8250233670229774683[136] = 0;
   out_8250233670229774683[137] = 0;
   out_8250233670229774683[138] = 0;
   out_8250233670229774683[139] = 0;
   out_8250233670229774683[140] = 0;
   out_8250233670229774683[141] = 0;
   out_8250233670229774683[142] = 0;
   out_8250233670229774683[143] = 0;
   out_8250233670229774683[144] = 0;
   out_8250233670229774683[145] = 0;
   out_8250233670229774683[146] = 0;
   out_8250233670229774683[147] = 0;
   out_8250233670229774683[148] = 0;
   out_8250233670229774683[149] = 0;
   out_8250233670229774683[150] = 0;
   out_8250233670229774683[151] = 0;
   out_8250233670229774683[152] = 1;
   out_8250233670229774683[153] = 0;
   out_8250233670229774683[154] = 0;
   out_8250233670229774683[155] = 0;
   out_8250233670229774683[156] = 0;
   out_8250233670229774683[157] = 0;
   out_8250233670229774683[158] = 0;
   out_8250233670229774683[159] = 0;
   out_8250233670229774683[160] = 0;
   out_8250233670229774683[161] = 0;
   out_8250233670229774683[162] = 0;
   out_8250233670229774683[163] = 0;
   out_8250233670229774683[164] = 0;
   out_8250233670229774683[165] = 0;
   out_8250233670229774683[166] = 0;
   out_8250233670229774683[167] = 0;
   out_8250233670229774683[168] = 0;
   out_8250233670229774683[169] = 0;
   out_8250233670229774683[170] = 0;
   out_8250233670229774683[171] = 1;
   out_8250233670229774683[172] = 0;
   out_8250233670229774683[173] = 0;
   out_8250233670229774683[174] = 0;
   out_8250233670229774683[175] = 0;
   out_8250233670229774683[176] = 0;
   out_8250233670229774683[177] = 0;
   out_8250233670229774683[178] = 0;
   out_8250233670229774683[179] = 0;
   out_8250233670229774683[180] = 0;
   out_8250233670229774683[181] = 0;
   out_8250233670229774683[182] = 0;
   out_8250233670229774683[183] = 0;
   out_8250233670229774683[184] = 0;
   out_8250233670229774683[185] = 0;
   out_8250233670229774683[186] = 0;
   out_8250233670229774683[187] = 0;
   out_8250233670229774683[188] = 0;
   out_8250233670229774683[189] = 0;
   out_8250233670229774683[190] = 1;
   out_8250233670229774683[191] = 0;
   out_8250233670229774683[192] = 0;
   out_8250233670229774683[193] = 0;
   out_8250233670229774683[194] = 0;
   out_8250233670229774683[195] = 0;
   out_8250233670229774683[196] = 0;
   out_8250233670229774683[197] = 0;
   out_8250233670229774683[198] = 0;
   out_8250233670229774683[199] = 0;
   out_8250233670229774683[200] = 0;
   out_8250233670229774683[201] = 0;
   out_8250233670229774683[202] = 0;
   out_8250233670229774683[203] = 0;
   out_8250233670229774683[204] = 0;
   out_8250233670229774683[205] = 0;
   out_8250233670229774683[206] = 0;
   out_8250233670229774683[207] = 0;
   out_8250233670229774683[208] = 0;
   out_8250233670229774683[209] = 1;
   out_8250233670229774683[210] = 0;
   out_8250233670229774683[211] = 0;
   out_8250233670229774683[212] = 0;
   out_8250233670229774683[213] = 0;
   out_8250233670229774683[214] = 0;
   out_8250233670229774683[215] = 0;
   out_8250233670229774683[216] = 0;
   out_8250233670229774683[217] = 0;
   out_8250233670229774683[218] = 0;
   out_8250233670229774683[219] = 0;
   out_8250233670229774683[220] = 0;
   out_8250233670229774683[221] = 0;
   out_8250233670229774683[222] = 0;
   out_8250233670229774683[223] = 0;
   out_8250233670229774683[224] = 0;
   out_8250233670229774683[225] = 0;
   out_8250233670229774683[226] = 0;
   out_8250233670229774683[227] = 0;
   out_8250233670229774683[228] = 1;
   out_8250233670229774683[229] = 0;
   out_8250233670229774683[230] = 0;
   out_8250233670229774683[231] = 0;
   out_8250233670229774683[232] = 0;
   out_8250233670229774683[233] = 0;
   out_8250233670229774683[234] = 0;
   out_8250233670229774683[235] = 0;
   out_8250233670229774683[236] = 0;
   out_8250233670229774683[237] = 0;
   out_8250233670229774683[238] = 0;
   out_8250233670229774683[239] = 0;
   out_8250233670229774683[240] = 0;
   out_8250233670229774683[241] = 0;
   out_8250233670229774683[242] = 0;
   out_8250233670229774683[243] = 0;
   out_8250233670229774683[244] = 0;
   out_8250233670229774683[245] = 0;
   out_8250233670229774683[246] = 0;
   out_8250233670229774683[247] = 1;
   out_8250233670229774683[248] = 0;
   out_8250233670229774683[249] = 0;
   out_8250233670229774683[250] = 0;
   out_8250233670229774683[251] = 0;
   out_8250233670229774683[252] = 0;
   out_8250233670229774683[253] = 0;
   out_8250233670229774683[254] = 0;
   out_8250233670229774683[255] = 0;
   out_8250233670229774683[256] = 0;
   out_8250233670229774683[257] = 0;
   out_8250233670229774683[258] = 0;
   out_8250233670229774683[259] = 0;
   out_8250233670229774683[260] = 0;
   out_8250233670229774683[261] = 0;
   out_8250233670229774683[262] = 0;
   out_8250233670229774683[263] = 0;
   out_8250233670229774683[264] = 0;
   out_8250233670229774683[265] = 0;
   out_8250233670229774683[266] = 1;
   out_8250233670229774683[267] = 0;
   out_8250233670229774683[268] = 0;
   out_8250233670229774683[269] = 0;
   out_8250233670229774683[270] = 0;
   out_8250233670229774683[271] = 0;
   out_8250233670229774683[272] = 0;
   out_8250233670229774683[273] = 0;
   out_8250233670229774683[274] = 0;
   out_8250233670229774683[275] = 0;
   out_8250233670229774683[276] = 0;
   out_8250233670229774683[277] = 0;
   out_8250233670229774683[278] = 0;
   out_8250233670229774683[279] = 0;
   out_8250233670229774683[280] = 0;
   out_8250233670229774683[281] = 0;
   out_8250233670229774683[282] = 0;
   out_8250233670229774683[283] = 0;
   out_8250233670229774683[284] = 0;
   out_8250233670229774683[285] = 1;
   out_8250233670229774683[286] = 0;
   out_8250233670229774683[287] = 0;
   out_8250233670229774683[288] = 0;
   out_8250233670229774683[289] = 0;
   out_8250233670229774683[290] = 0;
   out_8250233670229774683[291] = 0;
   out_8250233670229774683[292] = 0;
   out_8250233670229774683[293] = 0;
   out_8250233670229774683[294] = 0;
   out_8250233670229774683[295] = 0;
   out_8250233670229774683[296] = 0;
   out_8250233670229774683[297] = 0;
   out_8250233670229774683[298] = 0;
   out_8250233670229774683[299] = 0;
   out_8250233670229774683[300] = 0;
   out_8250233670229774683[301] = 0;
   out_8250233670229774683[302] = 0;
   out_8250233670229774683[303] = 0;
   out_8250233670229774683[304] = 1;
   out_8250233670229774683[305] = 0;
   out_8250233670229774683[306] = 0;
   out_8250233670229774683[307] = 0;
   out_8250233670229774683[308] = 0;
   out_8250233670229774683[309] = 0;
   out_8250233670229774683[310] = 0;
   out_8250233670229774683[311] = 0;
   out_8250233670229774683[312] = 0;
   out_8250233670229774683[313] = 0;
   out_8250233670229774683[314] = 0;
   out_8250233670229774683[315] = 0;
   out_8250233670229774683[316] = 0;
   out_8250233670229774683[317] = 0;
   out_8250233670229774683[318] = 0;
   out_8250233670229774683[319] = 0;
   out_8250233670229774683[320] = 0;
   out_8250233670229774683[321] = 0;
   out_8250233670229774683[322] = 0;
   out_8250233670229774683[323] = 1;
}
void h_4(double *state, double *unused, double *out_3198903065177037567) {
   out_3198903065177037567[0] = state[6] + state[9];
   out_3198903065177037567[1] = state[7] + state[10];
   out_3198903065177037567[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_5661603524506085814) {
   out_5661603524506085814[0] = 0;
   out_5661603524506085814[1] = 0;
   out_5661603524506085814[2] = 0;
   out_5661603524506085814[3] = 0;
   out_5661603524506085814[4] = 0;
   out_5661603524506085814[5] = 0;
   out_5661603524506085814[6] = 1;
   out_5661603524506085814[7] = 0;
   out_5661603524506085814[8] = 0;
   out_5661603524506085814[9] = 1;
   out_5661603524506085814[10] = 0;
   out_5661603524506085814[11] = 0;
   out_5661603524506085814[12] = 0;
   out_5661603524506085814[13] = 0;
   out_5661603524506085814[14] = 0;
   out_5661603524506085814[15] = 0;
   out_5661603524506085814[16] = 0;
   out_5661603524506085814[17] = 0;
   out_5661603524506085814[18] = 0;
   out_5661603524506085814[19] = 0;
   out_5661603524506085814[20] = 0;
   out_5661603524506085814[21] = 0;
   out_5661603524506085814[22] = 0;
   out_5661603524506085814[23] = 0;
   out_5661603524506085814[24] = 0;
   out_5661603524506085814[25] = 1;
   out_5661603524506085814[26] = 0;
   out_5661603524506085814[27] = 0;
   out_5661603524506085814[28] = 1;
   out_5661603524506085814[29] = 0;
   out_5661603524506085814[30] = 0;
   out_5661603524506085814[31] = 0;
   out_5661603524506085814[32] = 0;
   out_5661603524506085814[33] = 0;
   out_5661603524506085814[34] = 0;
   out_5661603524506085814[35] = 0;
   out_5661603524506085814[36] = 0;
   out_5661603524506085814[37] = 0;
   out_5661603524506085814[38] = 0;
   out_5661603524506085814[39] = 0;
   out_5661603524506085814[40] = 0;
   out_5661603524506085814[41] = 0;
   out_5661603524506085814[42] = 0;
   out_5661603524506085814[43] = 0;
   out_5661603524506085814[44] = 1;
   out_5661603524506085814[45] = 0;
   out_5661603524506085814[46] = 0;
   out_5661603524506085814[47] = 1;
   out_5661603524506085814[48] = 0;
   out_5661603524506085814[49] = 0;
   out_5661603524506085814[50] = 0;
   out_5661603524506085814[51] = 0;
   out_5661603524506085814[52] = 0;
   out_5661603524506085814[53] = 0;
}
void h_10(double *state, double *unused, double *out_6914485878183044840) {
   out_6914485878183044840[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_6914485878183044840[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_6914485878183044840[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_8926024378503649402) {
   out_8926024378503649402[0] = 0;
   out_8926024378503649402[1] = 9.8100000000000005*cos(state[1]);
   out_8926024378503649402[2] = 0;
   out_8926024378503649402[3] = 0;
   out_8926024378503649402[4] = -state[8];
   out_8926024378503649402[5] = state[7];
   out_8926024378503649402[6] = 0;
   out_8926024378503649402[7] = state[5];
   out_8926024378503649402[8] = -state[4];
   out_8926024378503649402[9] = 0;
   out_8926024378503649402[10] = 0;
   out_8926024378503649402[11] = 0;
   out_8926024378503649402[12] = 1;
   out_8926024378503649402[13] = 0;
   out_8926024378503649402[14] = 0;
   out_8926024378503649402[15] = 1;
   out_8926024378503649402[16] = 0;
   out_8926024378503649402[17] = 0;
   out_8926024378503649402[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_8926024378503649402[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_8926024378503649402[20] = 0;
   out_8926024378503649402[21] = state[8];
   out_8926024378503649402[22] = 0;
   out_8926024378503649402[23] = -state[6];
   out_8926024378503649402[24] = -state[5];
   out_8926024378503649402[25] = 0;
   out_8926024378503649402[26] = state[3];
   out_8926024378503649402[27] = 0;
   out_8926024378503649402[28] = 0;
   out_8926024378503649402[29] = 0;
   out_8926024378503649402[30] = 0;
   out_8926024378503649402[31] = 1;
   out_8926024378503649402[32] = 0;
   out_8926024378503649402[33] = 0;
   out_8926024378503649402[34] = 1;
   out_8926024378503649402[35] = 0;
   out_8926024378503649402[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_8926024378503649402[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_8926024378503649402[38] = 0;
   out_8926024378503649402[39] = -state[7];
   out_8926024378503649402[40] = state[6];
   out_8926024378503649402[41] = 0;
   out_8926024378503649402[42] = state[4];
   out_8926024378503649402[43] = -state[3];
   out_8926024378503649402[44] = 0;
   out_8926024378503649402[45] = 0;
   out_8926024378503649402[46] = 0;
   out_8926024378503649402[47] = 0;
   out_8926024378503649402[48] = 0;
   out_8926024378503649402[49] = 0;
   out_8926024378503649402[50] = 1;
   out_8926024378503649402[51] = 0;
   out_8926024378503649402[52] = 0;
   out_8926024378503649402[53] = 1;
}
void h_13(double *state, double *unused, double *out_5850172911985206166) {
   out_5850172911985206166[0] = state[3];
   out_5850172911985206166[1] = state[4];
   out_5850172911985206166[2] = state[5];
}
void H_13(double *state, double *unused, double *out_2449329699173753013) {
   out_2449329699173753013[0] = 0;
   out_2449329699173753013[1] = 0;
   out_2449329699173753013[2] = 0;
   out_2449329699173753013[3] = 1;
   out_2449329699173753013[4] = 0;
   out_2449329699173753013[5] = 0;
   out_2449329699173753013[6] = 0;
   out_2449329699173753013[7] = 0;
   out_2449329699173753013[8] = 0;
   out_2449329699173753013[9] = 0;
   out_2449329699173753013[10] = 0;
   out_2449329699173753013[11] = 0;
   out_2449329699173753013[12] = 0;
   out_2449329699173753013[13] = 0;
   out_2449329699173753013[14] = 0;
   out_2449329699173753013[15] = 0;
   out_2449329699173753013[16] = 0;
   out_2449329699173753013[17] = 0;
   out_2449329699173753013[18] = 0;
   out_2449329699173753013[19] = 0;
   out_2449329699173753013[20] = 0;
   out_2449329699173753013[21] = 0;
   out_2449329699173753013[22] = 1;
   out_2449329699173753013[23] = 0;
   out_2449329699173753013[24] = 0;
   out_2449329699173753013[25] = 0;
   out_2449329699173753013[26] = 0;
   out_2449329699173753013[27] = 0;
   out_2449329699173753013[28] = 0;
   out_2449329699173753013[29] = 0;
   out_2449329699173753013[30] = 0;
   out_2449329699173753013[31] = 0;
   out_2449329699173753013[32] = 0;
   out_2449329699173753013[33] = 0;
   out_2449329699173753013[34] = 0;
   out_2449329699173753013[35] = 0;
   out_2449329699173753013[36] = 0;
   out_2449329699173753013[37] = 0;
   out_2449329699173753013[38] = 0;
   out_2449329699173753013[39] = 0;
   out_2449329699173753013[40] = 0;
   out_2449329699173753013[41] = 1;
   out_2449329699173753013[42] = 0;
   out_2449329699173753013[43] = 0;
   out_2449329699173753013[44] = 0;
   out_2449329699173753013[45] = 0;
   out_2449329699173753013[46] = 0;
   out_2449329699173753013[47] = 0;
   out_2449329699173753013[48] = 0;
   out_2449329699173753013[49] = 0;
   out_2449329699173753013[50] = 0;
   out_2449329699173753013[51] = 0;
   out_2449329699173753013[52] = 0;
   out_2449329699173753013[53] = 0;
}
void h_14(double *state, double *unused, double *out_348759912654094184) {
   out_348759912654094184[0] = state[6];
   out_348759912654094184[1] = state[7];
   out_348759912654094184[2] = state[8];
}
void H_14(double *state, double *unused, double *out_1698362668166601285) {
   out_1698362668166601285[0] = 0;
   out_1698362668166601285[1] = 0;
   out_1698362668166601285[2] = 0;
   out_1698362668166601285[3] = 0;
   out_1698362668166601285[4] = 0;
   out_1698362668166601285[5] = 0;
   out_1698362668166601285[6] = 1;
   out_1698362668166601285[7] = 0;
   out_1698362668166601285[8] = 0;
   out_1698362668166601285[9] = 0;
   out_1698362668166601285[10] = 0;
   out_1698362668166601285[11] = 0;
   out_1698362668166601285[12] = 0;
   out_1698362668166601285[13] = 0;
   out_1698362668166601285[14] = 0;
   out_1698362668166601285[15] = 0;
   out_1698362668166601285[16] = 0;
   out_1698362668166601285[17] = 0;
   out_1698362668166601285[18] = 0;
   out_1698362668166601285[19] = 0;
   out_1698362668166601285[20] = 0;
   out_1698362668166601285[21] = 0;
   out_1698362668166601285[22] = 0;
   out_1698362668166601285[23] = 0;
   out_1698362668166601285[24] = 0;
   out_1698362668166601285[25] = 1;
   out_1698362668166601285[26] = 0;
   out_1698362668166601285[27] = 0;
   out_1698362668166601285[28] = 0;
   out_1698362668166601285[29] = 0;
   out_1698362668166601285[30] = 0;
   out_1698362668166601285[31] = 0;
   out_1698362668166601285[32] = 0;
   out_1698362668166601285[33] = 0;
   out_1698362668166601285[34] = 0;
   out_1698362668166601285[35] = 0;
   out_1698362668166601285[36] = 0;
   out_1698362668166601285[37] = 0;
   out_1698362668166601285[38] = 0;
   out_1698362668166601285[39] = 0;
   out_1698362668166601285[40] = 0;
   out_1698362668166601285[41] = 0;
   out_1698362668166601285[42] = 0;
   out_1698362668166601285[43] = 0;
   out_1698362668166601285[44] = 1;
   out_1698362668166601285[45] = 0;
   out_1698362668166601285[46] = 0;
   out_1698362668166601285[47] = 0;
   out_1698362668166601285[48] = 0;
   out_1698362668166601285[49] = 0;
   out_1698362668166601285[50] = 0;
   out_1698362668166601285[51] = 0;
   out_1698362668166601285[52] = 0;
   out_1698362668166601285[53] = 0;
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
void pose_err_fun(double *nom_x, double *delta_x, double *out_4095137122478409328) {
  err_fun(nom_x, delta_x, out_4095137122478409328);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_698683085994115938) {
  inv_err_fun(nom_x, true_x, out_698683085994115938);
}
void pose_H_mod_fun(double *state, double *out_2509686983776334379) {
  H_mod_fun(state, out_2509686983776334379);
}
void pose_f_fun(double *state, double dt, double *out_3854505650261104831) {
  f_fun(state,  dt, out_3854505650261104831);
}
void pose_F_fun(double *state, double dt, double *out_8250233670229774683) {
  F_fun(state,  dt, out_8250233670229774683);
}
void pose_h_4(double *state, double *unused, double *out_3198903065177037567) {
  h_4(state, unused, out_3198903065177037567);
}
void pose_H_4(double *state, double *unused, double *out_5661603524506085814) {
  H_4(state, unused, out_5661603524506085814);
}
void pose_h_10(double *state, double *unused, double *out_6914485878183044840) {
  h_10(state, unused, out_6914485878183044840);
}
void pose_H_10(double *state, double *unused, double *out_8926024378503649402) {
  H_10(state, unused, out_8926024378503649402);
}
void pose_h_13(double *state, double *unused, double *out_5850172911985206166) {
  h_13(state, unused, out_5850172911985206166);
}
void pose_H_13(double *state, double *unused, double *out_2449329699173753013) {
  H_13(state, unused, out_2449329699173753013);
}
void pose_h_14(double *state, double *unused, double *out_348759912654094184) {
  h_14(state, unused, out_348759912654094184);
}
void pose_H_14(double *state, double *unused, double *out_1698362668166601285) {
  H_14(state, unused, out_1698362668166601285);
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
