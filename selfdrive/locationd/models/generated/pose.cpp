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
void err_fun(double *nom_x, double *delta_x, double *out_3175539824002796945) {
   out_3175539824002796945[0] = delta_x[0] + nom_x[0];
   out_3175539824002796945[1] = delta_x[1] + nom_x[1];
   out_3175539824002796945[2] = delta_x[2] + nom_x[2];
   out_3175539824002796945[3] = delta_x[3] + nom_x[3];
   out_3175539824002796945[4] = delta_x[4] + nom_x[4];
   out_3175539824002796945[5] = delta_x[5] + nom_x[5];
   out_3175539824002796945[6] = delta_x[6] + nom_x[6];
   out_3175539824002796945[7] = delta_x[7] + nom_x[7];
   out_3175539824002796945[8] = delta_x[8] + nom_x[8];
   out_3175539824002796945[9] = delta_x[9] + nom_x[9];
   out_3175539824002796945[10] = delta_x[10] + nom_x[10];
   out_3175539824002796945[11] = delta_x[11] + nom_x[11];
   out_3175539824002796945[12] = delta_x[12] + nom_x[12];
   out_3175539824002796945[13] = delta_x[13] + nom_x[13];
   out_3175539824002796945[14] = delta_x[14] + nom_x[14];
   out_3175539824002796945[15] = delta_x[15] + nom_x[15];
   out_3175539824002796945[16] = delta_x[16] + nom_x[16];
   out_3175539824002796945[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_2251372369976179325) {
   out_2251372369976179325[0] = -nom_x[0] + true_x[0];
   out_2251372369976179325[1] = -nom_x[1] + true_x[1];
   out_2251372369976179325[2] = -nom_x[2] + true_x[2];
   out_2251372369976179325[3] = -nom_x[3] + true_x[3];
   out_2251372369976179325[4] = -nom_x[4] + true_x[4];
   out_2251372369976179325[5] = -nom_x[5] + true_x[5];
   out_2251372369976179325[6] = -nom_x[6] + true_x[6];
   out_2251372369976179325[7] = -nom_x[7] + true_x[7];
   out_2251372369976179325[8] = -nom_x[8] + true_x[8];
   out_2251372369976179325[9] = -nom_x[9] + true_x[9];
   out_2251372369976179325[10] = -nom_x[10] + true_x[10];
   out_2251372369976179325[11] = -nom_x[11] + true_x[11];
   out_2251372369976179325[12] = -nom_x[12] + true_x[12];
   out_2251372369976179325[13] = -nom_x[13] + true_x[13];
   out_2251372369976179325[14] = -nom_x[14] + true_x[14];
   out_2251372369976179325[15] = -nom_x[15] + true_x[15];
   out_2251372369976179325[16] = -nom_x[16] + true_x[16];
   out_2251372369976179325[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_5180136592465146401) {
   out_5180136592465146401[0] = 1.0;
   out_5180136592465146401[1] = 0.0;
   out_5180136592465146401[2] = 0.0;
   out_5180136592465146401[3] = 0.0;
   out_5180136592465146401[4] = 0.0;
   out_5180136592465146401[5] = 0.0;
   out_5180136592465146401[6] = 0.0;
   out_5180136592465146401[7] = 0.0;
   out_5180136592465146401[8] = 0.0;
   out_5180136592465146401[9] = 0.0;
   out_5180136592465146401[10] = 0.0;
   out_5180136592465146401[11] = 0.0;
   out_5180136592465146401[12] = 0.0;
   out_5180136592465146401[13] = 0.0;
   out_5180136592465146401[14] = 0.0;
   out_5180136592465146401[15] = 0.0;
   out_5180136592465146401[16] = 0.0;
   out_5180136592465146401[17] = 0.0;
   out_5180136592465146401[18] = 0.0;
   out_5180136592465146401[19] = 1.0;
   out_5180136592465146401[20] = 0.0;
   out_5180136592465146401[21] = 0.0;
   out_5180136592465146401[22] = 0.0;
   out_5180136592465146401[23] = 0.0;
   out_5180136592465146401[24] = 0.0;
   out_5180136592465146401[25] = 0.0;
   out_5180136592465146401[26] = 0.0;
   out_5180136592465146401[27] = 0.0;
   out_5180136592465146401[28] = 0.0;
   out_5180136592465146401[29] = 0.0;
   out_5180136592465146401[30] = 0.0;
   out_5180136592465146401[31] = 0.0;
   out_5180136592465146401[32] = 0.0;
   out_5180136592465146401[33] = 0.0;
   out_5180136592465146401[34] = 0.0;
   out_5180136592465146401[35] = 0.0;
   out_5180136592465146401[36] = 0.0;
   out_5180136592465146401[37] = 0.0;
   out_5180136592465146401[38] = 1.0;
   out_5180136592465146401[39] = 0.0;
   out_5180136592465146401[40] = 0.0;
   out_5180136592465146401[41] = 0.0;
   out_5180136592465146401[42] = 0.0;
   out_5180136592465146401[43] = 0.0;
   out_5180136592465146401[44] = 0.0;
   out_5180136592465146401[45] = 0.0;
   out_5180136592465146401[46] = 0.0;
   out_5180136592465146401[47] = 0.0;
   out_5180136592465146401[48] = 0.0;
   out_5180136592465146401[49] = 0.0;
   out_5180136592465146401[50] = 0.0;
   out_5180136592465146401[51] = 0.0;
   out_5180136592465146401[52] = 0.0;
   out_5180136592465146401[53] = 0.0;
   out_5180136592465146401[54] = 0.0;
   out_5180136592465146401[55] = 0.0;
   out_5180136592465146401[56] = 0.0;
   out_5180136592465146401[57] = 1.0;
   out_5180136592465146401[58] = 0.0;
   out_5180136592465146401[59] = 0.0;
   out_5180136592465146401[60] = 0.0;
   out_5180136592465146401[61] = 0.0;
   out_5180136592465146401[62] = 0.0;
   out_5180136592465146401[63] = 0.0;
   out_5180136592465146401[64] = 0.0;
   out_5180136592465146401[65] = 0.0;
   out_5180136592465146401[66] = 0.0;
   out_5180136592465146401[67] = 0.0;
   out_5180136592465146401[68] = 0.0;
   out_5180136592465146401[69] = 0.0;
   out_5180136592465146401[70] = 0.0;
   out_5180136592465146401[71] = 0.0;
   out_5180136592465146401[72] = 0.0;
   out_5180136592465146401[73] = 0.0;
   out_5180136592465146401[74] = 0.0;
   out_5180136592465146401[75] = 0.0;
   out_5180136592465146401[76] = 1.0;
   out_5180136592465146401[77] = 0.0;
   out_5180136592465146401[78] = 0.0;
   out_5180136592465146401[79] = 0.0;
   out_5180136592465146401[80] = 0.0;
   out_5180136592465146401[81] = 0.0;
   out_5180136592465146401[82] = 0.0;
   out_5180136592465146401[83] = 0.0;
   out_5180136592465146401[84] = 0.0;
   out_5180136592465146401[85] = 0.0;
   out_5180136592465146401[86] = 0.0;
   out_5180136592465146401[87] = 0.0;
   out_5180136592465146401[88] = 0.0;
   out_5180136592465146401[89] = 0.0;
   out_5180136592465146401[90] = 0.0;
   out_5180136592465146401[91] = 0.0;
   out_5180136592465146401[92] = 0.0;
   out_5180136592465146401[93] = 0.0;
   out_5180136592465146401[94] = 0.0;
   out_5180136592465146401[95] = 1.0;
   out_5180136592465146401[96] = 0.0;
   out_5180136592465146401[97] = 0.0;
   out_5180136592465146401[98] = 0.0;
   out_5180136592465146401[99] = 0.0;
   out_5180136592465146401[100] = 0.0;
   out_5180136592465146401[101] = 0.0;
   out_5180136592465146401[102] = 0.0;
   out_5180136592465146401[103] = 0.0;
   out_5180136592465146401[104] = 0.0;
   out_5180136592465146401[105] = 0.0;
   out_5180136592465146401[106] = 0.0;
   out_5180136592465146401[107] = 0.0;
   out_5180136592465146401[108] = 0.0;
   out_5180136592465146401[109] = 0.0;
   out_5180136592465146401[110] = 0.0;
   out_5180136592465146401[111] = 0.0;
   out_5180136592465146401[112] = 0.0;
   out_5180136592465146401[113] = 0.0;
   out_5180136592465146401[114] = 1.0;
   out_5180136592465146401[115] = 0.0;
   out_5180136592465146401[116] = 0.0;
   out_5180136592465146401[117] = 0.0;
   out_5180136592465146401[118] = 0.0;
   out_5180136592465146401[119] = 0.0;
   out_5180136592465146401[120] = 0.0;
   out_5180136592465146401[121] = 0.0;
   out_5180136592465146401[122] = 0.0;
   out_5180136592465146401[123] = 0.0;
   out_5180136592465146401[124] = 0.0;
   out_5180136592465146401[125] = 0.0;
   out_5180136592465146401[126] = 0.0;
   out_5180136592465146401[127] = 0.0;
   out_5180136592465146401[128] = 0.0;
   out_5180136592465146401[129] = 0.0;
   out_5180136592465146401[130] = 0.0;
   out_5180136592465146401[131] = 0.0;
   out_5180136592465146401[132] = 0.0;
   out_5180136592465146401[133] = 1.0;
   out_5180136592465146401[134] = 0.0;
   out_5180136592465146401[135] = 0.0;
   out_5180136592465146401[136] = 0.0;
   out_5180136592465146401[137] = 0.0;
   out_5180136592465146401[138] = 0.0;
   out_5180136592465146401[139] = 0.0;
   out_5180136592465146401[140] = 0.0;
   out_5180136592465146401[141] = 0.0;
   out_5180136592465146401[142] = 0.0;
   out_5180136592465146401[143] = 0.0;
   out_5180136592465146401[144] = 0.0;
   out_5180136592465146401[145] = 0.0;
   out_5180136592465146401[146] = 0.0;
   out_5180136592465146401[147] = 0.0;
   out_5180136592465146401[148] = 0.0;
   out_5180136592465146401[149] = 0.0;
   out_5180136592465146401[150] = 0.0;
   out_5180136592465146401[151] = 0.0;
   out_5180136592465146401[152] = 1.0;
   out_5180136592465146401[153] = 0.0;
   out_5180136592465146401[154] = 0.0;
   out_5180136592465146401[155] = 0.0;
   out_5180136592465146401[156] = 0.0;
   out_5180136592465146401[157] = 0.0;
   out_5180136592465146401[158] = 0.0;
   out_5180136592465146401[159] = 0.0;
   out_5180136592465146401[160] = 0.0;
   out_5180136592465146401[161] = 0.0;
   out_5180136592465146401[162] = 0.0;
   out_5180136592465146401[163] = 0.0;
   out_5180136592465146401[164] = 0.0;
   out_5180136592465146401[165] = 0.0;
   out_5180136592465146401[166] = 0.0;
   out_5180136592465146401[167] = 0.0;
   out_5180136592465146401[168] = 0.0;
   out_5180136592465146401[169] = 0.0;
   out_5180136592465146401[170] = 0.0;
   out_5180136592465146401[171] = 1.0;
   out_5180136592465146401[172] = 0.0;
   out_5180136592465146401[173] = 0.0;
   out_5180136592465146401[174] = 0.0;
   out_5180136592465146401[175] = 0.0;
   out_5180136592465146401[176] = 0.0;
   out_5180136592465146401[177] = 0.0;
   out_5180136592465146401[178] = 0.0;
   out_5180136592465146401[179] = 0.0;
   out_5180136592465146401[180] = 0.0;
   out_5180136592465146401[181] = 0.0;
   out_5180136592465146401[182] = 0.0;
   out_5180136592465146401[183] = 0.0;
   out_5180136592465146401[184] = 0.0;
   out_5180136592465146401[185] = 0.0;
   out_5180136592465146401[186] = 0.0;
   out_5180136592465146401[187] = 0.0;
   out_5180136592465146401[188] = 0.0;
   out_5180136592465146401[189] = 0.0;
   out_5180136592465146401[190] = 1.0;
   out_5180136592465146401[191] = 0.0;
   out_5180136592465146401[192] = 0.0;
   out_5180136592465146401[193] = 0.0;
   out_5180136592465146401[194] = 0.0;
   out_5180136592465146401[195] = 0.0;
   out_5180136592465146401[196] = 0.0;
   out_5180136592465146401[197] = 0.0;
   out_5180136592465146401[198] = 0.0;
   out_5180136592465146401[199] = 0.0;
   out_5180136592465146401[200] = 0.0;
   out_5180136592465146401[201] = 0.0;
   out_5180136592465146401[202] = 0.0;
   out_5180136592465146401[203] = 0.0;
   out_5180136592465146401[204] = 0.0;
   out_5180136592465146401[205] = 0.0;
   out_5180136592465146401[206] = 0.0;
   out_5180136592465146401[207] = 0.0;
   out_5180136592465146401[208] = 0.0;
   out_5180136592465146401[209] = 1.0;
   out_5180136592465146401[210] = 0.0;
   out_5180136592465146401[211] = 0.0;
   out_5180136592465146401[212] = 0.0;
   out_5180136592465146401[213] = 0.0;
   out_5180136592465146401[214] = 0.0;
   out_5180136592465146401[215] = 0.0;
   out_5180136592465146401[216] = 0.0;
   out_5180136592465146401[217] = 0.0;
   out_5180136592465146401[218] = 0.0;
   out_5180136592465146401[219] = 0.0;
   out_5180136592465146401[220] = 0.0;
   out_5180136592465146401[221] = 0.0;
   out_5180136592465146401[222] = 0.0;
   out_5180136592465146401[223] = 0.0;
   out_5180136592465146401[224] = 0.0;
   out_5180136592465146401[225] = 0.0;
   out_5180136592465146401[226] = 0.0;
   out_5180136592465146401[227] = 0.0;
   out_5180136592465146401[228] = 1.0;
   out_5180136592465146401[229] = 0.0;
   out_5180136592465146401[230] = 0.0;
   out_5180136592465146401[231] = 0.0;
   out_5180136592465146401[232] = 0.0;
   out_5180136592465146401[233] = 0.0;
   out_5180136592465146401[234] = 0.0;
   out_5180136592465146401[235] = 0.0;
   out_5180136592465146401[236] = 0.0;
   out_5180136592465146401[237] = 0.0;
   out_5180136592465146401[238] = 0.0;
   out_5180136592465146401[239] = 0.0;
   out_5180136592465146401[240] = 0.0;
   out_5180136592465146401[241] = 0.0;
   out_5180136592465146401[242] = 0.0;
   out_5180136592465146401[243] = 0.0;
   out_5180136592465146401[244] = 0.0;
   out_5180136592465146401[245] = 0.0;
   out_5180136592465146401[246] = 0.0;
   out_5180136592465146401[247] = 1.0;
   out_5180136592465146401[248] = 0.0;
   out_5180136592465146401[249] = 0.0;
   out_5180136592465146401[250] = 0.0;
   out_5180136592465146401[251] = 0.0;
   out_5180136592465146401[252] = 0.0;
   out_5180136592465146401[253] = 0.0;
   out_5180136592465146401[254] = 0.0;
   out_5180136592465146401[255] = 0.0;
   out_5180136592465146401[256] = 0.0;
   out_5180136592465146401[257] = 0.0;
   out_5180136592465146401[258] = 0.0;
   out_5180136592465146401[259] = 0.0;
   out_5180136592465146401[260] = 0.0;
   out_5180136592465146401[261] = 0.0;
   out_5180136592465146401[262] = 0.0;
   out_5180136592465146401[263] = 0.0;
   out_5180136592465146401[264] = 0.0;
   out_5180136592465146401[265] = 0.0;
   out_5180136592465146401[266] = 1.0;
   out_5180136592465146401[267] = 0.0;
   out_5180136592465146401[268] = 0.0;
   out_5180136592465146401[269] = 0.0;
   out_5180136592465146401[270] = 0.0;
   out_5180136592465146401[271] = 0.0;
   out_5180136592465146401[272] = 0.0;
   out_5180136592465146401[273] = 0.0;
   out_5180136592465146401[274] = 0.0;
   out_5180136592465146401[275] = 0.0;
   out_5180136592465146401[276] = 0.0;
   out_5180136592465146401[277] = 0.0;
   out_5180136592465146401[278] = 0.0;
   out_5180136592465146401[279] = 0.0;
   out_5180136592465146401[280] = 0.0;
   out_5180136592465146401[281] = 0.0;
   out_5180136592465146401[282] = 0.0;
   out_5180136592465146401[283] = 0.0;
   out_5180136592465146401[284] = 0.0;
   out_5180136592465146401[285] = 1.0;
   out_5180136592465146401[286] = 0.0;
   out_5180136592465146401[287] = 0.0;
   out_5180136592465146401[288] = 0.0;
   out_5180136592465146401[289] = 0.0;
   out_5180136592465146401[290] = 0.0;
   out_5180136592465146401[291] = 0.0;
   out_5180136592465146401[292] = 0.0;
   out_5180136592465146401[293] = 0.0;
   out_5180136592465146401[294] = 0.0;
   out_5180136592465146401[295] = 0.0;
   out_5180136592465146401[296] = 0.0;
   out_5180136592465146401[297] = 0.0;
   out_5180136592465146401[298] = 0.0;
   out_5180136592465146401[299] = 0.0;
   out_5180136592465146401[300] = 0.0;
   out_5180136592465146401[301] = 0.0;
   out_5180136592465146401[302] = 0.0;
   out_5180136592465146401[303] = 0.0;
   out_5180136592465146401[304] = 1.0;
   out_5180136592465146401[305] = 0.0;
   out_5180136592465146401[306] = 0.0;
   out_5180136592465146401[307] = 0.0;
   out_5180136592465146401[308] = 0.0;
   out_5180136592465146401[309] = 0.0;
   out_5180136592465146401[310] = 0.0;
   out_5180136592465146401[311] = 0.0;
   out_5180136592465146401[312] = 0.0;
   out_5180136592465146401[313] = 0.0;
   out_5180136592465146401[314] = 0.0;
   out_5180136592465146401[315] = 0.0;
   out_5180136592465146401[316] = 0.0;
   out_5180136592465146401[317] = 0.0;
   out_5180136592465146401[318] = 0.0;
   out_5180136592465146401[319] = 0.0;
   out_5180136592465146401[320] = 0.0;
   out_5180136592465146401[321] = 0.0;
   out_5180136592465146401[322] = 0.0;
   out_5180136592465146401[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_8240783831830882114) {
   out_8240783831830882114[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_8240783831830882114[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_8240783831830882114[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_8240783831830882114[3] = dt*state[12] + state[3];
   out_8240783831830882114[4] = dt*state[13] + state[4];
   out_8240783831830882114[5] = dt*state[14] + state[5];
   out_8240783831830882114[6] = state[6];
   out_8240783831830882114[7] = state[7];
   out_8240783831830882114[8] = state[8];
   out_8240783831830882114[9] = state[9];
   out_8240783831830882114[10] = state[10];
   out_8240783831830882114[11] = state[11];
   out_8240783831830882114[12] = state[12];
   out_8240783831830882114[13] = state[13];
   out_8240783831830882114[14] = state[14];
   out_8240783831830882114[15] = state[15];
   out_8240783831830882114[16] = state[16];
   out_8240783831830882114[17] = state[17];
}
void F_fun(double *state, double dt, double *out_8106921975506752195) {
   out_8106921975506752195[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_8106921975506752195[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_8106921975506752195[2] = 0;
   out_8106921975506752195[3] = 0;
   out_8106921975506752195[4] = 0;
   out_8106921975506752195[5] = 0;
   out_8106921975506752195[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_8106921975506752195[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_8106921975506752195[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_8106921975506752195[9] = 0;
   out_8106921975506752195[10] = 0;
   out_8106921975506752195[11] = 0;
   out_8106921975506752195[12] = 0;
   out_8106921975506752195[13] = 0;
   out_8106921975506752195[14] = 0;
   out_8106921975506752195[15] = 0;
   out_8106921975506752195[16] = 0;
   out_8106921975506752195[17] = 0;
   out_8106921975506752195[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_8106921975506752195[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_8106921975506752195[20] = 0;
   out_8106921975506752195[21] = 0;
   out_8106921975506752195[22] = 0;
   out_8106921975506752195[23] = 0;
   out_8106921975506752195[24] = 0;
   out_8106921975506752195[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_8106921975506752195[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_8106921975506752195[27] = 0;
   out_8106921975506752195[28] = 0;
   out_8106921975506752195[29] = 0;
   out_8106921975506752195[30] = 0;
   out_8106921975506752195[31] = 0;
   out_8106921975506752195[32] = 0;
   out_8106921975506752195[33] = 0;
   out_8106921975506752195[34] = 0;
   out_8106921975506752195[35] = 0;
   out_8106921975506752195[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_8106921975506752195[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_8106921975506752195[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_8106921975506752195[39] = 0;
   out_8106921975506752195[40] = 0;
   out_8106921975506752195[41] = 0;
   out_8106921975506752195[42] = 0;
   out_8106921975506752195[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_8106921975506752195[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_8106921975506752195[45] = 0;
   out_8106921975506752195[46] = 0;
   out_8106921975506752195[47] = 0;
   out_8106921975506752195[48] = 0;
   out_8106921975506752195[49] = 0;
   out_8106921975506752195[50] = 0;
   out_8106921975506752195[51] = 0;
   out_8106921975506752195[52] = 0;
   out_8106921975506752195[53] = 0;
   out_8106921975506752195[54] = 0;
   out_8106921975506752195[55] = 0;
   out_8106921975506752195[56] = 0;
   out_8106921975506752195[57] = 1;
   out_8106921975506752195[58] = 0;
   out_8106921975506752195[59] = 0;
   out_8106921975506752195[60] = 0;
   out_8106921975506752195[61] = 0;
   out_8106921975506752195[62] = 0;
   out_8106921975506752195[63] = 0;
   out_8106921975506752195[64] = 0;
   out_8106921975506752195[65] = 0;
   out_8106921975506752195[66] = dt;
   out_8106921975506752195[67] = 0;
   out_8106921975506752195[68] = 0;
   out_8106921975506752195[69] = 0;
   out_8106921975506752195[70] = 0;
   out_8106921975506752195[71] = 0;
   out_8106921975506752195[72] = 0;
   out_8106921975506752195[73] = 0;
   out_8106921975506752195[74] = 0;
   out_8106921975506752195[75] = 0;
   out_8106921975506752195[76] = 1;
   out_8106921975506752195[77] = 0;
   out_8106921975506752195[78] = 0;
   out_8106921975506752195[79] = 0;
   out_8106921975506752195[80] = 0;
   out_8106921975506752195[81] = 0;
   out_8106921975506752195[82] = 0;
   out_8106921975506752195[83] = 0;
   out_8106921975506752195[84] = 0;
   out_8106921975506752195[85] = dt;
   out_8106921975506752195[86] = 0;
   out_8106921975506752195[87] = 0;
   out_8106921975506752195[88] = 0;
   out_8106921975506752195[89] = 0;
   out_8106921975506752195[90] = 0;
   out_8106921975506752195[91] = 0;
   out_8106921975506752195[92] = 0;
   out_8106921975506752195[93] = 0;
   out_8106921975506752195[94] = 0;
   out_8106921975506752195[95] = 1;
   out_8106921975506752195[96] = 0;
   out_8106921975506752195[97] = 0;
   out_8106921975506752195[98] = 0;
   out_8106921975506752195[99] = 0;
   out_8106921975506752195[100] = 0;
   out_8106921975506752195[101] = 0;
   out_8106921975506752195[102] = 0;
   out_8106921975506752195[103] = 0;
   out_8106921975506752195[104] = dt;
   out_8106921975506752195[105] = 0;
   out_8106921975506752195[106] = 0;
   out_8106921975506752195[107] = 0;
   out_8106921975506752195[108] = 0;
   out_8106921975506752195[109] = 0;
   out_8106921975506752195[110] = 0;
   out_8106921975506752195[111] = 0;
   out_8106921975506752195[112] = 0;
   out_8106921975506752195[113] = 0;
   out_8106921975506752195[114] = 1;
   out_8106921975506752195[115] = 0;
   out_8106921975506752195[116] = 0;
   out_8106921975506752195[117] = 0;
   out_8106921975506752195[118] = 0;
   out_8106921975506752195[119] = 0;
   out_8106921975506752195[120] = 0;
   out_8106921975506752195[121] = 0;
   out_8106921975506752195[122] = 0;
   out_8106921975506752195[123] = 0;
   out_8106921975506752195[124] = 0;
   out_8106921975506752195[125] = 0;
   out_8106921975506752195[126] = 0;
   out_8106921975506752195[127] = 0;
   out_8106921975506752195[128] = 0;
   out_8106921975506752195[129] = 0;
   out_8106921975506752195[130] = 0;
   out_8106921975506752195[131] = 0;
   out_8106921975506752195[132] = 0;
   out_8106921975506752195[133] = 1;
   out_8106921975506752195[134] = 0;
   out_8106921975506752195[135] = 0;
   out_8106921975506752195[136] = 0;
   out_8106921975506752195[137] = 0;
   out_8106921975506752195[138] = 0;
   out_8106921975506752195[139] = 0;
   out_8106921975506752195[140] = 0;
   out_8106921975506752195[141] = 0;
   out_8106921975506752195[142] = 0;
   out_8106921975506752195[143] = 0;
   out_8106921975506752195[144] = 0;
   out_8106921975506752195[145] = 0;
   out_8106921975506752195[146] = 0;
   out_8106921975506752195[147] = 0;
   out_8106921975506752195[148] = 0;
   out_8106921975506752195[149] = 0;
   out_8106921975506752195[150] = 0;
   out_8106921975506752195[151] = 0;
   out_8106921975506752195[152] = 1;
   out_8106921975506752195[153] = 0;
   out_8106921975506752195[154] = 0;
   out_8106921975506752195[155] = 0;
   out_8106921975506752195[156] = 0;
   out_8106921975506752195[157] = 0;
   out_8106921975506752195[158] = 0;
   out_8106921975506752195[159] = 0;
   out_8106921975506752195[160] = 0;
   out_8106921975506752195[161] = 0;
   out_8106921975506752195[162] = 0;
   out_8106921975506752195[163] = 0;
   out_8106921975506752195[164] = 0;
   out_8106921975506752195[165] = 0;
   out_8106921975506752195[166] = 0;
   out_8106921975506752195[167] = 0;
   out_8106921975506752195[168] = 0;
   out_8106921975506752195[169] = 0;
   out_8106921975506752195[170] = 0;
   out_8106921975506752195[171] = 1;
   out_8106921975506752195[172] = 0;
   out_8106921975506752195[173] = 0;
   out_8106921975506752195[174] = 0;
   out_8106921975506752195[175] = 0;
   out_8106921975506752195[176] = 0;
   out_8106921975506752195[177] = 0;
   out_8106921975506752195[178] = 0;
   out_8106921975506752195[179] = 0;
   out_8106921975506752195[180] = 0;
   out_8106921975506752195[181] = 0;
   out_8106921975506752195[182] = 0;
   out_8106921975506752195[183] = 0;
   out_8106921975506752195[184] = 0;
   out_8106921975506752195[185] = 0;
   out_8106921975506752195[186] = 0;
   out_8106921975506752195[187] = 0;
   out_8106921975506752195[188] = 0;
   out_8106921975506752195[189] = 0;
   out_8106921975506752195[190] = 1;
   out_8106921975506752195[191] = 0;
   out_8106921975506752195[192] = 0;
   out_8106921975506752195[193] = 0;
   out_8106921975506752195[194] = 0;
   out_8106921975506752195[195] = 0;
   out_8106921975506752195[196] = 0;
   out_8106921975506752195[197] = 0;
   out_8106921975506752195[198] = 0;
   out_8106921975506752195[199] = 0;
   out_8106921975506752195[200] = 0;
   out_8106921975506752195[201] = 0;
   out_8106921975506752195[202] = 0;
   out_8106921975506752195[203] = 0;
   out_8106921975506752195[204] = 0;
   out_8106921975506752195[205] = 0;
   out_8106921975506752195[206] = 0;
   out_8106921975506752195[207] = 0;
   out_8106921975506752195[208] = 0;
   out_8106921975506752195[209] = 1;
   out_8106921975506752195[210] = 0;
   out_8106921975506752195[211] = 0;
   out_8106921975506752195[212] = 0;
   out_8106921975506752195[213] = 0;
   out_8106921975506752195[214] = 0;
   out_8106921975506752195[215] = 0;
   out_8106921975506752195[216] = 0;
   out_8106921975506752195[217] = 0;
   out_8106921975506752195[218] = 0;
   out_8106921975506752195[219] = 0;
   out_8106921975506752195[220] = 0;
   out_8106921975506752195[221] = 0;
   out_8106921975506752195[222] = 0;
   out_8106921975506752195[223] = 0;
   out_8106921975506752195[224] = 0;
   out_8106921975506752195[225] = 0;
   out_8106921975506752195[226] = 0;
   out_8106921975506752195[227] = 0;
   out_8106921975506752195[228] = 1;
   out_8106921975506752195[229] = 0;
   out_8106921975506752195[230] = 0;
   out_8106921975506752195[231] = 0;
   out_8106921975506752195[232] = 0;
   out_8106921975506752195[233] = 0;
   out_8106921975506752195[234] = 0;
   out_8106921975506752195[235] = 0;
   out_8106921975506752195[236] = 0;
   out_8106921975506752195[237] = 0;
   out_8106921975506752195[238] = 0;
   out_8106921975506752195[239] = 0;
   out_8106921975506752195[240] = 0;
   out_8106921975506752195[241] = 0;
   out_8106921975506752195[242] = 0;
   out_8106921975506752195[243] = 0;
   out_8106921975506752195[244] = 0;
   out_8106921975506752195[245] = 0;
   out_8106921975506752195[246] = 0;
   out_8106921975506752195[247] = 1;
   out_8106921975506752195[248] = 0;
   out_8106921975506752195[249] = 0;
   out_8106921975506752195[250] = 0;
   out_8106921975506752195[251] = 0;
   out_8106921975506752195[252] = 0;
   out_8106921975506752195[253] = 0;
   out_8106921975506752195[254] = 0;
   out_8106921975506752195[255] = 0;
   out_8106921975506752195[256] = 0;
   out_8106921975506752195[257] = 0;
   out_8106921975506752195[258] = 0;
   out_8106921975506752195[259] = 0;
   out_8106921975506752195[260] = 0;
   out_8106921975506752195[261] = 0;
   out_8106921975506752195[262] = 0;
   out_8106921975506752195[263] = 0;
   out_8106921975506752195[264] = 0;
   out_8106921975506752195[265] = 0;
   out_8106921975506752195[266] = 1;
   out_8106921975506752195[267] = 0;
   out_8106921975506752195[268] = 0;
   out_8106921975506752195[269] = 0;
   out_8106921975506752195[270] = 0;
   out_8106921975506752195[271] = 0;
   out_8106921975506752195[272] = 0;
   out_8106921975506752195[273] = 0;
   out_8106921975506752195[274] = 0;
   out_8106921975506752195[275] = 0;
   out_8106921975506752195[276] = 0;
   out_8106921975506752195[277] = 0;
   out_8106921975506752195[278] = 0;
   out_8106921975506752195[279] = 0;
   out_8106921975506752195[280] = 0;
   out_8106921975506752195[281] = 0;
   out_8106921975506752195[282] = 0;
   out_8106921975506752195[283] = 0;
   out_8106921975506752195[284] = 0;
   out_8106921975506752195[285] = 1;
   out_8106921975506752195[286] = 0;
   out_8106921975506752195[287] = 0;
   out_8106921975506752195[288] = 0;
   out_8106921975506752195[289] = 0;
   out_8106921975506752195[290] = 0;
   out_8106921975506752195[291] = 0;
   out_8106921975506752195[292] = 0;
   out_8106921975506752195[293] = 0;
   out_8106921975506752195[294] = 0;
   out_8106921975506752195[295] = 0;
   out_8106921975506752195[296] = 0;
   out_8106921975506752195[297] = 0;
   out_8106921975506752195[298] = 0;
   out_8106921975506752195[299] = 0;
   out_8106921975506752195[300] = 0;
   out_8106921975506752195[301] = 0;
   out_8106921975506752195[302] = 0;
   out_8106921975506752195[303] = 0;
   out_8106921975506752195[304] = 1;
   out_8106921975506752195[305] = 0;
   out_8106921975506752195[306] = 0;
   out_8106921975506752195[307] = 0;
   out_8106921975506752195[308] = 0;
   out_8106921975506752195[309] = 0;
   out_8106921975506752195[310] = 0;
   out_8106921975506752195[311] = 0;
   out_8106921975506752195[312] = 0;
   out_8106921975506752195[313] = 0;
   out_8106921975506752195[314] = 0;
   out_8106921975506752195[315] = 0;
   out_8106921975506752195[316] = 0;
   out_8106921975506752195[317] = 0;
   out_8106921975506752195[318] = 0;
   out_8106921975506752195[319] = 0;
   out_8106921975506752195[320] = 0;
   out_8106921975506752195[321] = 0;
   out_8106921975506752195[322] = 0;
   out_8106921975506752195[323] = 1;
}
void h_4(double *state, double *unused, double *out_8584561754466655706) {
   out_8584561754466655706[0] = state[6] + state[9];
   out_8584561754466655706[1] = state[7] + state[10];
   out_8584561754466655706[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_1408329382621238829) {
   out_1408329382621238829[0] = 0;
   out_1408329382621238829[1] = 0;
   out_1408329382621238829[2] = 0;
   out_1408329382621238829[3] = 0;
   out_1408329382621238829[4] = 0;
   out_1408329382621238829[5] = 0;
   out_1408329382621238829[6] = 1;
   out_1408329382621238829[7] = 0;
   out_1408329382621238829[8] = 0;
   out_1408329382621238829[9] = 1;
   out_1408329382621238829[10] = 0;
   out_1408329382621238829[11] = 0;
   out_1408329382621238829[12] = 0;
   out_1408329382621238829[13] = 0;
   out_1408329382621238829[14] = 0;
   out_1408329382621238829[15] = 0;
   out_1408329382621238829[16] = 0;
   out_1408329382621238829[17] = 0;
   out_1408329382621238829[18] = 0;
   out_1408329382621238829[19] = 0;
   out_1408329382621238829[20] = 0;
   out_1408329382621238829[21] = 0;
   out_1408329382621238829[22] = 0;
   out_1408329382621238829[23] = 0;
   out_1408329382621238829[24] = 0;
   out_1408329382621238829[25] = 1;
   out_1408329382621238829[26] = 0;
   out_1408329382621238829[27] = 0;
   out_1408329382621238829[28] = 1;
   out_1408329382621238829[29] = 0;
   out_1408329382621238829[30] = 0;
   out_1408329382621238829[31] = 0;
   out_1408329382621238829[32] = 0;
   out_1408329382621238829[33] = 0;
   out_1408329382621238829[34] = 0;
   out_1408329382621238829[35] = 0;
   out_1408329382621238829[36] = 0;
   out_1408329382621238829[37] = 0;
   out_1408329382621238829[38] = 0;
   out_1408329382621238829[39] = 0;
   out_1408329382621238829[40] = 0;
   out_1408329382621238829[41] = 0;
   out_1408329382621238829[42] = 0;
   out_1408329382621238829[43] = 0;
   out_1408329382621238829[44] = 1;
   out_1408329382621238829[45] = 0;
   out_1408329382621238829[46] = 0;
   out_1408329382621238829[47] = 1;
   out_1408329382621238829[48] = 0;
   out_1408329382621238829[49] = 0;
   out_1408329382621238829[50] = 0;
   out_1408329382621238829[51] = 0;
   out_1408329382621238829[52] = 0;
   out_1408329382621238829[53] = 0;
}
void h_10(double *state, double *unused, double *out_8425326832718263788) {
   out_8425326832718263788[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_8425326832718263788[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_8425326832718263788[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_7742185847493432477) {
   out_7742185847493432477[0] = 0;
   out_7742185847493432477[1] = 9.8100000000000005*cos(state[1]);
   out_7742185847493432477[2] = 0;
   out_7742185847493432477[3] = 0;
   out_7742185847493432477[4] = -state[8];
   out_7742185847493432477[5] = state[7];
   out_7742185847493432477[6] = 0;
   out_7742185847493432477[7] = state[5];
   out_7742185847493432477[8] = -state[4];
   out_7742185847493432477[9] = 0;
   out_7742185847493432477[10] = 0;
   out_7742185847493432477[11] = 0;
   out_7742185847493432477[12] = 1;
   out_7742185847493432477[13] = 0;
   out_7742185847493432477[14] = 0;
   out_7742185847493432477[15] = 1;
   out_7742185847493432477[16] = 0;
   out_7742185847493432477[17] = 0;
   out_7742185847493432477[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_7742185847493432477[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_7742185847493432477[20] = 0;
   out_7742185847493432477[21] = state[8];
   out_7742185847493432477[22] = 0;
   out_7742185847493432477[23] = -state[6];
   out_7742185847493432477[24] = -state[5];
   out_7742185847493432477[25] = 0;
   out_7742185847493432477[26] = state[3];
   out_7742185847493432477[27] = 0;
   out_7742185847493432477[28] = 0;
   out_7742185847493432477[29] = 0;
   out_7742185847493432477[30] = 0;
   out_7742185847493432477[31] = 1;
   out_7742185847493432477[32] = 0;
   out_7742185847493432477[33] = 0;
   out_7742185847493432477[34] = 1;
   out_7742185847493432477[35] = 0;
   out_7742185847493432477[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_7742185847493432477[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_7742185847493432477[38] = 0;
   out_7742185847493432477[39] = -state[7];
   out_7742185847493432477[40] = state[6];
   out_7742185847493432477[41] = 0;
   out_7742185847493432477[42] = state[4];
   out_7742185847493432477[43] = -state[3];
   out_7742185847493432477[44] = 0;
   out_7742185847493432477[45] = 0;
   out_7742185847493432477[46] = 0;
   out_7742185847493432477[47] = 0;
   out_7742185847493432477[48] = 0;
   out_7742185847493432477[49] = 0;
   out_7742185847493432477[50] = 1;
   out_7742185847493432477[51] = 0;
   out_7742185847493432477[52] = 0;
   out_7742185847493432477[53] = 1;
}
void h_13(double *state, double *unused, double *out_3427524069920046808) {
   out_3427524069920046808[0] = state[3];
   out_3427524069920046808[1] = state[4];
   out_3427524069920046808[2] = state[5];
}
void H_13(double *state, double *unused, double *out_1803944442711093972) {
   out_1803944442711093972[0] = 0;
   out_1803944442711093972[1] = 0;
   out_1803944442711093972[2] = 0;
   out_1803944442711093972[3] = 1;
   out_1803944442711093972[4] = 0;
   out_1803944442711093972[5] = 0;
   out_1803944442711093972[6] = 0;
   out_1803944442711093972[7] = 0;
   out_1803944442711093972[8] = 0;
   out_1803944442711093972[9] = 0;
   out_1803944442711093972[10] = 0;
   out_1803944442711093972[11] = 0;
   out_1803944442711093972[12] = 0;
   out_1803944442711093972[13] = 0;
   out_1803944442711093972[14] = 0;
   out_1803944442711093972[15] = 0;
   out_1803944442711093972[16] = 0;
   out_1803944442711093972[17] = 0;
   out_1803944442711093972[18] = 0;
   out_1803944442711093972[19] = 0;
   out_1803944442711093972[20] = 0;
   out_1803944442711093972[21] = 0;
   out_1803944442711093972[22] = 1;
   out_1803944442711093972[23] = 0;
   out_1803944442711093972[24] = 0;
   out_1803944442711093972[25] = 0;
   out_1803944442711093972[26] = 0;
   out_1803944442711093972[27] = 0;
   out_1803944442711093972[28] = 0;
   out_1803944442711093972[29] = 0;
   out_1803944442711093972[30] = 0;
   out_1803944442711093972[31] = 0;
   out_1803944442711093972[32] = 0;
   out_1803944442711093972[33] = 0;
   out_1803944442711093972[34] = 0;
   out_1803944442711093972[35] = 0;
   out_1803944442711093972[36] = 0;
   out_1803944442711093972[37] = 0;
   out_1803944442711093972[38] = 0;
   out_1803944442711093972[39] = 0;
   out_1803944442711093972[40] = 0;
   out_1803944442711093972[41] = 1;
   out_1803944442711093972[42] = 0;
   out_1803944442711093972[43] = 0;
   out_1803944442711093972[44] = 0;
   out_1803944442711093972[45] = 0;
   out_1803944442711093972[46] = 0;
   out_1803944442711093972[47] = 0;
   out_1803944442711093972[48] = 0;
   out_1803944442711093972[49] = 0;
   out_1803944442711093972[50] = 0;
   out_1803944442711093972[51] = 0;
   out_1803944442711093972[52] = 0;
   out_1803944442711093972[53] = 0;
}
void h_14(double *state, double *unused, double *out_3360686240400799473) {
   out_3360686240400799473[0] = state[6];
   out_3360686240400799473[1] = state[7];
   out_3360686240400799473[2] = state[8];
}
void H_14(double *state, double *unused, double *out_2554911473718245700) {
   out_2554911473718245700[0] = 0;
   out_2554911473718245700[1] = 0;
   out_2554911473718245700[2] = 0;
   out_2554911473718245700[3] = 0;
   out_2554911473718245700[4] = 0;
   out_2554911473718245700[5] = 0;
   out_2554911473718245700[6] = 1;
   out_2554911473718245700[7] = 0;
   out_2554911473718245700[8] = 0;
   out_2554911473718245700[9] = 0;
   out_2554911473718245700[10] = 0;
   out_2554911473718245700[11] = 0;
   out_2554911473718245700[12] = 0;
   out_2554911473718245700[13] = 0;
   out_2554911473718245700[14] = 0;
   out_2554911473718245700[15] = 0;
   out_2554911473718245700[16] = 0;
   out_2554911473718245700[17] = 0;
   out_2554911473718245700[18] = 0;
   out_2554911473718245700[19] = 0;
   out_2554911473718245700[20] = 0;
   out_2554911473718245700[21] = 0;
   out_2554911473718245700[22] = 0;
   out_2554911473718245700[23] = 0;
   out_2554911473718245700[24] = 0;
   out_2554911473718245700[25] = 1;
   out_2554911473718245700[26] = 0;
   out_2554911473718245700[27] = 0;
   out_2554911473718245700[28] = 0;
   out_2554911473718245700[29] = 0;
   out_2554911473718245700[30] = 0;
   out_2554911473718245700[31] = 0;
   out_2554911473718245700[32] = 0;
   out_2554911473718245700[33] = 0;
   out_2554911473718245700[34] = 0;
   out_2554911473718245700[35] = 0;
   out_2554911473718245700[36] = 0;
   out_2554911473718245700[37] = 0;
   out_2554911473718245700[38] = 0;
   out_2554911473718245700[39] = 0;
   out_2554911473718245700[40] = 0;
   out_2554911473718245700[41] = 0;
   out_2554911473718245700[42] = 0;
   out_2554911473718245700[43] = 0;
   out_2554911473718245700[44] = 1;
   out_2554911473718245700[45] = 0;
   out_2554911473718245700[46] = 0;
   out_2554911473718245700[47] = 0;
   out_2554911473718245700[48] = 0;
   out_2554911473718245700[49] = 0;
   out_2554911473718245700[50] = 0;
   out_2554911473718245700[51] = 0;
   out_2554911473718245700[52] = 0;
   out_2554911473718245700[53] = 0;
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
void pose_err_fun(double *nom_x, double *delta_x, double *out_3175539824002796945) {
  err_fun(nom_x, delta_x, out_3175539824002796945);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_2251372369976179325) {
  inv_err_fun(nom_x, true_x, out_2251372369976179325);
}
void pose_H_mod_fun(double *state, double *out_5180136592465146401) {
  H_mod_fun(state, out_5180136592465146401);
}
void pose_f_fun(double *state, double dt, double *out_8240783831830882114) {
  f_fun(state,  dt, out_8240783831830882114);
}
void pose_F_fun(double *state, double dt, double *out_8106921975506752195) {
  F_fun(state,  dt, out_8106921975506752195);
}
void pose_h_4(double *state, double *unused, double *out_8584561754466655706) {
  h_4(state, unused, out_8584561754466655706);
}
void pose_H_4(double *state, double *unused, double *out_1408329382621238829) {
  H_4(state, unused, out_1408329382621238829);
}
void pose_h_10(double *state, double *unused, double *out_8425326832718263788) {
  h_10(state, unused, out_8425326832718263788);
}
void pose_H_10(double *state, double *unused, double *out_7742185847493432477) {
  H_10(state, unused, out_7742185847493432477);
}
void pose_h_13(double *state, double *unused, double *out_3427524069920046808) {
  h_13(state, unused, out_3427524069920046808);
}
void pose_H_13(double *state, double *unused, double *out_1803944442711093972) {
  H_13(state, unused, out_1803944442711093972);
}
void pose_h_14(double *state, double *unused, double *out_3360686240400799473) {
  h_14(state, unused, out_3360686240400799473);
}
void pose_H_14(double *state, double *unused, double *out_2554911473718245700) {
  H_14(state, unused, out_2554911473718245700);
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
