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
void err_fun(double *nom_x, double *delta_x, double *out_2213402149788806935) {
   out_2213402149788806935[0] = delta_x[0] + nom_x[0];
   out_2213402149788806935[1] = delta_x[1] + nom_x[1];
   out_2213402149788806935[2] = delta_x[2] + nom_x[2];
   out_2213402149788806935[3] = delta_x[3] + nom_x[3];
   out_2213402149788806935[4] = delta_x[4] + nom_x[4];
   out_2213402149788806935[5] = delta_x[5] + nom_x[5];
   out_2213402149788806935[6] = delta_x[6] + nom_x[6];
   out_2213402149788806935[7] = delta_x[7] + nom_x[7];
   out_2213402149788806935[8] = delta_x[8] + nom_x[8];
   out_2213402149788806935[9] = delta_x[9] + nom_x[9];
   out_2213402149788806935[10] = delta_x[10] + nom_x[10];
   out_2213402149788806935[11] = delta_x[11] + nom_x[11];
   out_2213402149788806935[12] = delta_x[12] + nom_x[12];
   out_2213402149788806935[13] = delta_x[13] + nom_x[13];
   out_2213402149788806935[14] = delta_x[14] + nom_x[14];
   out_2213402149788806935[15] = delta_x[15] + nom_x[15];
   out_2213402149788806935[16] = delta_x[16] + nom_x[16];
   out_2213402149788806935[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_8912039620049795540) {
   out_8912039620049795540[0] = -nom_x[0] + true_x[0];
   out_8912039620049795540[1] = -nom_x[1] + true_x[1];
   out_8912039620049795540[2] = -nom_x[2] + true_x[2];
   out_8912039620049795540[3] = -nom_x[3] + true_x[3];
   out_8912039620049795540[4] = -nom_x[4] + true_x[4];
   out_8912039620049795540[5] = -nom_x[5] + true_x[5];
   out_8912039620049795540[6] = -nom_x[6] + true_x[6];
   out_8912039620049795540[7] = -nom_x[7] + true_x[7];
   out_8912039620049795540[8] = -nom_x[8] + true_x[8];
   out_8912039620049795540[9] = -nom_x[9] + true_x[9];
   out_8912039620049795540[10] = -nom_x[10] + true_x[10];
   out_8912039620049795540[11] = -nom_x[11] + true_x[11];
   out_8912039620049795540[12] = -nom_x[12] + true_x[12];
   out_8912039620049795540[13] = -nom_x[13] + true_x[13];
   out_8912039620049795540[14] = -nom_x[14] + true_x[14];
   out_8912039620049795540[15] = -nom_x[15] + true_x[15];
   out_8912039620049795540[16] = -nom_x[16] + true_x[16];
   out_8912039620049795540[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_2211145472851047127) {
   out_2211145472851047127[0] = 1.0;
   out_2211145472851047127[1] = 0.0;
   out_2211145472851047127[2] = 0.0;
   out_2211145472851047127[3] = 0.0;
   out_2211145472851047127[4] = 0.0;
   out_2211145472851047127[5] = 0.0;
   out_2211145472851047127[6] = 0.0;
   out_2211145472851047127[7] = 0.0;
   out_2211145472851047127[8] = 0.0;
   out_2211145472851047127[9] = 0.0;
   out_2211145472851047127[10] = 0.0;
   out_2211145472851047127[11] = 0.0;
   out_2211145472851047127[12] = 0.0;
   out_2211145472851047127[13] = 0.0;
   out_2211145472851047127[14] = 0.0;
   out_2211145472851047127[15] = 0.0;
   out_2211145472851047127[16] = 0.0;
   out_2211145472851047127[17] = 0.0;
   out_2211145472851047127[18] = 0.0;
   out_2211145472851047127[19] = 1.0;
   out_2211145472851047127[20] = 0.0;
   out_2211145472851047127[21] = 0.0;
   out_2211145472851047127[22] = 0.0;
   out_2211145472851047127[23] = 0.0;
   out_2211145472851047127[24] = 0.0;
   out_2211145472851047127[25] = 0.0;
   out_2211145472851047127[26] = 0.0;
   out_2211145472851047127[27] = 0.0;
   out_2211145472851047127[28] = 0.0;
   out_2211145472851047127[29] = 0.0;
   out_2211145472851047127[30] = 0.0;
   out_2211145472851047127[31] = 0.0;
   out_2211145472851047127[32] = 0.0;
   out_2211145472851047127[33] = 0.0;
   out_2211145472851047127[34] = 0.0;
   out_2211145472851047127[35] = 0.0;
   out_2211145472851047127[36] = 0.0;
   out_2211145472851047127[37] = 0.0;
   out_2211145472851047127[38] = 1.0;
   out_2211145472851047127[39] = 0.0;
   out_2211145472851047127[40] = 0.0;
   out_2211145472851047127[41] = 0.0;
   out_2211145472851047127[42] = 0.0;
   out_2211145472851047127[43] = 0.0;
   out_2211145472851047127[44] = 0.0;
   out_2211145472851047127[45] = 0.0;
   out_2211145472851047127[46] = 0.0;
   out_2211145472851047127[47] = 0.0;
   out_2211145472851047127[48] = 0.0;
   out_2211145472851047127[49] = 0.0;
   out_2211145472851047127[50] = 0.0;
   out_2211145472851047127[51] = 0.0;
   out_2211145472851047127[52] = 0.0;
   out_2211145472851047127[53] = 0.0;
   out_2211145472851047127[54] = 0.0;
   out_2211145472851047127[55] = 0.0;
   out_2211145472851047127[56] = 0.0;
   out_2211145472851047127[57] = 1.0;
   out_2211145472851047127[58] = 0.0;
   out_2211145472851047127[59] = 0.0;
   out_2211145472851047127[60] = 0.0;
   out_2211145472851047127[61] = 0.0;
   out_2211145472851047127[62] = 0.0;
   out_2211145472851047127[63] = 0.0;
   out_2211145472851047127[64] = 0.0;
   out_2211145472851047127[65] = 0.0;
   out_2211145472851047127[66] = 0.0;
   out_2211145472851047127[67] = 0.0;
   out_2211145472851047127[68] = 0.0;
   out_2211145472851047127[69] = 0.0;
   out_2211145472851047127[70] = 0.0;
   out_2211145472851047127[71] = 0.0;
   out_2211145472851047127[72] = 0.0;
   out_2211145472851047127[73] = 0.0;
   out_2211145472851047127[74] = 0.0;
   out_2211145472851047127[75] = 0.0;
   out_2211145472851047127[76] = 1.0;
   out_2211145472851047127[77] = 0.0;
   out_2211145472851047127[78] = 0.0;
   out_2211145472851047127[79] = 0.0;
   out_2211145472851047127[80] = 0.0;
   out_2211145472851047127[81] = 0.0;
   out_2211145472851047127[82] = 0.0;
   out_2211145472851047127[83] = 0.0;
   out_2211145472851047127[84] = 0.0;
   out_2211145472851047127[85] = 0.0;
   out_2211145472851047127[86] = 0.0;
   out_2211145472851047127[87] = 0.0;
   out_2211145472851047127[88] = 0.0;
   out_2211145472851047127[89] = 0.0;
   out_2211145472851047127[90] = 0.0;
   out_2211145472851047127[91] = 0.0;
   out_2211145472851047127[92] = 0.0;
   out_2211145472851047127[93] = 0.0;
   out_2211145472851047127[94] = 0.0;
   out_2211145472851047127[95] = 1.0;
   out_2211145472851047127[96] = 0.0;
   out_2211145472851047127[97] = 0.0;
   out_2211145472851047127[98] = 0.0;
   out_2211145472851047127[99] = 0.0;
   out_2211145472851047127[100] = 0.0;
   out_2211145472851047127[101] = 0.0;
   out_2211145472851047127[102] = 0.0;
   out_2211145472851047127[103] = 0.0;
   out_2211145472851047127[104] = 0.0;
   out_2211145472851047127[105] = 0.0;
   out_2211145472851047127[106] = 0.0;
   out_2211145472851047127[107] = 0.0;
   out_2211145472851047127[108] = 0.0;
   out_2211145472851047127[109] = 0.0;
   out_2211145472851047127[110] = 0.0;
   out_2211145472851047127[111] = 0.0;
   out_2211145472851047127[112] = 0.0;
   out_2211145472851047127[113] = 0.0;
   out_2211145472851047127[114] = 1.0;
   out_2211145472851047127[115] = 0.0;
   out_2211145472851047127[116] = 0.0;
   out_2211145472851047127[117] = 0.0;
   out_2211145472851047127[118] = 0.0;
   out_2211145472851047127[119] = 0.0;
   out_2211145472851047127[120] = 0.0;
   out_2211145472851047127[121] = 0.0;
   out_2211145472851047127[122] = 0.0;
   out_2211145472851047127[123] = 0.0;
   out_2211145472851047127[124] = 0.0;
   out_2211145472851047127[125] = 0.0;
   out_2211145472851047127[126] = 0.0;
   out_2211145472851047127[127] = 0.0;
   out_2211145472851047127[128] = 0.0;
   out_2211145472851047127[129] = 0.0;
   out_2211145472851047127[130] = 0.0;
   out_2211145472851047127[131] = 0.0;
   out_2211145472851047127[132] = 0.0;
   out_2211145472851047127[133] = 1.0;
   out_2211145472851047127[134] = 0.0;
   out_2211145472851047127[135] = 0.0;
   out_2211145472851047127[136] = 0.0;
   out_2211145472851047127[137] = 0.0;
   out_2211145472851047127[138] = 0.0;
   out_2211145472851047127[139] = 0.0;
   out_2211145472851047127[140] = 0.0;
   out_2211145472851047127[141] = 0.0;
   out_2211145472851047127[142] = 0.0;
   out_2211145472851047127[143] = 0.0;
   out_2211145472851047127[144] = 0.0;
   out_2211145472851047127[145] = 0.0;
   out_2211145472851047127[146] = 0.0;
   out_2211145472851047127[147] = 0.0;
   out_2211145472851047127[148] = 0.0;
   out_2211145472851047127[149] = 0.0;
   out_2211145472851047127[150] = 0.0;
   out_2211145472851047127[151] = 0.0;
   out_2211145472851047127[152] = 1.0;
   out_2211145472851047127[153] = 0.0;
   out_2211145472851047127[154] = 0.0;
   out_2211145472851047127[155] = 0.0;
   out_2211145472851047127[156] = 0.0;
   out_2211145472851047127[157] = 0.0;
   out_2211145472851047127[158] = 0.0;
   out_2211145472851047127[159] = 0.0;
   out_2211145472851047127[160] = 0.0;
   out_2211145472851047127[161] = 0.0;
   out_2211145472851047127[162] = 0.0;
   out_2211145472851047127[163] = 0.0;
   out_2211145472851047127[164] = 0.0;
   out_2211145472851047127[165] = 0.0;
   out_2211145472851047127[166] = 0.0;
   out_2211145472851047127[167] = 0.0;
   out_2211145472851047127[168] = 0.0;
   out_2211145472851047127[169] = 0.0;
   out_2211145472851047127[170] = 0.0;
   out_2211145472851047127[171] = 1.0;
   out_2211145472851047127[172] = 0.0;
   out_2211145472851047127[173] = 0.0;
   out_2211145472851047127[174] = 0.0;
   out_2211145472851047127[175] = 0.0;
   out_2211145472851047127[176] = 0.0;
   out_2211145472851047127[177] = 0.0;
   out_2211145472851047127[178] = 0.0;
   out_2211145472851047127[179] = 0.0;
   out_2211145472851047127[180] = 0.0;
   out_2211145472851047127[181] = 0.0;
   out_2211145472851047127[182] = 0.0;
   out_2211145472851047127[183] = 0.0;
   out_2211145472851047127[184] = 0.0;
   out_2211145472851047127[185] = 0.0;
   out_2211145472851047127[186] = 0.0;
   out_2211145472851047127[187] = 0.0;
   out_2211145472851047127[188] = 0.0;
   out_2211145472851047127[189] = 0.0;
   out_2211145472851047127[190] = 1.0;
   out_2211145472851047127[191] = 0.0;
   out_2211145472851047127[192] = 0.0;
   out_2211145472851047127[193] = 0.0;
   out_2211145472851047127[194] = 0.0;
   out_2211145472851047127[195] = 0.0;
   out_2211145472851047127[196] = 0.0;
   out_2211145472851047127[197] = 0.0;
   out_2211145472851047127[198] = 0.0;
   out_2211145472851047127[199] = 0.0;
   out_2211145472851047127[200] = 0.0;
   out_2211145472851047127[201] = 0.0;
   out_2211145472851047127[202] = 0.0;
   out_2211145472851047127[203] = 0.0;
   out_2211145472851047127[204] = 0.0;
   out_2211145472851047127[205] = 0.0;
   out_2211145472851047127[206] = 0.0;
   out_2211145472851047127[207] = 0.0;
   out_2211145472851047127[208] = 0.0;
   out_2211145472851047127[209] = 1.0;
   out_2211145472851047127[210] = 0.0;
   out_2211145472851047127[211] = 0.0;
   out_2211145472851047127[212] = 0.0;
   out_2211145472851047127[213] = 0.0;
   out_2211145472851047127[214] = 0.0;
   out_2211145472851047127[215] = 0.0;
   out_2211145472851047127[216] = 0.0;
   out_2211145472851047127[217] = 0.0;
   out_2211145472851047127[218] = 0.0;
   out_2211145472851047127[219] = 0.0;
   out_2211145472851047127[220] = 0.0;
   out_2211145472851047127[221] = 0.0;
   out_2211145472851047127[222] = 0.0;
   out_2211145472851047127[223] = 0.0;
   out_2211145472851047127[224] = 0.0;
   out_2211145472851047127[225] = 0.0;
   out_2211145472851047127[226] = 0.0;
   out_2211145472851047127[227] = 0.0;
   out_2211145472851047127[228] = 1.0;
   out_2211145472851047127[229] = 0.0;
   out_2211145472851047127[230] = 0.0;
   out_2211145472851047127[231] = 0.0;
   out_2211145472851047127[232] = 0.0;
   out_2211145472851047127[233] = 0.0;
   out_2211145472851047127[234] = 0.0;
   out_2211145472851047127[235] = 0.0;
   out_2211145472851047127[236] = 0.0;
   out_2211145472851047127[237] = 0.0;
   out_2211145472851047127[238] = 0.0;
   out_2211145472851047127[239] = 0.0;
   out_2211145472851047127[240] = 0.0;
   out_2211145472851047127[241] = 0.0;
   out_2211145472851047127[242] = 0.0;
   out_2211145472851047127[243] = 0.0;
   out_2211145472851047127[244] = 0.0;
   out_2211145472851047127[245] = 0.0;
   out_2211145472851047127[246] = 0.0;
   out_2211145472851047127[247] = 1.0;
   out_2211145472851047127[248] = 0.0;
   out_2211145472851047127[249] = 0.0;
   out_2211145472851047127[250] = 0.0;
   out_2211145472851047127[251] = 0.0;
   out_2211145472851047127[252] = 0.0;
   out_2211145472851047127[253] = 0.0;
   out_2211145472851047127[254] = 0.0;
   out_2211145472851047127[255] = 0.0;
   out_2211145472851047127[256] = 0.0;
   out_2211145472851047127[257] = 0.0;
   out_2211145472851047127[258] = 0.0;
   out_2211145472851047127[259] = 0.0;
   out_2211145472851047127[260] = 0.0;
   out_2211145472851047127[261] = 0.0;
   out_2211145472851047127[262] = 0.0;
   out_2211145472851047127[263] = 0.0;
   out_2211145472851047127[264] = 0.0;
   out_2211145472851047127[265] = 0.0;
   out_2211145472851047127[266] = 1.0;
   out_2211145472851047127[267] = 0.0;
   out_2211145472851047127[268] = 0.0;
   out_2211145472851047127[269] = 0.0;
   out_2211145472851047127[270] = 0.0;
   out_2211145472851047127[271] = 0.0;
   out_2211145472851047127[272] = 0.0;
   out_2211145472851047127[273] = 0.0;
   out_2211145472851047127[274] = 0.0;
   out_2211145472851047127[275] = 0.0;
   out_2211145472851047127[276] = 0.0;
   out_2211145472851047127[277] = 0.0;
   out_2211145472851047127[278] = 0.0;
   out_2211145472851047127[279] = 0.0;
   out_2211145472851047127[280] = 0.0;
   out_2211145472851047127[281] = 0.0;
   out_2211145472851047127[282] = 0.0;
   out_2211145472851047127[283] = 0.0;
   out_2211145472851047127[284] = 0.0;
   out_2211145472851047127[285] = 1.0;
   out_2211145472851047127[286] = 0.0;
   out_2211145472851047127[287] = 0.0;
   out_2211145472851047127[288] = 0.0;
   out_2211145472851047127[289] = 0.0;
   out_2211145472851047127[290] = 0.0;
   out_2211145472851047127[291] = 0.0;
   out_2211145472851047127[292] = 0.0;
   out_2211145472851047127[293] = 0.0;
   out_2211145472851047127[294] = 0.0;
   out_2211145472851047127[295] = 0.0;
   out_2211145472851047127[296] = 0.0;
   out_2211145472851047127[297] = 0.0;
   out_2211145472851047127[298] = 0.0;
   out_2211145472851047127[299] = 0.0;
   out_2211145472851047127[300] = 0.0;
   out_2211145472851047127[301] = 0.0;
   out_2211145472851047127[302] = 0.0;
   out_2211145472851047127[303] = 0.0;
   out_2211145472851047127[304] = 1.0;
   out_2211145472851047127[305] = 0.0;
   out_2211145472851047127[306] = 0.0;
   out_2211145472851047127[307] = 0.0;
   out_2211145472851047127[308] = 0.0;
   out_2211145472851047127[309] = 0.0;
   out_2211145472851047127[310] = 0.0;
   out_2211145472851047127[311] = 0.0;
   out_2211145472851047127[312] = 0.0;
   out_2211145472851047127[313] = 0.0;
   out_2211145472851047127[314] = 0.0;
   out_2211145472851047127[315] = 0.0;
   out_2211145472851047127[316] = 0.0;
   out_2211145472851047127[317] = 0.0;
   out_2211145472851047127[318] = 0.0;
   out_2211145472851047127[319] = 0.0;
   out_2211145472851047127[320] = 0.0;
   out_2211145472851047127[321] = 0.0;
   out_2211145472851047127[322] = 0.0;
   out_2211145472851047127[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_287394934320885742) {
   out_287394934320885742[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_287394934320885742[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_287394934320885742[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_287394934320885742[3] = dt*state[12] + state[3];
   out_287394934320885742[4] = dt*state[13] + state[4];
   out_287394934320885742[5] = dt*state[14] + state[5];
   out_287394934320885742[6] = state[6];
   out_287394934320885742[7] = state[7];
   out_287394934320885742[8] = state[8];
   out_287394934320885742[9] = state[9];
   out_287394934320885742[10] = state[10];
   out_287394934320885742[11] = state[11];
   out_287394934320885742[12] = state[12];
   out_287394934320885742[13] = state[13];
   out_287394934320885742[14] = state[14];
   out_287394934320885742[15] = state[15];
   out_287394934320885742[16] = state[16];
   out_287394934320885742[17] = state[17];
}
void F_fun(double *state, double dt, double *out_6828483922787760217) {
   out_6828483922787760217[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6828483922787760217[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6828483922787760217[2] = 0;
   out_6828483922787760217[3] = 0;
   out_6828483922787760217[4] = 0;
   out_6828483922787760217[5] = 0;
   out_6828483922787760217[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6828483922787760217[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6828483922787760217[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6828483922787760217[9] = 0;
   out_6828483922787760217[10] = 0;
   out_6828483922787760217[11] = 0;
   out_6828483922787760217[12] = 0;
   out_6828483922787760217[13] = 0;
   out_6828483922787760217[14] = 0;
   out_6828483922787760217[15] = 0;
   out_6828483922787760217[16] = 0;
   out_6828483922787760217[17] = 0;
   out_6828483922787760217[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_6828483922787760217[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_6828483922787760217[20] = 0;
   out_6828483922787760217[21] = 0;
   out_6828483922787760217[22] = 0;
   out_6828483922787760217[23] = 0;
   out_6828483922787760217[24] = 0;
   out_6828483922787760217[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_6828483922787760217[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_6828483922787760217[27] = 0;
   out_6828483922787760217[28] = 0;
   out_6828483922787760217[29] = 0;
   out_6828483922787760217[30] = 0;
   out_6828483922787760217[31] = 0;
   out_6828483922787760217[32] = 0;
   out_6828483922787760217[33] = 0;
   out_6828483922787760217[34] = 0;
   out_6828483922787760217[35] = 0;
   out_6828483922787760217[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6828483922787760217[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6828483922787760217[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6828483922787760217[39] = 0;
   out_6828483922787760217[40] = 0;
   out_6828483922787760217[41] = 0;
   out_6828483922787760217[42] = 0;
   out_6828483922787760217[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6828483922787760217[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6828483922787760217[45] = 0;
   out_6828483922787760217[46] = 0;
   out_6828483922787760217[47] = 0;
   out_6828483922787760217[48] = 0;
   out_6828483922787760217[49] = 0;
   out_6828483922787760217[50] = 0;
   out_6828483922787760217[51] = 0;
   out_6828483922787760217[52] = 0;
   out_6828483922787760217[53] = 0;
   out_6828483922787760217[54] = 0;
   out_6828483922787760217[55] = 0;
   out_6828483922787760217[56] = 0;
   out_6828483922787760217[57] = 1;
   out_6828483922787760217[58] = 0;
   out_6828483922787760217[59] = 0;
   out_6828483922787760217[60] = 0;
   out_6828483922787760217[61] = 0;
   out_6828483922787760217[62] = 0;
   out_6828483922787760217[63] = 0;
   out_6828483922787760217[64] = 0;
   out_6828483922787760217[65] = 0;
   out_6828483922787760217[66] = dt;
   out_6828483922787760217[67] = 0;
   out_6828483922787760217[68] = 0;
   out_6828483922787760217[69] = 0;
   out_6828483922787760217[70] = 0;
   out_6828483922787760217[71] = 0;
   out_6828483922787760217[72] = 0;
   out_6828483922787760217[73] = 0;
   out_6828483922787760217[74] = 0;
   out_6828483922787760217[75] = 0;
   out_6828483922787760217[76] = 1;
   out_6828483922787760217[77] = 0;
   out_6828483922787760217[78] = 0;
   out_6828483922787760217[79] = 0;
   out_6828483922787760217[80] = 0;
   out_6828483922787760217[81] = 0;
   out_6828483922787760217[82] = 0;
   out_6828483922787760217[83] = 0;
   out_6828483922787760217[84] = 0;
   out_6828483922787760217[85] = dt;
   out_6828483922787760217[86] = 0;
   out_6828483922787760217[87] = 0;
   out_6828483922787760217[88] = 0;
   out_6828483922787760217[89] = 0;
   out_6828483922787760217[90] = 0;
   out_6828483922787760217[91] = 0;
   out_6828483922787760217[92] = 0;
   out_6828483922787760217[93] = 0;
   out_6828483922787760217[94] = 0;
   out_6828483922787760217[95] = 1;
   out_6828483922787760217[96] = 0;
   out_6828483922787760217[97] = 0;
   out_6828483922787760217[98] = 0;
   out_6828483922787760217[99] = 0;
   out_6828483922787760217[100] = 0;
   out_6828483922787760217[101] = 0;
   out_6828483922787760217[102] = 0;
   out_6828483922787760217[103] = 0;
   out_6828483922787760217[104] = dt;
   out_6828483922787760217[105] = 0;
   out_6828483922787760217[106] = 0;
   out_6828483922787760217[107] = 0;
   out_6828483922787760217[108] = 0;
   out_6828483922787760217[109] = 0;
   out_6828483922787760217[110] = 0;
   out_6828483922787760217[111] = 0;
   out_6828483922787760217[112] = 0;
   out_6828483922787760217[113] = 0;
   out_6828483922787760217[114] = 1;
   out_6828483922787760217[115] = 0;
   out_6828483922787760217[116] = 0;
   out_6828483922787760217[117] = 0;
   out_6828483922787760217[118] = 0;
   out_6828483922787760217[119] = 0;
   out_6828483922787760217[120] = 0;
   out_6828483922787760217[121] = 0;
   out_6828483922787760217[122] = 0;
   out_6828483922787760217[123] = 0;
   out_6828483922787760217[124] = 0;
   out_6828483922787760217[125] = 0;
   out_6828483922787760217[126] = 0;
   out_6828483922787760217[127] = 0;
   out_6828483922787760217[128] = 0;
   out_6828483922787760217[129] = 0;
   out_6828483922787760217[130] = 0;
   out_6828483922787760217[131] = 0;
   out_6828483922787760217[132] = 0;
   out_6828483922787760217[133] = 1;
   out_6828483922787760217[134] = 0;
   out_6828483922787760217[135] = 0;
   out_6828483922787760217[136] = 0;
   out_6828483922787760217[137] = 0;
   out_6828483922787760217[138] = 0;
   out_6828483922787760217[139] = 0;
   out_6828483922787760217[140] = 0;
   out_6828483922787760217[141] = 0;
   out_6828483922787760217[142] = 0;
   out_6828483922787760217[143] = 0;
   out_6828483922787760217[144] = 0;
   out_6828483922787760217[145] = 0;
   out_6828483922787760217[146] = 0;
   out_6828483922787760217[147] = 0;
   out_6828483922787760217[148] = 0;
   out_6828483922787760217[149] = 0;
   out_6828483922787760217[150] = 0;
   out_6828483922787760217[151] = 0;
   out_6828483922787760217[152] = 1;
   out_6828483922787760217[153] = 0;
   out_6828483922787760217[154] = 0;
   out_6828483922787760217[155] = 0;
   out_6828483922787760217[156] = 0;
   out_6828483922787760217[157] = 0;
   out_6828483922787760217[158] = 0;
   out_6828483922787760217[159] = 0;
   out_6828483922787760217[160] = 0;
   out_6828483922787760217[161] = 0;
   out_6828483922787760217[162] = 0;
   out_6828483922787760217[163] = 0;
   out_6828483922787760217[164] = 0;
   out_6828483922787760217[165] = 0;
   out_6828483922787760217[166] = 0;
   out_6828483922787760217[167] = 0;
   out_6828483922787760217[168] = 0;
   out_6828483922787760217[169] = 0;
   out_6828483922787760217[170] = 0;
   out_6828483922787760217[171] = 1;
   out_6828483922787760217[172] = 0;
   out_6828483922787760217[173] = 0;
   out_6828483922787760217[174] = 0;
   out_6828483922787760217[175] = 0;
   out_6828483922787760217[176] = 0;
   out_6828483922787760217[177] = 0;
   out_6828483922787760217[178] = 0;
   out_6828483922787760217[179] = 0;
   out_6828483922787760217[180] = 0;
   out_6828483922787760217[181] = 0;
   out_6828483922787760217[182] = 0;
   out_6828483922787760217[183] = 0;
   out_6828483922787760217[184] = 0;
   out_6828483922787760217[185] = 0;
   out_6828483922787760217[186] = 0;
   out_6828483922787760217[187] = 0;
   out_6828483922787760217[188] = 0;
   out_6828483922787760217[189] = 0;
   out_6828483922787760217[190] = 1;
   out_6828483922787760217[191] = 0;
   out_6828483922787760217[192] = 0;
   out_6828483922787760217[193] = 0;
   out_6828483922787760217[194] = 0;
   out_6828483922787760217[195] = 0;
   out_6828483922787760217[196] = 0;
   out_6828483922787760217[197] = 0;
   out_6828483922787760217[198] = 0;
   out_6828483922787760217[199] = 0;
   out_6828483922787760217[200] = 0;
   out_6828483922787760217[201] = 0;
   out_6828483922787760217[202] = 0;
   out_6828483922787760217[203] = 0;
   out_6828483922787760217[204] = 0;
   out_6828483922787760217[205] = 0;
   out_6828483922787760217[206] = 0;
   out_6828483922787760217[207] = 0;
   out_6828483922787760217[208] = 0;
   out_6828483922787760217[209] = 1;
   out_6828483922787760217[210] = 0;
   out_6828483922787760217[211] = 0;
   out_6828483922787760217[212] = 0;
   out_6828483922787760217[213] = 0;
   out_6828483922787760217[214] = 0;
   out_6828483922787760217[215] = 0;
   out_6828483922787760217[216] = 0;
   out_6828483922787760217[217] = 0;
   out_6828483922787760217[218] = 0;
   out_6828483922787760217[219] = 0;
   out_6828483922787760217[220] = 0;
   out_6828483922787760217[221] = 0;
   out_6828483922787760217[222] = 0;
   out_6828483922787760217[223] = 0;
   out_6828483922787760217[224] = 0;
   out_6828483922787760217[225] = 0;
   out_6828483922787760217[226] = 0;
   out_6828483922787760217[227] = 0;
   out_6828483922787760217[228] = 1;
   out_6828483922787760217[229] = 0;
   out_6828483922787760217[230] = 0;
   out_6828483922787760217[231] = 0;
   out_6828483922787760217[232] = 0;
   out_6828483922787760217[233] = 0;
   out_6828483922787760217[234] = 0;
   out_6828483922787760217[235] = 0;
   out_6828483922787760217[236] = 0;
   out_6828483922787760217[237] = 0;
   out_6828483922787760217[238] = 0;
   out_6828483922787760217[239] = 0;
   out_6828483922787760217[240] = 0;
   out_6828483922787760217[241] = 0;
   out_6828483922787760217[242] = 0;
   out_6828483922787760217[243] = 0;
   out_6828483922787760217[244] = 0;
   out_6828483922787760217[245] = 0;
   out_6828483922787760217[246] = 0;
   out_6828483922787760217[247] = 1;
   out_6828483922787760217[248] = 0;
   out_6828483922787760217[249] = 0;
   out_6828483922787760217[250] = 0;
   out_6828483922787760217[251] = 0;
   out_6828483922787760217[252] = 0;
   out_6828483922787760217[253] = 0;
   out_6828483922787760217[254] = 0;
   out_6828483922787760217[255] = 0;
   out_6828483922787760217[256] = 0;
   out_6828483922787760217[257] = 0;
   out_6828483922787760217[258] = 0;
   out_6828483922787760217[259] = 0;
   out_6828483922787760217[260] = 0;
   out_6828483922787760217[261] = 0;
   out_6828483922787760217[262] = 0;
   out_6828483922787760217[263] = 0;
   out_6828483922787760217[264] = 0;
   out_6828483922787760217[265] = 0;
   out_6828483922787760217[266] = 1;
   out_6828483922787760217[267] = 0;
   out_6828483922787760217[268] = 0;
   out_6828483922787760217[269] = 0;
   out_6828483922787760217[270] = 0;
   out_6828483922787760217[271] = 0;
   out_6828483922787760217[272] = 0;
   out_6828483922787760217[273] = 0;
   out_6828483922787760217[274] = 0;
   out_6828483922787760217[275] = 0;
   out_6828483922787760217[276] = 0;
   out_6828483922787760217[277] = 0;
   out_6828483922787760217[278] = 0;
   out_6828483922787760217[279] = 0;
   out_6828483922787760217[280] = 0;
   out_6828483922787760217[281] = 0;
   out_6828483922787760217[282] = 0;
   out_6828483922787760217[283] = 0;
   out_6828483922787760217[284] = 0;
   out_6828483922787760217[285] = 1;
   out_6828483922787760217[286] = 0;
   out_6828483922787760217[287] = 0;
   out_6828483922787760217[288] = 0;
   out_6828483922787760217[289] = 0;
   out_6828483922787760217[290] = 0;
   out_6828483922787760217[291] = 0;
   out_6828483922787760217[292] = 0;
   out_6828483922787760217[293] = 0;
   out_6828483922787760217[294] = 0;
   out_6828483922787760217[295] = 0;
   out_6828483922787760217[296] = 0;
   out_6828483922787760217[297] = 0;
   out_6828483922787760217[298] = 0;
   out_6828483922787760217[299] = 0;
   out_6828483922787760217[300] = 0;
   out_6828483922787760217[301] = 0;
   out_6828483922787760217[302] = 0;
   out_6828483922787760217[303] = 0;
   out_6828483922787760217[304] = 1;
   out_6828483922787760217[305] = 0;
   out_6828483922787760217[306] = 0;
   out_6828483922787760217[307] = 0;
   out_6828483922787760217[308] = 0;
   out_6828483922787760217[309] = 0;
   out_6828483922787760217[310] = 0;
   out_6828483922787760217[311] = 0;
   out_6828483922787760217[312] = 0;
   out_6828483922787760217[313] = 0;
   out_6828483922787760217[314] = 0;
   out_6828483922787760217[315] = 0;
   out_6828483922787760217[316] = 0;
   out_6828483922787760217[317] = 0;
   out_6828483922787760217[318] = 0;
   out_6828483922787760217[319] = 0;
   out_6828483922787760217[320] = 0;
   out_6828483922787760217[321] = 0;
   out_6828483922787760217[322] = 0;
   out_6828483922787760217[323] = 1;
}
void h_4(double *state, double *unused, double *out_2357736201306386795) {
   out_2357736201306386795[0] = state[6] + state[9];
   out_2357736201306386795[1] = state[7] + state[10];
   out_2357736201306386795[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_7369870586317350803) {
   out_7369870586317350803[0] = 0;
   out_7369870586317350803[1] = 0;
   out_7369870586317350803[2] = 0;
   out_7369870586317350803[3] = 0;
   out_7369870586317350803[4] = 0;
   out_7369870586317350803[5] = 0;
   out_7369870586317350803[6] = 1;
   out_7369870586317350803[7] = 0;
   out_7369870586317350803[8] = 0;
   out_7369870586317350803[9] = 1;
   out_7369870586317350803[10] = 0;
   out_7369870586317350803[11] = 0;
   out_7369870586317350803[12] = 0;
   out_7369870586317350803[13] = 0;
   out_7369870586317350803[14] = 0;
   out_7369870586317350803[15] = 0;
   out_7369870586317350803[16] = 0;
   out_7369870586317350803[17] = 0;
   out_7369870586317350803[18] = 0;
   out_7369870586317350803[19] = 0;
   out_7369870586317350803[20] = 0;
   out_7369870586317350803[21] = 0;
   out_7369870586317350803[22] = 0;
   out_7369870586317350803[23] = 0;
   out_7369870586317350803[24] = 0;
   out_7369870586317350803[25] = 1;
   out_7369870586317350803[26] = 0;
   out_7369870586317350803[27] = 0;
   out_7369870586317350803[28] = 1;
   out_7369870586317350803[29] = 0;
   out_7369870586317350803[30] = 0;
   out_7369870586317350803[31] = 0;
   out_7369870586317350803[32] = 0;
   out_7369870586317350803[33] = 0;
   out_7369870586317350803[34] = 0;
   out_7369870586317350803[35] = 0;
   out_7369870586317350803[36] = 0;
   out_7369870586317350803[37] = 0;
   out_7369870586317350803[38] = 0;
   out_7369870586317350803[39] = 0;
   out_7369870586317350803[40] = 0;
   out_7369870586317350803[41] = 0;
   out_7369870586317350803[42] = 0;
   out_7369870586317350803[43] = 0;
   out_7369870586317350803[44] = 1;
   out_7369870586317350803[45] = 0;
   out_7369870586317350803[46] = 0;
   out_7369870586317350803[47] = 1;
   out_7369870586317350803[48] = 0;
   out_7369870586317350803[49] = 0;
   out_7369870586317350803[50] = 0;
   out_7369870586317350803[51] = 0;
   out_7369870586317350803[52] = 0;
   out_7369870586317350803[53] = 0;
}
void h_10(double *state, double *unused, double *out_7192317269448767453) {
   out_7192317269448767453[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_7192317269448767453[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_7192317269448767453[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_1958792711272977224) {
   out_1958792711272977224[0] = 0;
   out_1958792711272977224[1] = 9.8100000000000005*cos(state[1]);
   out_1958792711272977224[2] = 0;
   out_1958792711272977224[3] = 0;
   out_1958792711272977224[4] = -state[8];
   out_1958792711272977224[5] = state[7];
   out_1958792711272977224[6] = 0;
   out_1958792711272977224[7] = state[5];
   out_1958792711272977224[8] = -state[4];
   out_1958792711272977224[9] = 0;
   out_1958792711272977224[10] = 0;
   out_1958792711272977224[11] = 0;
   out_1958792711272977224[12] = 1;
   out_1958792711272977224[13] = 0;
   out_1958792711272977224[14] = 0;
   out_1958792711272977224[15] = 1;
   out_1958792711272977224[16] = 0;
   out_1958792711272977224[17] = 0;
   out_1958792711272977224[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_1958792711272977224[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_1958792711272977224[20] = 0;
   out_1958792711272977224[21] = state[8];
   out_1958792711272977224[22] = 0;
   out_1958792711272977224[23] = -state[6];
   out_1958792711272977224[24] = -state[5];
   out_1958792711272977224[25] = 0;
   out_1958792711272977224[26] = state[3];
   out_1958792711272977224[27] = 0;
   out_1958792711272977224[28] = 0;
   out_1958792711272977224[29] = 0;
   out_1958792711272977224[30] = 0;
   out_1958792711272977224[31] = 1;
   out_1958792711272977224[32] = 0;
   out_1958792711272977224[33] = 0;
   out_1958792711272977224[34] = 1;
   out_1958792711272977224[35] = 0;
   out_1958792711272977224[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_1958792711272977224[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_1958792711272977224[38] = 0;
   out_1958792711272977224[39] = -state[7];
   out_1958792711272977224[40] = state[6];
   out_1958792711272977224[41] = 0;
   out_1958792711272977224[42] = state[4];
   out_1958792711272977224[43] = -state[3];
   out_1958792711272977224[44] = 0;
   out_1958792711272977224[45] = 0;
   out_1958792711272977224[46] = 0;
   out_1958792711272977224[47] = 0;
   out_1958792711272977224[48] = 0;
   out_1958792711272977224[49] = 0;
   out_1958792711272977224[50] = 1;
   out_1958792711272977224[51] = 0;
   out_1958792711272977224[52] = 0;
   out_1958792711272977224[53] = 1;
}
void h_13(double *state, double *unused, double *out_7636136109731301200) {
   out_7636136109731301200[0] = state[3];
   out_7636136109731301200[1] = state[4];
   out_7636136109731301200[2] = state[5];
}
void H_13(double *state, double *unused, double *out_7864599662059868012) {
   out_7864599662059868012[0] = 0;
   out_7864599662059868012[1] = 0;
   out_7864599662059868012[2] = 0;
   out_7864599662059868012[3] = 1;
   out_7864599662059868012[4] = 0;
   out_7864599662059868012[5] = 0;
   out_7864599662059868012[6] = 0;
   out_7864599662059868012[7] = 0;
   out_7864599662059868012[8] = 0;
   out_7864599662059868012[9] = 0;
   out_7864599662059868012[10] = 0;
   out_7864599662059868012[11] = 0;
   out_7864599662059868012[12] = 0;
   out_7864599662059868012[13] = 0;
   out_7864599662059868012[14] = 0;
   out_7864599662059868012[15] = 0;
   out_7864599662059868012[16] = 0;
   out_7864599662059868012[17] = 0;
   out_7864599662059868012[18] = 0;
   out_7864599662059868012[19] = 0;
   out_7864599662059868012[20] = 0;
   out_7864599662059868012[21] = 0;
   out_7864599662059868012[22] = 1;
   out_7864599662059868012[23] = 0;
   out_7864599662059868012[24] = 0;
   out_7864599662059868012[25] = 0;
   out_7864599662059868012[26] = 0;
   out_7864599662059868012[27] = 0;
   out_7864599662059868012[28] = 0;
   out_7864599662059868012[29] = 0;
   out_7864599662059868012[30] = 0;
   out_7864599662059868012[31] = 0;
   out_7864599662059868012[32] = 0;
   out_7864599662059868012[33] = 0;
   out_7864599662059868012[34] = 0;
   out_7864599662059868012[35] = 0;
   out_7864599662059868012[36] = 0;
   out_7864599662059868012[37] = 0;
   out_7864599662059868012[38] = 0;
   out_7864599662059868012[39] = 0;
   out_7864599662059868012[40] = 0;
   out_7864599662059868012[41] = 1;
   out_7864599662059868012[42] = 0;
   out_7864599662059868012[43] = 0;
   out_7864599662059868012[44] = 0;
   out_7864599662059868012[45] = 0;
   out_7864599662059868012[46] = 0;
   out_7864599662059868012[47] = 0;
   out_7864599662059868012[48] = 0;
   out_7864599662059868012[49] = 0;
   out_7864599662059868012[50] = 0;
   out_7864599662059868012[51] = 0;
   out_7864599662059868012[52] = 0;
   out_7864599662059868012[53] = 0;
}
void h_14(double *state, double *unused, double *out_7813843745027135896) {
   out_7813843745027135896[0] = state[6];
   out_7813843745027135896[1] = state[7];
   out_7813843745027135896[2] = state[8];
}
void H_14(double *state, double *unused, double *out_7113632631052716284) {
   out_7113632631052716284[0] = 0;
   out_7113632631052716284[1] = 0;
   out_7113632631052716284[2] = 0;
   out_7113632631052716284[3] = 0;
   out_7113632631052716284[4] = 0;
   out_7113632631052716284[5] = 0;
   out_7113632631052716284[6] = 1;
   out_7113632631052716284[7] = 0;
   out_7113632631052716284[8] = 0;
   out_7113632631052716284[9] = 0;
   out_7113632631052716284[10] = 0;
   out_7113632631052716284[11] = 0;
   out_7113632631052716284[12] = 0;
   out_7113632631052716284[13] = 0;
   out_7113632631052716284[14] = 0;
   out_7113632631052716284[15] = 0;
   out_7113632631052716284[16] = 0;
   out_7113632631052716284[17] = 0;
   out_7113632631052716284[18] = 0;
   out_7113632631052716284[19] = 0;
   out_7113632631052716284[20] = 0;
   out_7113632631052716284[21] = 0;
   out_7113632631052716284[22] = 0;
   out_7113632631052716284[23] = 0;
   out_7113632631052716284[24] = 0;
   out_7113632631052716284[25] = 1;
   out_7113632631052716284[26] = 0;
   out_7113632631052716284[27] = 0;
   out_7113632631052716284[28] = 0;
   out_7113632631052716284[29] = 0;
   out_7113632631052716284[30] = 0;
   out_7113632631052716284[31] = 0;
   out_7113632631052716284[32] = 0;
   out_7113632631052716284[33] = 0;
   out_7113632631052716284[34] = 0;
   out_7113632631052716284[35] = 0;
   out_7113632631052716284[36] = 0;
   out_7113632631052716284[37] = 0;
   out_7113632631052716284[38] = 0;
   out_7113632631052716284[39] = 0;
   out_7113632631052716284[40] = 0;
   out_7113632631052716284[41] = 0;
   out_7113632631052716284[42] = 0;
   out_7113632631052716284[43] = 0;
   out_7113632631052716284[44] = 1;
   out_7113632631052716284[45] = 0;
   out_7113632631052716284[46] = 0;
   out_7113632631052716284[47] = 0;
   out_7113632631052716284[48] = 0;
   out_7113632631052716284[49] = 0;
   out_7113632631052716284[50] = 0;
   out_7113632631052716284[51] = 0;
   out_7113632631052716284[52] = 0;
   out_7113632631052716284[53] = 0;
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
void pose_err_fun(double *nom_x, double *delta_x, double *out_2213402149788806935) {
  err_fun(nom_x, delta_x, out_2213402149788806935);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_8912039620049795540) {
  inv_err_fun(nom_x, true_x, out_8912039620049795540);
}
void pose_H_mod_fun(double *state, double *out_2211145472851047127) {
  H_mod_fun(state, out_2211145472851047127);
}
void pose_f_fun(double *state, double dt, double *out_287394934320885742) {
  f_fun(state,  dt, out_287394934320885742);
}
void pose_F_fun(double *state, double dt, double *out_6828483922787760217) {
  F_fun(state,  dt, out_6828483922787760217);
}
void pose_h_4(double *state, double *unused, double *out_2357736201306386795) {
  h_4(state, unused, out_2357736201306386795);
}
void pose_H_4(double *state, double *unused, double *out_7369870586317350803) {
  H_4(state, unused, out_7369870586317350803);
}
void pose_h_10(double *state, double *unused, double *out_7192317269448767453) {
  h_10(state, unused, out_7192317269448767453);
}
void pose_H_10(double *state, double *unused, double *out_1958792711272977224) {
  H_10(state, unused, out_1958792711272977224);
}
void pose_h_13(double *state, double *unused, double *out_7636136109731301200) {
  h_13(state, unused, out_7636136109731301200);
}
void pose_H_13(double *state, double *unused, double *out_7864599662059868012) {
  H_13(state, unused, out_7864599662059868012);
}
void pose_h_14(double *state, double *unused, double *out_7813843745027135896) {
  h_14(state, unused, out_7813843745027135896);
}
void pose_H_14(double *state, double *unused, double *out_7113632631052716284) {
  H_14(state, unused, out_7113632631052716284);
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
