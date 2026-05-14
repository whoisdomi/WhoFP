#include "car.h"

namespace {
#define DIM 9
#define EDIM 9
#define MEDIM 9
typedef void (*Hfun)(double *, double *, double *);

double mass;

void set_mass(double x){ mass = x;}

double rotational_inertia;

void set_rotational_inertia(double x){ rotational_inertia = x;}

double center_to_front;

void set_center_to_front(double x){ center_to_front = x;}

double center_to_rear;

void set_center_to_rear(double x){ center_to_rear = x;}

double stiffness_front;

void set_stiffness_front(double x){ stiffness_front = x;}

double stiffness_rear;

void set_stiffness_rear(double x){ stiffness_rear = x;}
const static double MAHA_THRESH_25 = 3.8414588206941227;
const static double MAHA_THRESH_24 = 5.991464547107981;
const static double MAHA_THRESH_30 = 3.8414588206941227;
const static double MAHA_THRESH_26 = 3.8414588206941227;
const static double MAHA_THRESH_27 = 3.8414588206941227;
const static double MAHA_THRESH_29 = 3.8414588206941227;
const static double MAHA_THRESH_28 = 3.8414588206941227;
const static double MAHA_THRESH_31 = 3.8414588206941227;

/******************************************************************************
 *                      Code generated with SymPy 1.14.0                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_4284318502583774505) {
   out_4284318502583774505[0] = delta_x[0] + nom_x[0];
   out_4284318502583774505[1] = delta_x[1] + nom_x[1];
   out_4284318502583774505[2] = delta_x[2] + nom_x[2];
   out_4284318502583774505[3] = delta_x[3] + nom_x[3];
   out_4284318502583774505[4] = delta_x[4] + nom_x[4];
   out_4284318502583774505[5] = delta_x[5] + nom_x[5];
   out_4284318502583774505[6] = delta_x[6] + nom_x[6];
   out_4284318502583774505[7] = delta_x[7] + nom_x[7];
   out_4284318502583774505[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_9062210534970943629) {
   out_9062210534970943629[0] = -nom_x[0] + true_x[0];
   out_9062210534970943629[1] = -nom_x[1] + true_x[1];
   out_9062210534970943629[2] = -nom_x[2] + true_x[2];
   out_9062210534970943629[3] = -nom_x[3] + true_x[3];
   out_9062210534970943629[4] = -nom_x[4] + true_x[4];
   out_9062210534970943629[5] = -nom_x[5] + true_x[5];
   out_9062210534970943629[6] = -nom_x[6] + true_x[6];
   out_9062210534970943629[7] = -nom_x[7] + true_x[7];
   out_9062210534970943629[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_5030433578682660012) {
   out_5030433578682660012[0] = 1.0;
   out_5030433578682660012[1] = 0.0;
   out_5030433578682660012[2] = 0.0;
   out_5030433578682660012[3] = 0.0;
   out_5030433578682660012[4] = 0.0;
   out_5030433578682660012[5] = 0.0;
   out_5030433578682660012[6] = 0.0;
   out_5030433578682660012[7] = 0.0;
   out_5030433578682660012[8] = 0.0;
   out_5030433578682660012[9] = 0.0;
   out_5030433578682660012[10] = 1.0;
   out_5030433578682660012[11] = 0.0;
   out_5030433578682660012[12] = 0.0;
   out_5030433578682660012[13] = 0.0;
   out_5030433578682660012[14] = 0.0;
   out_5030433578682660012[15] = 0.0;
   out_5030433578682660012[16] = 0.0;
   out_5030433578682660012[17] = 0.0;
   out_5030433578682660012[18] = 0.0;
   out_5030433578682660012[19] = 0.0;
   out_5030433578682660012[20] = 1.0;
   out_5030433578682660012[21] = 0.0;
   out_5030433578682660012[22] = 0.0;
   out_5030433578682660012[23] = 0.0;
   out_5030433578682660012[24] = 0.0;
   out_5030433578682660012[25] = 0.0;
   out_5030433578682660012[26] = 0.0;
   out_5030433578682660012[27] = 0.0;
   out_5030433578682660012[28] = 0.0;
   out_5030433578682660012[29] = 0.0;
   out_5030433578682660012[30] = 1.0;
   out_5030433578682660012[31] = 0.0;
   out_5030433578682660012[32] = 0.0;
   out_5030433578682660012[33] = 0.0;
   out_5030433578682660012[34] = 0.0;
   out_5030433578682660012[35] = 0.0;
   out_5030433578682660012[36] = 0.0;
   out_5030433578682660012[37] = 0.0;
   out_5030433578682660012[38] = 0.0;
   out_5030433578682660012[39] = 0.0;
   out_5030433578682660012[40] = 1.0;
   out_5030433578682660012[41] = 0.0;
   out_5030433578682660012[42] = 0.0;
   out_5030433578682660012[43] = 0.0;
   out_5030433578682660012[44] = 0.0;
   out_5030433578682660012[45] = 0.0;
   out_5030433578682660012[46] = 0.0;
   out_5030433578682660012[47] = 0.0;
   out_5030433578682660012[48] = 0.0;
   out_5030433578682660012[49] = 0.0;
   out_5030433578682660012[50] = 1.0;
   out_5030433578682660012[51] = 0.0;
   out_5030433578682660012[52] = 0.0;
   out_5030433578682660012[53] = 0.0;
   out_5030433578682660012[54] = 0.0;
   out_5030433578682660012[55] = 0.0;
   out_5030433578682660012[56] = 0.0;
   out_5030433578682660012[57] = 0.0;
   out_5030433578682660012[58] = 0.0;
   out_5030433578682660012[59] = 0.0;
   out_5030433578682660012[60] = 1.0;
   out_5030433578682660012[61] = 0.0;
   out_5030433578682660012[62] = 0.0;
   out_5030433578682660012[63] = 0.0;
   out_5030433578682660012[64] = 0.0;
   out_5030433578682660012[65] = 0.0;
   out_5030433578682660012[66] = 0.0;
   out_5030433578682660012[67] = 0.0;
   out_5030433578682660012[68] = 0.0;
   out_5030433578682660012[69] = 0.0;
   out_5030433578682660012[70] = 1.0;
   out_5030433578682660012[71] = 0.0;
   out_5030433578682660012[72] = 0.0;
   out_5030433578682660012[73] = 0.0;
   out_5030433578682660012[74] = 0.0;
   out_5030433578682660012[75] = 0.0;
   out_5030433578682660012[76] = 0.0;
   out_5030433578682660012[77] = 0.0;
   out_5030433578682660012[78] = 0.0;
   out_5030433578682660012[79] = 0.0;
   out_5030433578682660012[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_284361389350267453) {
   out_284361389350267453[0] = state[0];
   out_284361389350267453[1] = state[1];
   out_284361389350267453[2] = state[2];
   out_284361389350267453[3] = state[3];
   out_284361389350267453[4] = state[4];
   out_284361389350267453[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8100000000000005*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_284361389350267453[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_284361389350267453[7] = state[7];
   out_284361389350267453[8] = state[8];
}
void F_fun(double *state, double dt, double *out_3718156729130345156) {
   out_3718156729130345156[0] = 1;
   out_3718156729130345156[1] = 0;
   out_3718156729130345156[2] = 0;
   out_3718156729130345156[3] = 0;
   out_3718156729130345156[4] = 0;
   out_3718156729130345156[5] = 0;
   out_3718156729130345156[6] = 0;
   out_3718156729130345156[7] = 0;
   out_3718156729130345156[8] = 0;
   out_3718156729130345156[9] = 0;
   out_3718156729130345156[10] = 1;
   out_3718156729130345156[11] = 0;
   out_3718156729130345156[12] = 0;
   out_3718156729130345156[13] = 0;
   out_3718156729130345156[14] = 0;
   out_3718156729130345156[15] = 0;
   out_3718156729130345156[16] = 0;
   out_3718156729130345156[17] = 0;
   out_3718156729130345156[18] = 0;
   out_3718156729130345156[19] = 0;
   out_3718156729130345156[20] = 1;
   out_3718156729130345156[21] = 0;
   out_3718156729130345156[22] = 0;
   out_3718156729130345156[23] = 0;
   out_3718156729130345156[24] = 0;
   out_3718156729130345156[25] = 0;
   out_3718156729130345156[26] = 0;
   out_3718156729130345156[27] = 0;
   out_3718156729130345156[28] = 0;
   out_3718156729130345156[29] = 0;
   out_3718156729130345156[30] = 1;
   out_3718156729130345156[31] = 0;
   out_3718156729130345156[32] = 0;
   out_3718156729130345156[33] = 0;
   out_3718156729130345156[34] = 0;
   out_3718156729130345156[35] = 0;
   out_3718156729130345156[36] = 0;
   out_3718156729130345156[37] = 0;
   out_3718156729130345156[38] = 0;
   out_3718156729130345156[39] = 0;
   out_3718156729130345156[40] = 1;
   out_3718156729130345156[41] = 0;
   out_3718156729130345156[42] = 0;
   out_3718156729130345156[43] = 0;
   out_3718156729130345156[44] = 0;
   out_3718156729130345156[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_3718156729130345156[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_3718156729130345156[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_3718156729130345156[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_3718156729130345156[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_3718156729130345156[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_3718156729130345156[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_3718156729130345156[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_3718156729130345156[53] = -9.8100000000000005*dt;
   out_3718156729130345156[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_3718156729130345156[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_3718156729130345156[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3718156729130345156[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3718156729130345156[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_3718156729130345156[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_3718156729130345156[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_3718156729130345156[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3718156729130345156[62] = 0;
   out_3718156729130345156[63] = 0;
   out_3718156729130345156[64] = 0;
   out_3718156729130345156[65] = 0;
   out_3718156729130345156[66] = 0;
   out_3718156729130345156[67] = 0;
   out_3718156729130345156[68] = 0;
   out_3718156729130345156[69] = 0;
   out_3718156729130345156[70] = 1;
   out_3718156729130345156[71] = 0;
   out_3718156729130345156[72] = 0;
   out_3718156729130345156[73] = 0;
   out_3718156729130345156[74] = 0;
   out_3718156729130345156[75] = 0;
   out_3718156729130345156[76] = 0;
   out_3718156729130345156[77] = 0;
   out_3718156729130345156[78] = 0;
   out_3718156729130345156[79] = 0;
   out_3718156729130345156[80] = 1;
}
void h_25(double *state, double *unused, double *out_8289407373313103834) {
   out_8289407373313103834[0] = state[6];
}
void H_25(double *state, double *unused, double *out_4223122269114654367) {
   out_4223122269114654367[0] = 0;
   out_4223122269114654367[1] = 0;
   out_4223122269114654367[2] = 0;
   out_4223122269114654367[3] = 0;
   out_4223122269114654367[4] = 0;
   out_4223122269114654367[5] = 0;
   out_4223122269114654367[6] = 1;
   out_4223122269114654367[7] = 0;
   out_4223122269114654367[8] = 0;
}
void h_24(double *state, double *unused, double *out_2858732842488423539) {
   out_2858732842488423539[0] = state[4];
   out_2858732842488423539[1] = state[5];
}
void H_24(double *state, double *unused, double *out_6448830053093522929) {
   out_6448830053093522929[0] = 0;
   out_6448830053093522929[1] = 0;
   out_6448830053093522929[2] = 0;
   out_6448830053093522929[3] = 0;
   out_6448830053093522929[4] = 1;
   out_6448830053093522929[5] = 0;
   out_6448830053093522929[6] = 0;
   out_6448830053093522929[7] = 0;
   out_6448830053093522929[8] = 0;
   out_6448830053093522929[9] = 0;
   out_6448830053093522929[10] = 0;
   out_6448830053093522929[11] = 0;
   out_6448830053093522929[12] = 0;
   out_6448830053093522929[13] = 0;
   out_6448830053093522929[14] = 1;
   out_6448830053093522929[15] = 0;
   out_6448830053093522929[16] = 0;
   out_6448830053093522929[17] = 0;
}
void h_30(double *state, double *unused, double *out_350470103405438050) {
   out_350470103405438050[0] = state[4];
}
void H_30(double *state, double *unused, double *out_7306931463103280494) {
   out_7306931463103280494[0] = 0;
   out_7306931463103280494[1] = 0;
   out_7306931463103280494[2] = 0;
   out_7306931463103280494[3] = 0;
   out_7306931463103280494[4] = 1;
   out_7306931463103280494[5] = 0;
   out_7306931463103280494[6] = 0;
   out_7306931463103280494[7] = 0;
   out_7306931463103280494[8] = 0;
}
void h_26(double *state, double *unused, double *out_2686315948038894635) {
   out_2686315948038894635[0] = state[7];
}
void H_26(double *state, double *unused, double *out_481618950240598143) {
   out_481618950240598143[0] = 0;
   out_481618950240598143[1] = 0;
   out_481618950240598143[2] = 0;
   out_481618950240598143[3] = 0;
   out_481618950240598143[4] = 0;
   out_481618950240598143[5] = 0;
   out_481618950240598143[6] = 0;
   out_481618950240598143[7] = 1;
   out_481618950240598143[8] = 0;
}
void h_27(double *state, double *unused, double *out_9175252607061149670) {
   out_9175252607061149670[0] = state[3];
}
void H_27(double *state, double *unused, double *out_8965049298805846211) {
   out_8965049298805846211[0] = 0;
   out_8965049298805846211[1] = 0;
   out_8965049298805846211[2] = 0;
   out_8965049298805846211[3] = 1;
   out_8965049298805846211[4] = 0;
   out_8965049298805846211[5] = 0;
   out_8965049298805846211[6] = 0;
   out_8965049298805846211[7] = 0;
   out_8965049298805846211[8] = 0;
}
void h_29(double *state, double *unused, double *out_1293979956307100485) {
   out_1293979956307100485[0] = state[1];
}
void H_29(double *state, double *unused, double *out_6796700118788888310) {
   out_6796700118788888310[0] = 0;
   out_6796700118788888310[1] = 1;
   out_6796700118788888310[2] = 0;
   out_6796700118788888310[3] = 0;
   out_6796700118788888310[4] = 0;
   out_6796700118788888310[5] = 0;
   out_6796700118788888310[6] = 0;
   out_6796700118788888310[7] = 0;
   out_6796700118788888310[8] = 0;
}
void h_28(double *state, double *unused, double *out_4173579339400304025) {
   out_4173579339400304025[0] = state[0];
}
void H_28(double *state, double *unused, double *out_6567644937851132732) {
   out_6567644937851132732[0] = 1;
   out_6567644937851132732[1] = 0;
   out_6567644937851132732[2] = 0;
   out_6567644937851132732[3] = 0;
   out_6567644937851132732[4] = 0;
   out_6567644937851132732[5] = 0;
   out_6567644937851132732[6] = 0;
   out_6567644937851132732[7] = 0;
   out_6567644937851132732[8] = 0;
}
void h_31(double *state, double *unused, double *out_3386501474046096846) {
   out_3386501474046096846[0] = state[8];
}
void H_31(double *state, double *unused, double *out_4253768230991614795) {
   out_4253768230991614795[0] = 0;
   out_4253768230991614795[1] = 0;
   out_4253768230991614795[2] = 0;
   out_4253768230991614795[3] = 0;
   out_4253768230991614795[4] = 0;
   out_4253768230991614795[5] = 0;
   out_4253768230991614795[6] = 0;
   out_4253768230991614795[7] = 0;
   out_4253768230991614795[8] = 1;
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

void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_25, H_25, NULL, in_z, in_R, in_ea, MAHA_THRESH_25);
}
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<2, 3, 0>(in_x, in_P, h_24, H_24, NULL, in_z, in_R, in_ea, MAHA_THRESH_24);
}
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_30, H_30, NULL, in_z, in_R, in_ea, MAHA_THRESH_30);
}
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_26, H_26, NULL, in_z, in_R, in_ea, MAHA_THRESH_26);
}
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_27, H_27, NULL, in_z, in_R, in_ea, MAHA_THRESH_27);
}
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_29, H_29, NULL, in_z, in_R, in_ea, MAHA_THRESH_29);
}
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_28, H_28, NULL, in_z, in_R, in_ea, MAHA_THRESH_28);
}
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_31, H_31, NULL, in_z, in_R, in_ea, MAHA_THRESH_31);
}
void car_err_fun(double *nom_x, double *delta_x, double *out_4284318502583774505) {
  err_fun(nom_x, delta_x, out_4284318502583774505);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_9062210534970943629) {
  inv_err_fun(nom_x, true_x, out_9062210534970943629);
}
void car_H_mod_fun(double *state, double *out_5030433578682660012) {
  H_mod_fun(state, out_5030433578682660012);
}
void car_f_fun(double *state, double dt, double *out_284361389350267453) {
  f_fun(state,  dt, out_284361389350267453);
}
void car_F_fun(double *state, double dt, double *out_3718156729130345156) {
  F_fun(state,  dt, out_3718156729130345156);
}
void car_h_25(double *state, double *unused, double *out_8289407373313103834) {
  h_25(state, unused, out_8289407373313103834);
}
void car_H_25(double *state, double *unused, double *out_4223122269114654367) {
  H_25(state, unused, out_4223122269114654367);
}
void car_h_24(double *state, double *unused, double *out_2858732842488423539) {
  h_24(state, unused, out_2858732842488423539);
}
void car_H_24(double *state, double *unused, double *out_6448830053093522929) {
  H_24(state, unused, out_6448830053093522929);
}
void car_h_30(double *state, double *unused, double *out_350470103405438050) {
  h_30(state, unused, out_350470103405438050);
}
void car_H_30(double *state, double *unused, double *out_7306931463103280494) {
  H_30(state, unused, out_7306931463103280494);
}
void car_h_26(double *state, double *unused, double *out_2686315948038894635) {
  h_26(state, unused, out_2686315948038894635);
}
void car_H_26(double *state, double *unused, double *out_481618950240598143) {
  H_26(state, unused, out_481618950240598143);
}
void car_h_27(double *state, double *unused, double *out_9175252607061149670) {
  h_27(state, unused, out_9175252607061149670);
}
void car_H_27(double *state, double *unused, double *out_8965049298805846211) {
  H_27(state, unused, out_8965049298805846211);
}
void car_h_29(double *state, double *unused, double *out_1293979956307100485) {
  h_29(state, unused, out_1293979956307100485);
}
void car_H_29(double *state, double *unused, double *out_6796700118788888310) {
  H_29(state, unused, out_6796700118788888310);
}
void car_h_28(double *state, double *unused, double *out_4173579339400304025) {
  h_28(state, unused, out_4173579339400304025);
}
void car_H_28(double *state, double *unused, double *out_6567644937851132732) {
  H_28(state, unused, out_6567644937851132732);
}
void car_h_31(double *state, double *unused, double *out_3386501474046096846) {
  h_31(state, unused, out_3386501474046096846);
}
void car_H_31(double *state, double *unused, double *out_4253768230991614795) {
  H_31(state, unused, out_4253768230991614795);
}
void car_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
void car_set_mass(double x) {
  set_mass(x);
}
void car_set_rotational_inertia(double x) {
  set_rotational_inertia(x);
}
void car_set_center_to_front(double x) {
  set_center_to_front(x);
}
void car_set_center_to_rear(double x) {
  set_center_to_rear(x);
}
void car_set_stiffness_front(double x) {
  set_stiffness_front(x);
}
void car_set_stiffness_rear(double x) {
  set_stiffness_rear(x);
}
}

const EKF car = {
  .name = "car",
  .kinds = { 25, 24, 30, 26, 27, 29, 28, 31 },
  .feature_kinds = {  },
  .f_fun = car_f_fun,
  .F_fun = car_F_fun,
  .err_fun = car_err_fun,
  .inv_err_fun = car_inv_err_fun,
  .H_mod_fun = car_H_mod_fun,
  .predict = car_predict,
  .hs = {
    { 25, car_h_25 },
    { 24, car_h_24 },
    { 30, car_h_30 },
    { 26, car_h_26 },
    { 27, car_h_27 },
    { 29, car_h_29 },
    { 28, car_h_28 },
    { 31, car_h_31 },
  },
  .Hs = {
    { 25, car_H_25 },
    { 24, car_H_24 },
    { 30, car_H_30 },
    { 26, car_H_26 },
    { 27, car_H_27 },
    { 29, car_H_29 },
    { 28, car_H_28 },
    { 31, car_H_31 },
  },
  .updates = {
    { 25, car_update_25 },
    { 24, car_update_24 },
    { 30, car_update_30 },
    { 26, car_update_26 },
    { 27, car_update_27 },
    { 29, car_update_29 },
    { 28, car_update_28 },
    { 31, car_update_31 },
  },
  .Hes = {
  },
  .sets = {
    { "mass", car_set_mass },
    { "rotational_inertia", car_set_rotational_inertia },
    { "center_to_front", car_set_center_to_front },
    { "center_to_rear", car_set_center_to_rear },
    { "stiffness_front", car_set_stiffness_front },
    { "stiffness_rear", car_set_stiffness_rear },
  },
  .extra_routines = {
  },
};

ekf_lib_init(car)
