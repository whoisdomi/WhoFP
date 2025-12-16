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
void err_fun(double *nom_x, double *delta_x, double *out_4086162596914389259) {
   out_4086162596914389259[0] = delta_x[0] + nom_x[0];
   out_4086162596914389259[1] = delta_x[1] + nom_x[1];
   out_4086162596914389259[2] = delta_x[2] + nom_x[2];
   out_4086162596914389259[3] = delta_x[3] + nom_x[3];
   out_4086162596914389259[4] = delta_x[4] + nom_x[4];
   out_4086162596914389259[5] = delta_x[5] + nom_x[5];
   out_4086162596914389259[6] = delta_x[6] + nom_x[6];
   out_4086162596914389259[7] = delta_x[7] + nom_x[7];
   out_4086162596914389259[8] = delta_x[8] + nom_x[8];
   out_4086162596914389259[9] = delta_x[9] + nom_x[9];
   out_4086162596914389259[10] = delta_x[10] + nom_x[10];
   out_4086162596914389259[11] = delta_x[11] + nom_x[11];
   out_4086162596914389259[12] = delta_x[12] + nom_x[12];
   out_4086162596914389259[13] = delta_x[13] + nom_x[13];
   out_4086162596914389259[14] = delta_x[14] + nom_x[14];
   out_4086162596914389259[15] = delta_x[15] + nom_x[15];
   out_4086162596914389259[16] = delta_x[16] + nom_x[16];
   out_4086162596914389259[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_628093630908614818) {
   out_628093630908614818[0] = -nom_x[0] + true_x[0];
   out_628093630908614818[1] = -nom_x[1] + true_x[1];
   out_628093630908614818[2] = -nom_x[2] + true_x[2];
   out_628093630908614818[3] = -nom_x[3] + true_x[3];
   out_628093630908614818[4] = -nom_x[4] + true_x[4];
   out_628093630908614818[5] = -nom_x[5] + true_x[5];
   out_628093630908614818[6] = -nom_x[6] + true_x[6];
   out_628093630908614818[7] = -nom_x[7] + true_x[7];
   out_628093630908614818[8] = -nom_x[8] + true_x[8];
   out_628093630908614818[9] = -nom_x[9] + true_x[9];
   out_628093630908614818[10] = -nom_x[10] + true_x[10];
   out_628093630908614818[11] = -nom_x[11] + true_x[11];
   out_628093630908614818[12] = -nom_x[12] + true_x[12];
   out_628093630908614818[13] = -nom_x[13] + true_x[13];
   out_628093630908614818[14] = -nom_x[14] + true_x[14];
   out_628093630908614818[15] = -nom_x[15] + true_x[15];
   out_628093630908614818[16] = -nom_x[16] + true_x[16];
   out_628093630908614818[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_920885569904575623) {
   out_920885569904575623[0] = 1.0;
   out_920885569904575623[1] = 0.0;
   out_920885569904575623[2] = 0.0;
   out_920885569904575623[3] = 0.0;
   out_920885569904575623[4] = 0.0;
   out_920885569904575623[5] = 0.0;
   out_920885569904575623[6] = 0.0;
   out_920885569904575623[7] = 0.0;
   out_920885569904575623[8] = 0.0;
   out_920885569904575623[9] = 0.0;
   out_920885569904575623[10] = 0.0;
   out_920885569904575623[11] = 0.0;
   out_920885569904575623[12] = 0.0;
   out_920885569904575623[13] = 0.0;
   out_920885569904575623[14] = 0.0;
   out_920885569904575623[15] = 0.0;
   out_920885569904575623[16] = 0.0;
   out_920885569904575623[17] = 0.0;
   out_920885569904575623[18] = 0.0;
   out_920885569904575623[19] = 1.0;
   out_920885569904575623[20] = 0.0;
   out_920885569904575623[21] = 0.0;
   out_920885569904575623[22] = 0.0;
   out_920885569904575623[23] = 0.0;
   out_920885569904575623[24] = 0.0;
   out_920885569904575623[25] = 0.0;
   out_920885569904575623[26] = 0.0;
   out_920885569904575623[27] = 0.0;
   out_920885569904575623[28] = 0.0;
   out_920885569904575623[29] = 0.0;
   out_920885569904575623[30] = 0.0;
   out_920885569904575623[31] = 0.0;
   out_920885569904575623[32] = 0.0;
   out_920885569904575623[33] = 0.0;
   out_920885569904575623[34] = 0.0;
   out_920885569904575623[35] = 0.0;
   out_920885569904575623[36] = 0.0;
   out_920885569904575623[37] = 0.0;
   out_920885569904575623[38] = 1.0;
   out_920885569904575623[39] = 0.0;
   out_920885569904575623[40] = 0.0;
   out_920885569904575623[41] = 0.0;
   out_920885569904575623[42] = 0.0;
   out_920885569904575623[43] = 0.0;
   out_920885569904575623[44] = 0.0;
   out_920885569904575623[45] = 0.0;
   out_920885569904575623[46] = 0.0;
   out_920885569904575623[47] = 0.0;
   out_920885569904575623[48] = 0.0;
   out_920885569904575623[49] = 0.0;
   out_920885569904575623[50] = 0.0;
   out_920885569904575623[51] = 0.0;
   out_920885569904575623[52] = 0.0;
   out_920885569904575623[53] = 0.0;
   out_920885569904575623[54] = 0.0;
   out_920885569904575623[55] = 0.0;
   out_920885569904575623[56] = 0.0;
   out_920885569904575623[57] = 1.0;
   out_920885569904575623[58] = 0.0;
   out_920885569904575623[59] = 0.0;
   out_920885569904575623[60] = 0.0;
   out_920885569904575623[61] = 0.0;
   out_920885569904575623[62] = 0.0;
   out_920885569904575623[63] = 0.0;
   out_920885569904575623[64] = 0.0;
   out_920885569904575623[65] = 0.0;
   out_920885569904575623[66] = 0.0;
   out_920885569904575623[67] = 0.0;
   out_920885569904575623[68] = 0.0;
   out_920885569904575623[69] = 0.0;
   out_920885569904575623[70] = 0.0;
   out_920885569904575623[71] = 0.0;
   out_920885569904575623[72] = 0.0;
   out_920885569904575623[73] = 0.0;
   out_920885569904575623[74] = 0.0;
   out_920885569904575623[75] = 0.0;
   out_920885569904575623[76] = 1.0;
   out_920885569904575623[77] = 0.0;
   out_920885569904575623[78] = 0.0;
   out_920885569904575623[79] = 0.0;
   out_920885569904575623[80] = 0.0;
   out_920885569904575623[81] = 0.0;
   out_920885569904575623[82] = 0.0;
   out_920885569904575623[83] = 0.0;
   out_920885569904575623[84] = 0.0;
   out_920885569904575623[85] = 0.0;
   out_920885569904575623[86] = 0.0;
   out_920885569904575623[87] = 0.0;
   out_920885569904575623[88] = 0.0;
   out_920885569904575623[89] = 0.0;
   out_920885569904575623[90] = 0.0;
   out_920885569904575623[91] = 0.0;
   out_920885569904575623[92] = 0.0;
   out_920885569904575623[93] = 0.0;
   out_920885569904575623[94] = 0.0;
   out_920885569904575623[95] = 1.0;
   out_920885569904575623[96] = 0.0;
   out_920885569904575623[97] = 0.0;
   out_920885569904575623[98] = 0.0;
   out_920885569904575623[99] = 0.0;
   out_920885569904575623[100] = 0.0;
   out_920885569904575623[101] = 0.0;
   out_920885569904575623[102] = 0.0;
   out_920885569904575623[103] = 0.0;
   out_920885569904575623[104] = 0.0;
   out_920885569904575623[105] = 0.0;
   out_920885569904575623[106] = 0.0;
   out_920885569904575623[107] = 0.0;
   out_920885569904575623[108] = 0.0;
   out_920885569904575623[109] = 0.0;
   out_920885569904575623[110] = 0.0;
   out_920885569904575623[111] = 0.0;
   out_920885569904575623[112] = 0.0;
   out_920885569904575623[113] = 0.0;
   out_920885569904575623[114] = 1.0;
   out_920885569904575623[115] = 0.0;
   out_920885569904575623[116] = 0.0;
   out_920885569904575623[117] = 0.0;
   out_920885569904575623[118] = 0.0;
   out_920885569904575623[119] = 0.0;
   out_920885569904575623[120] = 0.0;
   out_920885569904575623[121] = 0.0;
   out_920885569904575623[122] = 0.0;
   out_920885569904575623[123] = 0.0;
   out_920885569904575623[124] = 0.0;
   out_920885569904575623[125] = 0.0;
   out_920885569904575623[126] = 0.0;
   out_920885569904575623[127] = 0.0;
   out_920885569904575623[128] = 0.0;
   out_920885569904575623[129] = 0.0;
   out_920885569904575623[130] = 0.0;
   out_920885569904575623[131] = 0.0;
   out_920885569904575623[132] = 0.0;
   out_920885569904575623[133] = 1.0;
   out_920885569904575623[134] = 0.0;
   out_920885569904575623[135] = 0.0;
   out_920885569904575623[136] = 0.0;
   out_920885569904575623[137] = 0.0;
   out_920885569904575623[138] = 0.0;
   out_920885569904575623[139] = 0.0;
   out_920885569904575623[140] = 0.0;
   out_920885569904575623[141] = 0.0;
   out_920885569904575623[142] = 0.0;
   out_920885569904575623[143] = 0.0;
   out_920885569904575623[144] = 0.0;
   out_920885569904575623[145] = 0.0;
   out_920885569904575623[146] = 0.0;
   out_920885569904575623[147] = 0.0;
   out_920885569904575623[148] = 0.0;
   out_920885569904575623[149] = 0.0;
   out_920885569904575623[150] = 0.0;
   out_920885569904575623[151] = 0.0;
   out_920885569904575623[152] = 1.0;
   out_920885569904575623[153] = 0.0;
   out_920885569904575623[154] = 0.0;
   out_920885569904575623[155] = 0.0;
   out_920885569904575623[156] = 0.0;
   out_920885569904575623[157] = 0.0;
   out_920885569904575623[158] = 0.0;
   out_920885569904575623[159] = 0.0;
   out_920885569904575623[160] = 0.0;
   out_920885569904575623[161] = 0.0;
   out_920885569904575623[162] = 0.0;
   out_920885569904575623[163] = 0.0;
   out_920885569904575623[164] = 0.0;
   out_920885569904575623[165] = 0.0;
   out_920885569904575623[166] = 0.0;
   out_920885569904575623[167] = 0.0;
   out_920885569904575623[168] = 0.0;
   out_920885569904575623[169] = 0.0;
   out_920885569904575623[170] = 0.0;
   out_920885569904575623[171] = 1.0;
   out_920885569904575623[172] = 0.0;
   out_920885569904575623[173] = 0.0;
   out_920885569904575623[174] = 0.0;
   out_920885569904575623[175] = 0.0;
   out_920885569904575623[176] = 0.0;
   out_920885569904575623[177] = 0.0;
   out_920885569904575623[178] = 0.0;
   out_920885569904575623[179] = 0.0;
   out_920885569904575623[180] = 0.0;
   out_920885569904575623[181] = 0.0;
   out_920885569904575623[182] = 0.0;
   out_920885569904575623[183] = 0.0;
   out_920885569904575623[184] = 0.0;
   out_920885569904575623[185] = 0.0;
   out_920885569904575623[186] = 0.0;
   out_920885569904575623[187] = 0.0;
   out_920885569904575623[188] = 0.0;
   out_920885569904575623[189] = 0.0;
   out_920885569904575623[190] = 1.0;
   out_920885569904575623[191] = 0.0;
   out_920885569904575623[192] = 0.0;
   out_920885569904575623[193] = 0.0;
   out_920885569904575623[194] = 0.0;
   out_920885569904575623[195] = 0.0;
   out_920885569904575623[196] = 0.0;
   out_920885569904575623[197] = 0.0;
   out_920885569904575623[198] = 0.0;
   out_920885569904575623[199] = 0.0;
   out_920885569904575623[200] = 0.0;
   out_920885569904575623[201] = 0.0;
   out_920885569904575623[202] = 0.0;
   out_920885569904575623[203] = 0.0;
   out_920885569904575623[204] = 0.0;
   out_920885569904575623[205] = 0.0;
   out_920885569904575623[206] = 0.0;
   out_920885569904575623[207] = 0.0;
   out_920885569904575623[208] = 0.0;
   out_920885569904575623[209] = 1.0;
   out_920885569904575623[210] = 0.0;
   out_920885569904575623[211] = 0.0;
   out_920885569904575623[212] = 0.0;
   out_920885569904575623[213] = 0.0;
   out_920885569904575623[214] = 0.0;
   out_920885569904575623[215] = 0.0;
   out_920885569904575623[216] = 0.0;
   out_920885569904575623[217] = 0.0;
   out_920885569904575623[218] = 0.0;
   out_920885569904575623[219] = 0.0;
   out_920885569904575623[220] = 0.0;
   out_920885569904575623[221] = 0.0;
   out_920885569904575623[222] = 0.0;
   out_920885569904575623[223] = 0.0;
   out_920885569904575623[224] = 0.0;
   out_920885569904575623[225] = 0.0;
   out_920885569904575623[226] = 0.0;
   out_920885569904575623[227] = 0.0;
   out_920885569904575623[228] = 1.0;
   out_920885569904575623[229] = 0.0;
   out_920885569904575623[230] = 0.0;
   out_920885569904575623[231] = 0.0;
   out_920885569904575623[232] = 0.0;
   out_920885569904575623[233] = 0.0;
   out_920885569904575623[234] = 0.0;
   out_920885569904575623[235] = 0.0;
   out_920885569904575623[236] = 0.0;
   out_920885569904575623[237] = 0.0;
   out_920885569904575623[238] = 0.0;
   out_920885569904575623[239] = 0.0;
   out_920885569904575623[240] = 0.0;
   out_920885569904575623[241] = 0.0;
   out_920885569904575623[242] = 0.0;
   out_920885569904575623[243] = 0.0;
   out_920885569904575623[244] = 0.0;
   out_920885569904575623[245] = 0.0;
   out_920885569904575623[246] = 0.0;
   out_920885569904575623[247] = 1.0;
   out_920885569904575623[248] = 0.0;
   out_920885569904575623[249] = 0.0;
   out_920885569904575623[250] = 0.0;
   out_920885569904575623[251] = 0.0;
   out_920885569904575623[252] = 0.0;
   out_920885569904575623[253] = 0.0;
   out_920885569904575623[254] = 0.0;
   out_920885569904575623[255] = 0.0;
   out_920885569904575623[256] = 0.0;
   out_920885569904575623[257] = 0.0;
   out_920885569904575623[258] = 0.0;
   out_920885569904575623[259] = 0.0;
   out_920885569904575623[260] = 0.0;
   out_920885569904575623[261] = 0.0;
   out_920885569904575623[262] = 0.0;
   out_920885569904575623[263] = 0.0;
   out_920885569904575623[264] = 0.0;
   out_920885569904575623[265] = 0.0;
   out_920885569904575623[266] = 1.0;
   out_920885569904575623[267] = 0.0;
   out_920885569904575623[268] = 0.0;
   out_920885569904575623[269] = 0.0;
   out_920885569904575623[270] = 0.0;
   out_920885569904575623[271] = 0.0;
   out_920885569904575623[272] = 0.0;
   out_920885569904575623[273] = 0.0;
   out_920885569904575623[274] = 0.0;
   out_920885569904575623[275] = 0.0;
   out_920885569904575623[276] = 0.0;
   out_920885569904575623[277] = 0.0;
   out_920885569904575623[278] = 0.0;
   out_920885569904575623[279] = 0.0;
   out_920885569904575623[280] = 0.0;
   out_920885569904575623[281] = 0.0;
   out_920885569904575623[282] = 0.0;
   out_920885569904575623[283] = 0.0;
   out_920885569904575623[284] = 0.0;
   out_920885569904575623[285] = 1.0;
   out_920885569904575623[286] = 0.0;
   out_920885569904575623[287] = 0.0;
   out_920885569904575623[288] = 0.0;
   out_920885569904575623[289] = 0.0;
   out_920885569904575623[290] = 0.0;
   out_920885569904575623[291] = 0.0;
   out_920885569904575623[292] = 0.0;
   out_920885569904575623[293] = 0.0;
   out_920885569904575623[294] = 0.0;
   out_920885569904575623[295] = 0.0;
   out_920885569904575623[296] = 0.0;
   out_920885569904575623[297] = 0.0;
   out_920885569904575623[298] = 0.0;
   out_920885569904575623[299] = 0.0;
   out_920885569904575623[300] = 0.0;
   out_920885569904575623[301] = 0.0;
   out_920885569904575623[302] = 0.0;
   out_920885569904575623[303] = 0.0;
   out_920885569904575623[304] = 1.0;
   out_920885569904575623[305] = 0.0;
   out_920885569904575623[306] = 0.0;
   out_920885569904575623[307] = 0.0;
   out_920885569904575623[308] = 0.0;
   out_920885569904575623[309] = 0.0;
   out_920885569904575623[310] = 0.0;
   out_920885569904575623[311] = 0.0;
   out_920885569904575623[312] = 0.0;
   out_920885569904575623[313] = 0.0;
   out_920885569904575623[314] = 0.0;
   out_920885569904575623[315] = 0.0;
   out_920885569904575623[316] = 0.0;
   out_920885569904575623[317] = 0.0;
   out_920885569904575623[318] = 0.0;
   out_920885569904575623[319] = 0.0;
   out_920885569904575623[320] = 0.0;
   out_920885569904575623[321] = 0.0;
   out_920885569904575623[322] = 0.0;
   out_920885569904575623[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_4638827357690861687) {
   out_4638827357690861687[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_4638827357690861687[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_4638827357690861687[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_4638827357690861687[3] = dt*state[12] + state[3];
   out_4638827357690861687[4] = dt*state[13] + state[4];
   out_4638827357690861687[5] = dt*state[14] + state[5];
   out_4638827357690861687[6] = state[6];
   out_4638827357690861687[7] = state[7];
   out_4638827357690861687[8] = state[8];
   out_4638827357690861687[9] = state[9];
   out_4638827357690861687[10] = state[10];
   out_4638827357690861687[11] = state[11];
   out_4638827357690861687[12] = state[12];
   out_4638827357690861687[13] = state[13];
   out_4638827357690861687[14] = state[14];
   out_4638827357690861687[15] = state[15];
   out_4638827357690861687[16] = state[16];
   out_4638827357690861687[17] = state[17];
}
void F_fun(double *state, double dt, double *out_2122913628783415921) {
   out_2122913628783415921[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_2122913628783415921[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_2122913628783415921[2] = 0;
   out_2122913628783415921[3] = 0;
   out_2122913628783415921[4] = 0;
   out_2122913628783415921[5] = 0;
   out_2122913628783415921[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_2122913628783415921[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_2122913628783415921[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_2122913628783415921[9] = 0;
   out_2122913628783415921[10] = 0;
   out_2122913628783415921[11] = 0;
   out_2122913628783415921[12] = 0;
   out_2122913628783415921[13] = 0;
   out_2122913628783415921[14] = 0;
   out_2122913628783415921[15] = 0;
   out_2122913628783415921[16] = 0;
   out_2122913628783415921[17] = 0;
   out_2122913628783415921[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_2122913628783415921[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_2122913628783415921[20] = 0;
   out_2122913628783415921[21] = 0;
   out_2122913628783415921[22] = 0;
   out_2122913628783415921[23] = 0;
   out_2122913628783415921[24] = 0;
   out_2122913628783415921[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_2122913628783415921[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_2122913628783415921[27] = 0;
   out_2122913628783415921[28] = 0;
   out_2122913628783415921[29] = 0;
   out_2122913628783415921[30] = 0;
   out_2122913628783415921[31] = 0;
   out_2122913628783415921[32] = 0;
   out_2122913628783415921[33] = 0;
   out_2122913628783415921[34] = 0;
   out_2122913628783415921[35] = 0;
   out_2122913628783415921[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_2122913628783415921[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_2122913628783415921[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_2122913628783415921[39] = 0;
   out_2122913628783415921[40] = 0;
   out_2122913628783415921[41] = 0;
   out_2122913628783415921[42] = 0;
   out_2122913628783415921[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_2122913628783415921[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_2122913628783415921[45] = 0;
   out_2122913628783415921[46] = 0;
   out_2122913628783415921[47] = 0;
   out_2122913628783415921[48] = 0;
   out_2122913628783415921[49] = 0;
   out_2122913628783415921[50] = 0;
   out_2122913628783415921[51] = 0;
   out_2122913628783415921[52] = 0;
   out_2122913628783415921[53] = 0;
   out_2122913628783415921[54] = 0;
   out_2122913628783415921[55] = 0;
   out_2122913628783415921[56] = 0;
   out_2122913628783415921[57] = 1;
   out_2122913628783415921[58] = 0;
   out_2122913628783415921[59] = 0;
   out_2122913628783415921[60] = 0;
   out_2122913628783415921[61] = 0;
   out_2122913628783415921[62] = 0;
   out_2122913628783415921[63] = 0;
   out_2122913628783415921[64] = 0;
   out_2122913628783415921[65] = 0;
   out_2122913628783415921[66] = dt;
   out_2122913628783415921[67] = 0;
   out_2122913628783415921[68] = 0;
   out_2122913628783415921[69] = 0;
   out_2122913628783415921[70] = 0;
   out_2122913628783415921[71] = 0;
   out_2122913628783415921[72] = 0;
   out_2122913628783415921[73] = 0;
   out_2122913628783415921[74] = 0;
   out_2122913628783415921[75] = 0;
   out_2122913628783415921[76] = 1;
   out_2122913628783415921[77] = 0;
   out_2122913628783415921[78] = 0;
   out_2122913628783415921[79] = 0;
   out_2122913628783415921[80] = 0;
   out_2122913628783415921[81] = 0;
   out_2122913628783415921[82] = 0;
   out_2122913628783415921[83] = 0;
   out_2122913628783415921[84] = 0;
   out_2122913628783415921[85] = dt;
   out_2122913628783415921[86] = 0;
   out_2122913628783415921[87] = 0;
   out_2122913628783415921[88] = 0;
   out_2122913628783415921[89] = 0;
   out_2122913628783415921[90] = 0;
   out_2122913628783415921[91] = 0;
   out_2122913628783415921[92] = 0;
   out_2122913628783415921[93] = 0;
   out_2122913628783415921[94] = 0;
   out_2122913628783415921[95] = 1;
   out_2122913628783415921[96] = 0;
   out_2122913628783415921[97] = 0;
   out_2122913628783415921[98] = 0;
   out_2122913628783415921[99] = 0;
   out_2122913628783415921[100] = 0;
   out_2122913628783415921[101] = 0;
   out_2122913628783415921[102] = 0;
   out_2122913628783415921[103] = 0;
   out_2122913628783415921[104] = dt;
   out_2122913628783415921[105] = 0;
   out_2122913628783415921[106] = 0;
   out_2122913628783415921[107] = 0;
   out_2122913628783415921[108] = 0;
   out_2122913628783415921[109] = 0;
   out_2122913628783415921[110] = 0;
   out_2122913628783415921[111] = 0;
   out_2122913628783415921[112] = 0;
   out_2122913628783415921[113] = 0;
   out_2122913628783415921[114] = 1;
   out_2122913628783415921[115] = 0;
   out_2122913628783415921[116] = 0;
   out_2122913628783415921[117] = 0;
   out_2122913628783415921[118] = 0;
   out_2122913628783415921[119] = 0;
   out_2122913628783415921[120] = 0;
   out_2122913628783415921[121] = 0;
   out_2122913628783415921[122] = 0;
   out_2122913628783415921[123] = 0;
   out_2122913628783415921[124] = 0;
   out_2122913628783415921[125] = 0;
   out_2122913628783415921[126] = 0;
   out_2122913628783415921[127] = 0;
   out_2122913628783415921[128] = 0;
   out_2122913628783415921[129] = 0;
   out_2122913628783415921[130] = 0;
   out_2122913628783415921[131] = 0;
   out_2122913628783415921[132] = 0;
   out_2122913628783415921[133] = 1;
   out_2122913628783415921[134] = 0;
   out_2122913628783415921[135] = 0;
   out_2122913628783415921[136] = 0;
   out_2122913628783415921[137] = 0;
   out_2122913628783415921[138] = 0;
   out_2122913628783415921[139] = 0;
   out_2122913628783415921[140] = 0;
   out_2122913628783415921[141] = 0;
   out_2122913628783415921[142] = 0;
   out_2122913628783415921[143] = 0;
   out_2122913628783415921[144] = 0;
   out_2122913628783415921[145] = 0;
   out_2122913628783415921[146] = 0;
   out_2122913628783415921[147] = 0;
   out_2122913628783415921[148] = 0;
   out_2122913628783415921[149] = 0;
   out_2122913628783415921[150] = 0;
   out_2122913628783415921[151] = 0;
   out_2122913628783415921[152] = 1;
   out_2122913628783415921[153] = 0;
   out_2122913628783415921[154] = 0;
   out_2122913628783415921[155] = 0;
   out_2122913628783415921[156] = 0;
   out_2122913628783415921[157] = 0;
   out_2122913628783415921[158] = 0;
   out_2122913628783415921[159] = 0;
   out_2122913628783415921[160] = 0;
   out_2122913628783415921[161] = 0;
   out_2122913628783415921[162] = 0;
   out_2122913628783415921[163] = 0;
   out_2122913628783415921[164] = 0;
   out_2122913628783415921[165] = 0;
   out_2122913628783415921[166] = 0;
   out_2122913628783415921[167] = 0;
   out_2122913628783415921[168] = 0;
   out_2122913628783415921[169] = 0;
   out_2122913628783415921[170] = 0;
   out_2122913628783415921[171] = 1;
   out_2122913628783415921[172] = 0;
   out_2122913628783415921[173] = 0;
   out_2122913628783415921[174] = 0;
   out_2122913628783415921[175] = 0;
   out_2122913628783415921[176] = 0;
   out_2122913628783415921[177] = 0;
   out_2122913628783415921[178] = 0;
   out_2122913628783415921[179] = 0;
   out_2122913628783415921[180] = 0;
   out_2122913628783415921[181] = 0;
   out_2122913628783415921[182] = 0;
   out_2122913628783415921[183] = 0;
   out_2122913628783415921[184] = 0;
   out_2122913628783415921[185] = 0;
   out_2122913628783415921[186] = 0;
   out_2122913628783415921[187] = 0;
   out_2122913628783415921[188] = 0;
   out_2122913628783415921[189] = 0;
   out_2122913628783415921[190] = 1;
   out_2122913628783415921[191] = 0;
   out_2122913628783415921[192] = 0;
   out_2122913628783415921[193] = 0;
   out_2122913628783415921[194] = 0;
   out_2122913628783415921[195] = 0;
   out_2122913628783415921[196] = 0;
   out_2122913628783415921[197] = 0;
   out_2122913628783415921[198] = 0;
   out_2122913628783415921[199] = 0;
   out_2122913628783415921[200] = 0;
   out_2122913628783415921[201] = 0;
   out_2122913628783415921[202] = 0;
   out_2122913628783415921[203] = 0;
   out_2122913628783415921[204] = 0;
   out_2122913628783415921[205] = 0;
   out_2122913628783415921[206] = 0;
   out_2122913628783415921[207] = 0;
   out_2122913628783415921[208] = 0;
   out_2122913628783415921[209] = 1;
   out_2122913628783415921[210] = 0;
   out_2122913628783415921[211] = 0;
   out_2122913628783415921[212] = 0;
   out_2122913628783415921[213] = 0;
   out_2122913628783415921[214] = 0;
   out_2122913628783415921[215] = 0;
   out_2122913628783415921[216] = 0;
   out_2122913628783415921[217] = 0;
   out_2122913628783415921[218] = 0;
   out_2122913628783415921[219] = 0;
   out_2122913628783415921[220] = 0;
   out_2122913628783415921[221] = 0;
   out_2122913628783415921[222] = 0;
   out_2122913628783415921[223] = 0;
   out_2122913628783415921[224] = 0;
   out_2122913628783415921[225] = 0;
   out_2122913628783415921[226] = 0;
   out_2122913628783415921[227] = 0;
   out_2122913628783415921[228] = 1;
   out_2122913628783415921[229] = 0;
   out_2122913628783415921[230] = 0;
   out_2122913628783415921[231] = 0;
   out_2122913628783415921[232] = 0;
   out_2122913628783415921[233] = 0;
   out_2122913628783415921[234] = 0;
   out_2122913628783415921[235] = 0;
   out_2122913628783415921[236] = 0;
   out_2122913628783415921[237] = 0;
   out_2122913628783415921[238] = 0;
   out_2122913628783415921[239] = 0;
   out_2122913628783415921[240] = 0;
   out_2122913628783415921[241] = 0;
   out_2122913628783415921[242] = 0;
   out_2122913628783415921[243] = 0;
   out_2122913628783415921[244] = 0;
   out_2122913628783415921[245] = 0;
   out_2122913628783415921[246] = 0;
   out_2122913628783415921[247] = 1;
   out_2122913628783415921[248] = 0;
   out_2122913628783415921[249] = 0;
   out_2122913628783415921[250] = 0;
   out_2122913628783415921[251] = 0;
   out_2122913628783415921[252] = 0;
   out_2122913628783415921[253] = 0;
   out_2122913628783415921[254] = 0;
   out_2122913628783415921[255] = 0;
   out_2122913628783415921[256] = 0;
   out_2122913628783415921[257] = 0;
   out_2122913628783415921[258] = 0;
   out_2122913628783415921[259] = 0;
   out_2122913628783415921[260] = 0;
   out_2122913628783415921[261] = 0;
   out_2122913628783415921[262] = 0;
   out_2122913628783415921[263] = 0;
   out_2122913628783415921[264] = 0;
   out_2122913628783415921[265] = 0;
   out_2122913628783415921[266] = 1;
   out_2122913628783415921[267] = 0;
   out_2122913628783415921[268] = 0;
   out_2122913628783415921[269] = 0;
   out_2122913628783415921[270] = 0;
   out_2122913628783415921[271] = 0;
   out_2122913628783415921[272] = 0;
   out_2122913628783415921[273] = 0;
   out_2122913628783415921[274] = 0;
   out_2122913628783415921[275] = 0;
   out_2122913628783415921[276] = 0;
   out_2122913628783415921[277] = 0;
   out_2122913628783415921[278] = 0;
   out_2122913628783415921[279] = 0;
   out_2122913628783415921[280] = 0;
   out_2122913628783415921[281] = 0;
   out_2122913628783415921[282] = 0;
   out_2122913628783415921[283] = 0;
   out_2122913628783415921[284] = 0;
   out_2122913628783415921[285] = 1;
   out_2122913628783415921[286] = 0;
   out_2122913628783415921[287] = 0;
   out_2122913628783415921[288] = 0;
   out_2122913628783415921[289] = 0;
   out_2122913628783415921[290] = 0;
   out_2122913628783415921[291] = 0;
   out_2122913628783415921[292] = 0;
   out_2122913628783415921[293] = 0;
   out_2122913628783415921[294] = 0;
   out_2122913628783415921[295] = 0;
   out_2122913628783415921[296] = 0;
   out_2122913628783415921[297] = 0;
   out_2122913628783415921[298] = 0;
   out_2122913628783415921[299] = 0;
   out_2122913628783415921[300] = 0;
   out_2122913628783415921[301] = 0;
   out_2122913628783415921[302] = 0;
   out_2122913628783415921[303] = 0;
   out_2122913628783415921[304] = 1;
   out_2122913628783415921[305] = 0;
   out_2122913628783415921[306] = 0;
   out_2122913628783415921[307] = 0;
   out_2122913628783415921[308] = 0;
   out_2122913628783415921[309] = 0;
   out_2122913628783415921[310] = 0;
   out_2122913628783415921[311] = 0;
   out_2122913628783415921[312] = 0;
   out_2122913628783415921[313] = 0;
   out_2122913628783415921[314] = 0;
   out_2122913628783415921[315] = 0;
   out_2122913628783415921[316] = 0;
   out_2122913628783415921[317] = 0;
   out_2122913628783415921[318] = 0;
   out_2122913628783415921[319] = 0;
   out_2122913628783415921[320] = 0;
   out_2122913628783415921[321] = 0;
   out_2122913628783415921[322] = 0;
   out_2122913628783415921[323] = 1;
}
void h_4(double *state, double *unused, double *out_8610552428979210253) {
   out_8610552428979210253[0] = state[6] + state[9];
   out_8610552428979210253[1] = state[7] + state[10];
   out_8610552428979210253[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_898813156001721238) {
   out_898813156001721238[0] = 0;
   out_898813156001721238[1] = 0;
   out_898813156001721238[2] = 0;
   out_898813156001721238[3] = 0;
   out_898813156001721238[4] = 0;
   out_898813156001721238[5] = 0;
   out_898813156001721238[6] = 1;
   out_898813156001721238[7] = 0;
   out_898813156001721238[8] = 0;
   out_898813156001721238[9] = 1;
   out_898813156001721238[10] = 0;
   out_898813156001721238[11] = 0;
   out_898813156001721238[12] = 0;
   out_898813156001721238[13] = 0;
   out_898813156001721238[14] = 0;
   out_898813156001721238[15] = 0;
   out_898813156001721238[16] = 0;
   out_898813156001721238[17] = 0;
   out_898813156001721238[18] = 0;
   out_898813156001721238[19] = 0;
   out_898813156001721238[20] = 0;
   out_898813156001721238[21] = 0;
   out_898813156001721238[22] = 0;
   out_898813156001721238[23] = 0;
   out_898813156001721238[24] = 0;
   out_898813156001721238[25] = 1;
   out_898813156001721238[26] = 0;
   out_898813156001721238[27] = 0;
   out_898813156001721238[28] = 1;
   out_898813156001721238[29] = 0;
   out_898813156001721238[30] = 0;
   out_898813156001721238[31] = 0;
   out_898813156001721238[32] = 0;
   out_898813156001721238[33] = 0;
   out_898813156001721238[34] = 0;
   out_898813156001721238[35] = 0;
   out_898813156001721238[36] = 0;
   out_898813156001721238[37] = 0;
   out_898813156001721238[38] = 0;
   out_898813156001721238[39] = 0;
   out_898813156001721238[40] = 0;
   out_898813156001721238[41] = 0;
   out_898813156001721238[42] = 0;
   out_898813156001721238[43] = 0;
   out_898813156001721238[44] = 1;
   out_898813156001721238[45] = 0;
   out_898813156001721238[46] = 0;
   out_898813156001721238[47] = 1;
   out_898813156001721238[48] = 0;
   out_898813156001721238[49] = 0;
   out_898813156001721238[50] = 0;
   out_898813156001721238[51] = 0;
   out_898813156001721238[52] = 0;
   out_898813156001721238[53] = 0;
}
void h_10(double *state, double *unused, double *out_6115253449497749874) {
   out_6115253449497749874[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_6115253449497749874[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_6115253449497749874[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_795420154396972986) {
   out_795420154396972986[0] = 0;
   out_795420154396972986[1] = 9.8100000000000005*cos(state[1]);
   out_795420154396972986[2] = 0;
   out_795420154396972986[3] = 0;
   out_795420154396972986[4] = -state[8];
   out_795420154396972986[5] = state[7];
   out_795420154396972986[6] = 0;
   out_795420154396972986[7] = state[5];
   out_795420154396972986[8] = -state[4];
   out_795420154396972986[9] = 0;
   out_795420154396972986[10] = 0;
   out_795420154396972986[11] = 0;
   out_795420154396972986[12] = 1;
   out_795420154396972986[13] = 0;
   out_795420154396972986[14] = 0;
   out_795420154396972986[15] = 1;
   out_795420154396972986[16] = 0;
   out_795420154396972986[17] = 0;
   out_795420154396972986[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_795420154396972986[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_795420154396972986[20] = 0;
   out_795420154396972986[21] = state[8];
   out_795420154396972986[22] = 0;
   out_795420154396972986[23] = -state[6];
   out_795420154396972986[24] = -state[5];
   out_795420154396972986[25] = 0;
   out_795420154396972986[26] = state[3];
   out_795420154396972986[27] = 0;
   out_795420154396972986[28] = 0;
   out_795420154396972986[29] = 0;
   out_795420154396972986[30] = 0;
   out_795420154396972986[31] = 1;
   out_795420154396972986[32] = 0;
   out_795420154396972986[33] = 0;
   out_795420154396972986[34] = 1;
   out_795420154396972986[35] = 0;
   out_795420154396972986[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_795420154396972986[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_795420154396972986[38] = 0;
   out_795420154396972986[39] = -state[7];
   out_795420154396972986[40] = state[6];
   out_795420154396972986[41] = 0;
   out_795420154396972986[42] = state[4];
   out_795420154396972986[43] = -state[3];
   out_795420154396972986[44] = 0;
   out_795420154396972986[45] = 0;
   out_795420154396972986[46] = 0;
   out_795420154396972986[47] = 0;
   out_795420154396972986[48] = 0;
   out_795420154396972986[49] = 0;
   out_795420154396972986[50] = 1;
   out_795420154396972986[51] = 0;
   out_795420154396972986[52] = 0;
   out_795420154396972986[53] = 1;
}
void h_13(double *state, double *unused, double *out_7913228155057385150) {
   out_7913228155057385150[0] = state[3];
   out_7913228155057385150[1] = state[4];
   out_7913228155057385150[2] = state[5];
}
void H_13(double *state, double *unused, double *out_6711818052314979691) {
   out_6711818052314979691[0] = 0;
   out_6711818052314979691[1] = 0;
   out_6711818052314979691[2] = 0;
   out_6711818052314979691[3] = 1;
   out_6711818052314979691[4] = 0;
   out_6711818052314979691[5] = 0;
   out_6711818052314979691[6] = 0;
   out_6711818052314979691[7] = 0;
   out_6711818052314979691[8] = 0;
   out_6711818052314979691[9] = 0;
   out_6711818052314979691[10] = 0;
   out_6711818052314979691[11] = 0;
   out_6711818052314979691[12] = 0;
   out_6711818052314979691[13] = 0;
   out_6711818052314979691[14] = 0;
   out_6711818052314979691[15] = 0;
   out_6711818052314979691[16] = 0;
   out_6711818052314979691[17] = 0;
   out_6711818052314979691[18] = 0;
   out_6711818052314979691[19] = 0;
   out_6711818052314979691[20] = 0;
   out_6711818052314979691[21] = 0;
   out_6711818052314979691[22] = 1;
   out_6711818052314979691[23] = 0;
   out_6711818052314979691[24] = 0;
   out_6711818052314979691[25] = 0;
   out_6711818052314979691[26] = 0;
   out_6711818052314979691[27] = 0;
   out_6711818052314979691[28] = 0;
   out_6711818052314979691[29] = 0;
   out_6711818052314979691[30] = 0;
   out_6711818052314979691[31] = 0;
   out_6711818052314979691[32] = 0;
   out_6711818052314979691[33] = 0;
   out_6711818052314979691[34] = 0;
   out_6711818052314979691[35] = 0;
   out_6711818052314979691[36] = 0;
   out_6711818052314979691[37] = 0;
   out_6711818052314979691[38] = 0;
   out_6711818052314979691[39] = 0;
   out_6711818052314979691[40] = 0;
   out_6711818052314979691[41] = 1;
   out_6711818052314979691[42] = 0;
   out_6711818052314979691[43] = 0;
   out_6711818052314979691[44] = 0;
   out_6711818052314979691[45] = 0;
   out_6711818052314979691[46] = 0;
   out_6711818052314979691[47] = 0;
   out_6711818052314979691[48] = 0;
   out_6711818052314979691[49] = 0;
   out_6711818052314979691[50] = 0;
   out_6711818052314979691[51] = 0;
   out_6711818052314979691[52] = 0;
   out_6711818052314979691[53] = 0;
}
void h_14(double *state, double *unused, double *out_6835226830806042526) {
   out_6835226830806042526[0] = state[6];
   out_6835226830806042526[1] = state[7];
   out_6835226830806042526[2] = state[8];
}
void H_14(double *state, double *unused, double *out_3064427700337763291) {
   out_3064427700337763291[0] = 0;
   out_3064427700337763291[1] = 0;
   out_3064427700337763291[2] = 0;
   out_3064427700337763291[3] = 0;
   out_3064427700337763291[4] = 0;
   out_3064427700337763291[5] = 0;
   out_3064427700337763291[6] = 1;
   out_3064427700337763291[7] = 0;
   out_3064427700337763291[8] = 0;
   out_3064427700337763291[9] = 0;
   out_3064427700337763291[10] = 0;
   out_3064427700337763291[11] = 0;
   out_3064427700337763291[12] = 0;
   out_3064427700337763291[13] = 0;
   out_3064427700337763291[14] = 0;
   out_3064427700337763291[15] = 0;
   out_3064427700337763291[16] = 0;
   out_3064427700337763291[17] = 0;
   out_3064427700337763291[18] = 0;
   out_3064427700337763291[19] = 0;
   out_3064427700337763291[20] = 0;
   out_3064427700337763291[21] = 0;
   out_3064427700337763291[22] = 0;
   out_3064427700337763291[23] = 0;
   out_3064427700337763291[24] = 0;
   out_3064427700337763291[25] = 1;
   out_3064427700337763291[26] = 0;
   out_3064427700337763291[27] = 0;
   out_3064427700337763291[28] = 0;
   out_3064427700337763291[29] = 0;
   out_3064427700337763291[30] = 0;
   out_3064427700337763291[31] = 0;
   out_3064427700337763291[32] = 0;
   out_3064427700337763291[33] = 0;
   out_3064427700337763291[34] = 0;
   out_3064427700337763291[35] = 0;
   out_3064427700337763291[36] = 0;
   out_3064427700337763291[37] = 0;
   out_3064427700337763291[38] = 0;
   out_3064427700337763291[39] = 0;
   out_3064427700337763291[40] = 0;
   out_3064427700337763291[41] = 0;
   out_3064427700337763291[42] = 0;
   out_3064427700337763291[43] = 0;
   out_3064427700337763291[44] = 1;
   out_3064427700337763291[45] = 0;
   out_3064427700337763291[46] = 0;
   out_3064427700337763291[47] = 0;
   out_3064427700337763291[48] = 0;
   out_3064427700337763291[49] = 0;
   out_3064427700337763291[50] = 0;
   out_3064427700337763291[51] = 0;
   out_3064427700337763291[52] = 0;
   out_3064427700337763291[53] = 0;
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
void pose_err_fun(double *nom_x, double *delta_x, double *out_4086162596914389259) {
  err_fun(nom_x, delta_x, out_4086162596914389259);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_628093630908614818) {
  inv_err_fun(nom_x, true_x, out_628093630908614818);
}
void pose_H_mod_fun(double *state, double *out_920885569904575623) {
  H_mod_fun(state, out_920885569904575623);
}
void pose_f_fun(double *state, double dt, double *out_4638827357690861687) {
  f_fun(state,  dt, out_4638827357690861687);
}
void pose_F_fun(double *state, double dt, double *out_2122913628783415921) {
  F_fun(state,  dt, out_2122913628783415921);
}
void pose_h_4(double *state, double *unused, double *out_8610552428979210253) {
  h_4(state, unused, out_8610552428979210253);
}
void pose_H_4(double *state, double *unused, double *out_898813156001721238) {
  H_4(state, unused, out_898813156001721238);
}
void pose_h_10(double *state, double *unused, double *out_6115253449497749874) {
  h_10(state, unused, out_6115253449497749874);
}
void pose_H_10(double *state, double *unused, double *out_795420154396972986) {
  H_10(state, unused, out_795420154396972986);
}
void pose_h_13(double *state, double *unused, double *out_7913228155057385150) {
  h_13(state, unused, out_7913228155057385150);
}
void pose_H_13(double *state, double *unused, double *out_6711818052314979691) {
  H_13(state, unused, out_6711818052314979691);
}
void pose_h_14(double *state, double *unused, double *out_6835226830806042526) {
  h_14(state, unused, out_6835226830806042526);
}
void pose_H_14(double *state, double *unused, double *out_3064427700337763291) {
  H_14(state, unused, out_3064427700337763291);
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
