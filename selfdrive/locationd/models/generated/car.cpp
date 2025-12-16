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
void err_fun(double *nom_x, double *delta_x, double *out_602274555864179715) {
   out_602274555864179715[0] = delta_x[0] + nom_x[0];
   out_602274555864179715[1] = delta_x[1] + nom_x[1];
   out_602274555864179715[2] = delta_x[2] + nom_x[2];
   out_602274555864179715[3] = delta_x[3] + nom_x[3];
   out_602274555864179715[4] = delta_x[4] + nom_x[4];
   out_602274555864179715[5] = delta_x[5] + nom_x[5];
   out_602274555864179715[6] = delta_x[6] + nom_x[6];
   out_602274555864179715[7] = delta_x[7] + nom_x[7];
   out_602274555864179715[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_1813808766499165488) {
   out_1813808766499165488[0] = -nom_x[0] + true_x[0];
   out_1813808766499165488[1] = -nom_x[1] + true_x[1];
   out_1813808766499165488[2] = -nom_x[2] + true_x[2];
   out_1813808766499165488[3] = -nom_x[3] + true_x[3];
   out_1813808766499165488[4] = -nom_x[4] + true_x[4];
   out_1813808766499165488[5] = -nom_x[5] + true_x[5];
   out_1813808766499165488[6] = -nom_x[6] + true_x[6];
   out_1813808766499165488[7] = -nom_x[7] + true_x[7];
   out_1813808766499165488[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_6993529450035416664) {
   out_6993529450035416664[0] = 1.0;
   out_6993529450035416664[1] = 0.0;
   out_6993529450035416664[2] = 0.0;
   out_6993529450035416664[3] = 0.0;
   out_6993529450035416664[4] = 0.0;
   out_6993529450035416664[5] = 0.0;
   out_6993529450035416664[6] = 0.0;
   out_6993529450035416664[7] = 0.0;
   out_6993529450035416664[8] = 0.0;
   out_6993529450035416664[9] = 0.0;
   out_6993529450035416664[10] = 1.0;
   out_6993529450035416664[11] = 0.0;
   out_6993529450035416664[12] = 0.0;
   out_6993529450035416664[13] = 0.0;
   out_6993529450035416664[14] = 0.0;
   out_6993529450035416664[15] = 0.0;
   out_6993529450035416664[16] = 0.0;
   out_6993529450035416664[17] = 0.0;
   out_6993529450035416664[18] = 0.0;
   out_6993529450035416664[19] = 0.0;
   out_6993529450035416664[20] = 1.0;
   out_6993529450035416664[21] = 0.0;
   out_6993529450035416664[22] = 0.0;
   out_6993529450035416664[23] = 0.0;
   out_6993529450035416664[24] = 0.0;
   out_6993529450035416664[25] = 0.0;
   out_6993529450035416664[26] = 0.0;
   out_6993529450035416664[27] = 0.0;
   out_6993529450035416664[28] = 0.0;
   out_6993529450035416664[29] = 0.0;
   out_6993529450035416664[30] = 1.0;
   out_6993529450035416664[31] = 0.0;
   out_6993529450035416664[32] = 0.0;
   out_6993529450035416664[33] = 0.0;
   out_6993529450035416664[34] = 0.0;
   out_6993529450035416664[35] = 0.0;
   out_6993529450035416664[36] = 0.0;
   out_6993529450035416664[37] = 0.0;
   out_6993529450035416664[38] = 0.0;
   out_6993529450035416664[39] = 0.0;
   out_6993529450035416664[40] = 1.0;
   out_6993529450035416664[41] = 0.0;
   out_6993529450035416664[42] = 0.0;
   out_6993529450035416664[43] = 0.0;
   out_6993529450035416664[44] = 0.0;
   out_6993529450035416664[45] = 0.0;
   out_6993529450035416664[46] = 0.0;
   out_6993529450035416664[47] = 0.0;
   out_6993529450035416664[48] = 0.0;
   out_6993529450035416664[49] = 0.0;
   out_6993529450035416664[50] = 1.0;
   out_6993529450035416664[51] = 0.0;
   out_6993529450035416664[52] = 0.0;
   out_6993529450035416664[53] = 0.0;
   out_6993529450035416664[54] = 0.0;
   out_6993529450035416664[55] = 0.0;
   out_6993529450035416664[56] = 0.0;
   out_6993529450035416664[57] = 0.0;
   out_6993529450035416664[58] = 0.0;
   out_6993529450035416664[59] = 0.0;
   out_6993529450035416664[60] = 1.0;
   out_6993529450035416664[61] = 0.0;
   out_6993529450035416664[62] = 0.0;
   out_6993529450035416664[63] = 0.0;
   out_6993529450035416664[64] = 0.0;
   out_6993529450035416664[65] = 0.0;
   out_6993529450035416664[66] = 0.0;
   out_6993529450035416664[67] = 0.0;
   out_6993529450035416664[68] = 0.0;
   out_6993529450035416664[69] = 0.0;
   out_6993529450035416664[70] = 1.0;
   out_6993529450035416664[71] = 0.0;
   out_6993529450035416664[72] = 0.0;
   out_6993529450035416664[73] = 0.0;
   out_6993529450035416664[74] = 0.0;
   out_6993529450035416664[75] = 0.0;
   out_6993529450035416664[76] = 0.0;
   out_6993529450035416664[77] = 0.0;
   out_6993529450035416664[78] = 0.0;
   out_6993529450035416664[79] = 0.0;
   out_6993529450035416664[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_8128556128886940709) {
   out_8128556128886940709[0] = state[0];
   out_8128556128886940709[1] = state[1];
   out_8128556128886940709[2] = state[2];
   out_8128556128886940709[3] = state[3];
   out_8128556128886940709[4] = state[4];
   out_8128556128886940709[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8100000000000005*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_8128556128886940709[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_8128556128886940709[7] = state[7];
   out_8128556128886940709[8] = state[8];
}
void F_fun(double *state, double dt, double *out_197190331083741563) {
   out_197190331083741563[0] = 1;
   out_197190331083741563[1] = 0;
   out_197190331083741563[2] = 0;
   out_197190331083741563[3] = 0;
   out_197190331083741563[4] = 0;
   out_197190331083741563[5] = 0;
   out_197190331083741563[6] = 0;
   out_197190331083741563[7] = 0;
   out_197190331083741563[8] = 0;
   out_197190331083741563[9] = 0;
   out_197190331083741563[10] = 1;
   out_197190331083741563[11] = 0;
   out_197190331083741563[12] = 0;
   out_197190331083741563[13] = 0;
   out_197190331083741563[14] = 0;
   out_197190331083741563[15] = 0;
   out_197190331083741563[16] = 0;
   out_197190331083741563[17] = 0;
   out_197190331083741563[18] = 0;
   out_197190331083741563[19] = 0;
   out_197190331083741563[20] = 1;
   out_197190331083741563[21] = 0;
   out_197190331083741563[22] = 0;
   out_197190331083741563[23] = 0;
   out_197190331083741563[24] = 0;
   out_197190331083741563[25] = 0;
   out_197190331083741563[26] = 0;
   out_197190331083741563[27] = 0;
   out_197190331083741563[28] = 0;
   out_197190331083741563[29] = 0;
   out_197190331083741563[30] = 1;
   out_197190331083741563[31] = 0;
   out_197190331083741563[32] = 0;
   out_197190331083741563[33] = 0;
   out_197190331083741563[34] = 0;
   out_197190331083741563[35] = 0;
   out_197190331083741563[36] = 0;
   out_197190331083741563[37] = 0;
   out_197190331083741563[38] = 0;
   out_197190331083741563[39] = 0;
   out_197190331083741563[40] = 1;
   out_197190331083741563[41] = 0;
   out_197190331083741563[42] = 0;
   out_197190331083741563[43] = 0;
   out_197190331083741563[44] = 0;
   out_197190331083741563[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_197190331083741563[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_197190331083741563[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_197190331083741563[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_197190331083741563[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_197190331083741563[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_197190331083741563[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_197190331083741563[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_197190331083741563[53] = -9.8100000000000005*dt;
   out_197190331083741563[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_197190331083741563[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_197190331083741563[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_197190331083741563[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_197190331083741563[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_197190331083741563[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_197190331083741563[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_197190331083741563[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_197190331083741563[62] = 0;
   out_197190331083741563[63] = 0;
   out_197190331083741563[64] = 0;
   out_197190331083741563[65] = 0;
   out_197190331083741563[66] = 0;
   out_197190331083741563[67] = 0;
   out_197190331083741563[68] = 0;
   out_197190331083741563[69] = 0;
   out_197190331083741563[70] = 1;
   out_197190331083741563[71] = 0;
   out_197190331083741563[72] = 0;
   out_197190331083741563[73] = 0;
   out_197190331083741563[74] = 0;
   out_197190331083741563[75] = 0;
   out_197190331083741563[76] = 0;
   out_197190331083741563[77] = 0;
   out_197190331083741563[78] = 0;
   out_197190331083741563[79] = 0;
   out_197190331083741563[80] = 1;
}
void h_25(double *state, double *unused, double *out_3672452708460500252) {
   out_3672452708460500252[0] = state[6];
}
void H_25(double *state, double *unused, double *out_6658383780746265843) {
   out_6658383780746265843[0] = 0;
   out_6658383780746265843[1] = 0;
   out_6658383780746265843[2] = 0;
   out_6658383780746265843[3] = 0;
   out_6658383780746265843[4] = 0;
   out_6658383780746265843[5] = 0;
   out_6658383780746265843[6] = 1;
   out_6658383780746265843[7] = 0;
   out_6658383780746265843[8] = 0;
}
void h_24(double *state, double *unused, double *out_7683485618314791650) {
   out_7683485618314791650[0] = state[4];
   out_7683485618314791650[1] = state[5];
}
void H_24(double *state, double *unused, double *out_2560295106894090548) {
   out_2560295106894090548[0] = 0;
   out_2560295106894090548[1] = 0;
   out_2560295106894090548[2] = 0;
   out_2560295106894090548[3] = 0;
   out_2560295106894090548[4] = 1;
   out_2560295106894090548[5] = 0;
   out_2560295106894090548[6] = 0;
   out_2560295106894090548[7] = 0;
   out_2560295106894090548[8] = 0;
   out_2560295106894090548[9] = 0;
   out_2560295106894090548[10] = 0;
   out_2560295106894090548[11] = 0;
   out_2560295106894090548[12] = 0;
   out_2560295106894090548[13] = 0;
   out_2560295106894090548[14] = 1;
   out_2560295106894090548[15] = 0;
   out_2560295106894090548[16] = 0;
   out_2560295106894090548[17] = 0;
}
void h_30(double *state, double *unused, double *out_4208819942293548933) {
   out_4208819942293548933[0] = state[4];
}
void H_30(double *state, double *unused, double *out_2130687450618657645) {
   out_2130687450618657645[0] = 0;
   out_2130687450618657645[1] = 0;
   out_2130687450618657645[2] = 0;
   out_2130687450618657645[3] = 0;
   out_2130687450618657645[4] = 1;
   out_2130687450618657645[5] = 0;
   out_2130687450618657645[6] = 0;
   out_2130687450618657645[7] = 0;
   out_2130687450618657645[8] = 0;
}
void h_26(double *state, double *unused, double *out_792853325367296712) {
   out_792853325367296712[0] = state[7];
}
void H_26(double *state, double *unused, double *out_2916880461872209619) {
   out_2916880461872209619[0] = 0;
   out_2916880461872209619[1] = 0;
   out_2916880461872209619[2] = 0;
   out_2916880461872209619[3] = 0;
   out_2916880461872209619[4] = 0;
   out_2916880461872209619[5] = 0;
   out_2916880461872209619[6] = 0;
   out_2916880461872209619[7] = 1;
   out_2916880461872209619[8] = 0;
}
void h_27(double *state, double *unused, double *out_1329220559200345393) {
   out_1329220559200345393[0] = state[3];
}
void H_27(double *state, double *unused, double *out_44075861181767266) {
   out_44075861181767266[0] = 0;
   out_44075861181767266[1] = 0;
   out_44075861181767266[2] = 0;
   out_44075861181767266[3] = 1;
   out_44075861181767266[4] = 0;
   out_44075861181767266[5] = 0;
   out_44075861181767266[6] = 0;
   out_44075861181767266[7] = 0;
   out_44075861181767266[8] = 0;
}
void h_29(double *state, double *unused, double *out_6434044697620327048) {
   out_6434044697620327048[0] = state[1];
}
void H_29(double *state, double *unused, double *out_2640918794933049829) {
   out_2640918794933049829[0] = 0;
   out_2640918794933049829[1] = 1;
   out_2640918794933049829[2] = 0;
   out_2640918794933049829[3] = 0;
   out_2640918794933049829[4] = 0;
   out_2640918794933049829[5] = 0;
   out_2640918794933049829[6] = 0;
   out_2640918794933049829[7] = 0;
   out_2640918794933049829[8] = 0;
}
void h_28(double *state, double *unused, double *out_7431739994933539783) {
   out_7431739994933539783[0] = state[0];
}
void H_28(double *state, double *unused, double *out_4604549066498376080) {
   out_4604549066498376080[0] = 1;
   out_4604549066498376080[1] = 0;
   out_4604549066498376080[2] = 0;
   out_4604549066498376080[3] = 0;
   out_4604549066498376080[4] = 0;
   out_4604549066498376080[5] = 0;
   out_4604549066498376080[6] = 0;
   out_4604549066498376080[7] = 0;
   out_4604549066498376080[8] = 0;
}
void h_31(double *state, double *unused, double *out_4824207972803239740) {
   out_4824207972803239740[0] = state[8];
}
void H_31(double *state, double *unused, double *out_2290672359638858143) {
   out_2290672359638858143[0] = 0;
   out_2290672359638858143[1] = 0;
   out_2290672359638858143[2] = 0;
   out_2290672359638858143[3] = 0;
   out_2290672359638858143[4] = 0;
   out_2290672359638858143[5] = 0;
   out_2290672359638858143[6] = 0;
   out_2290672359638858143[7] = 0;
   out_2290672359638858143[8] = 1;
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
void car_err_fun(double *nom_x, double *delta_x, double *out_602274555864179715) {
  err_fun(nom_x, delta_x, out_602274555864179715);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_1813808766499165488) {
  inv_err_fun(nom_x, true_x, out_1813808766499165488);
}
void car_H_mod_fun(double *state, double *out_6993529450035416664) {
  H_mod_fun(state, out_6993529450035416664);
}
void car_f_fun(double *state, double dt, double *out_8128556128886940709) {
  f_fun(state,  dt, out_8128556128886940709);
}
void car_F_fun(double *state, double dt, double *out_197190331083741563) {
  F_fun(state,  dt, out_197190331083741563);
}
void car_h_25(double *state, double *unused, double *out_3672452708460500252) {
  h_25(state, unused, out_3672452708460500252);
}
void car_H_25(double *state, double *unused, double *out_6658383780746265843) {
  H_25(state, unused, out_6658383780746265843);
}
void car_h_24(double *state, double *unused, double *out_7683485618314791650) {
  h_24(state, unused, out_7683485618314791650);
}
void car_H_24(double *state, double *unused, double *out_2560295106894090548) {
  H_24(state, unused, out_2560295106894090548);
}
void car_h_30(double *state, double *unused, double *out_4208819942293548933) {
  h_30(state, unused, out_4208819942293548933);
}
void car_H_30(double *state, double *unused, double *out_2130687450618657645) {
  H_30(state, unused, out_2130687450618657645);
}
void car_h_26(double *state, double *unused, double *out_792853325367296712) {
  h_26(state, unused, out_792853325367296712);
}
void car_H_26(double *state, double *unused, double *out_2916880461872209619) {
  H_26(state, unused, out_2916880461872209619);
}
void car_h_27(double *state, double *unused, double *out_1329220559200345393) {
  h_27(state, unused, out_1329220559200345393);
}
void car_H_27(double *state, double *unused, double *out_44075861181767266) {
  H_27(state, unused, out_44075861181767266);
}
void car_h_29(double *state, double *unused, double *out_6434044697620327048) {
  h_29(state, unused, out_6434044697620327048);
}
void car_H_29(double *state, double *unused, double *out_2640918794933049829) {
  H_29(state, unused, out_2640918794933049829);
}
void car_h_28(double *state, double *unused, double *out_7431739994933539783) {
  h_28(state, unused, out_7431739994933539783);
}
void car_H_28(double *state, double *unused, double *out_4604549066498376080) {
  H_28(state, unused, out_4604549066498376080);
}
void car_h_31(double *state, double *unused, double *out_4824207972803239740) {
  h_31(state, unused, out_4824207972803239740);
}
void car_H_31(double *state, double *unused, double *out_2290672359638858143) {
  H_31(state, unused, out_2290672359638858143);
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
