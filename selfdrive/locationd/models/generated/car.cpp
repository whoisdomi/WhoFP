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
void err_fun(double *nom_x, double *delta_x, double *out_3334009454934201680) {
   out_3334009454934201680[0] = delta_x[0] + nom_x[0];
   out_3334009454934201680[1] = delta_x[1] + nom_x[1];
   out_3334009454934201680[2] = delta_x[2] + nom_x[2];
   out_3334009454934201680[3] = delta_x[3] + nom_x[3];
   out_3334009454934201680[4] = delta_x[4] + nom_x[4];
   out_3334009454934201680[5] = delta_x[5] + nom_x[5];
   out_3334009454934201680[6] = delta_x[6] + nom_x[6];
   out_3334009454934201680[7] = delta_x[7] + nom_x[7];
   out_3334009454934201680[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_6969690774377442740) {
   out_6969690774377442740[0] = -nom_x[0] + true_x[0];
   out_6969690774377442740[1] = -nom_x[1] + true_x[1];
   out_6969690774377442740[2] = -nom_x[2] + true_x[2];
   out_6969690774377442740[3] = -nom_x[3] + true_x[3];
   out_6969690774377442740[4] = -nom_x[4] + true_x[4];
   out_6969690774377442740[5] = -nom_x[5] + true_x[5];
   out_6969690774377442740[6] = -nom_x[6] + true_x[6];
   out_6969690774377442740[7] = -nom_x[7] + true_x[7];
   out_6969690774377442740[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_5434705195323386726) {
   out_5434705195323386726[0] = 1.0;
   out_5434705195323386726[1] = 0.0;
   out_5434705195323386726[2] = 0.0;
   out_5434705195323386726[3] = 0.0;
   out_5434705195323386726[4] = 0.0;
   out_5434705195323386726[5] = 0.0;
   out_5434705195323386726[6] = 0.0;
   out_5434705195323386726[7] = 0.0;
   out_5434705195323386726[8] = 0.0;
   out_5434705195323386726[9] = 0.0;
   out_5434705195323386726[10] = 1.0;
   out_5434705195323386726[11] = 0.0;
   out_5434705195323386726[12] = 0.0;
   out_5434705195323386726[13] = 0.0;
   out_5434705195323386726[14] = 0.0;
   out_5434705195323386726[15] = 0.0;
   out_5434705195323386726[16] = 0.0;
   out_5434705195323386726[17] = 0.0;
   out_5434705195323386726[18] = 0.0;
   out_5434705195323386726[19] = 0.0;
   out_5434705195323386726[20] = 1.0;
   out_5434705195323386726[21] = 0.0;
   out_5434705195323386726[22] = 0.0;
   out_5434705195323386726[23] = 0.0;
   out_5434705195323386726[24] = 0.0;
   out_5434705195323386726[25] = 0.0;
   out_5434705195323386726[26] = 0.0;
   out_5434705195323386726[27] = 0.0;
   out_5434705195323386726[28] = 0.0;
   out_5434705195323386726[29] = 0.0;
   out_5434705195323386726[30] = 1.0;
   out_5434705195323386726[31] = 0.0;
   out_5434705195323386726[32] = 0.0;
   out_5434705195323386726[33] = 0.0;
   out_5434705195323386726[34] = 0.0;
   out_5434705195323386726[35] = 0.0;
   out_5434705195323386726[36] = 0.0;
   out_5434705195323386726[37] = 0.0;
   out_5434705195323386726[38] = 0.0;
   out_5434705195323386726[39] = 0.0;
   out_5434705195323386726[40] = 1.0;
   out_5434705195323386726[41] = 0.0;
   out_5434705195323386726[42] = 0.0;
   out_5434705195323386726[43] = 0.0;
   out_5434705195323386726[44] = 0.0;
   out_5434705195323386726[45] = 0.0;
   out_5434705195323386726[46] = 0.0;
   out_5434705195323386726[47] = 0.0;
   out_5434705195323386726[48] = 0.0;
   out_5434705195323386726[49] = 0.0;
   out_5434705195323386726[50] = 1.0;
   out_5434705195323386726[51] = 0.0;
   out_5434705195323386726[52] = 0.0;
   out_5434705195323386726[53] = 0.0;
   out_5434705195323386726[54] = 0.0;
   out_5434705195323386726[55] = 0.0;
   out_5434705195323386726[56] = 0.0;
   out_5434705195323386726[57] = 0.0;
   out_5434705195323386726[58] = 0.0;
   out_5434705195323386726[59] = 0.0;
   out_5434705195323386726[60] = 1.0;
   out_5434705195323386726[61] = 0.0;
   out_5434705195323386726[62] = 0.0;
   out_5434705195323386726[63] = 0.0;
   out_5434705195323386726[64] = 0.0;
   out_5434705195323386726[65] = 0.0;
   out_5434705195323386726[66] = 0.0;
   out_5434705195323386726[67] = 0.0;
   out_5434705195323386726[68] = 0.0;
   out_5434705195323386726[69] = 0.0;
   out_5434705195323386726[70] = 1.0;
   out_5434705195323386726[71] = 0.0;
   out_5434705195323386726[72] = 0.0;
   out_5434705195323386726[73] = 0.0;
   out_5434705195323386726[74] = 0.0;
   out_5434705195323386726[75] = 0.0;
   out_5434705195323386726[76] = 0.0;
   out_5434705195323386726[77] = 0.0;
   out_5434705195323386726[78] = 0.0;
   out_5434705195323386726[79] = 0.0;
   out_5434705195323386726[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_958362648498333800) {
   out_958362648498333800[0] = state[0];
   out_958362648498333800[1] = state[1];
   out_958362648498333800[2] = state[2];
   out_958362648498333800[3] = state[3];
   out_958362648498333800[4] = state[4];
   out_958362648498333800[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8100000000000005*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_958362648498333800[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_958362648498333800[7] = state[7];
   out_958362648498333800[8] = state[8];
}
void F_fun(double *state, double dt, double *out_8652167330854030009) {
   out_8652167330854030009[0] = 1;
   out_8652167330854030009[1] = 0;
   out_8652167330854030009[2] = 0;
   out_8652167330854030009[3] = 0;
   out_8652167330854030009[4] = 0;
   out_8652167330854030009[5] = 0;
   out_8652167330854030009[6] = 0;
   out_8652167330854030009[7] = 0;
   out_8652167330854030009[8] = 0;
   out_8652167330854030009[9] = 0;
   out_8652167330854030009[10] = 1;
   out_8652167330854030009[11] = 0;
   out_8652167330854030009[12] = 0;
   out_8652167330854030009[13] = 0;
   out_8652167330854030009[14] = 0;
   out_8652167330854030009[15] = 0;
   out_8652167330854030009[16] = 0;
   out_8652167330854030009[17] = 0;
   out_8652167330854030009[18] = 0;
   out_8652167330854030009[19] = 0;
   out_8652167330854030009[20] = 1;
   out_8652167330854030009[21] = 0;
   out_8652167330854030009[22] = 0;
   out_8652167330854030009[23] = 0;
   out_8652167330854030009[24] = 0;
   out_8652167330854030009[25] = 0;
   out_8652167330854030009[26] = 0;
   out_8652167330854030009[27] = 0;
   out_8652167330854030009[28] = 0;
   out_8652167330854030009[29] = 0;
   out_8652167330854030009[30] = 1;
   out_8652167330854030009[31] = 0;
   out_8652167330854030009[32] = 0;
   out_8652167330854030009[33] = 0;
   out_8652167330854030009[34] = 0;
   out_8652167330854030009[35] = 0;
   out_8652167330854030009[36] = 0;
   out_8652167330854030009[37] = 0;
   out_8652167330854030009[38] = 0;
   out_8652167330854030009[39] = 0;
   out_8652167330854030009[40] = 1;
   out_8652167330854030009[41] = 0;
   out_8652167330854030009[42] = 0;
   out_8652167330854030009[43] = 0;
   out_8652167330854030009[44] = 0;
   out_8652167330854030009[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_8652167330854030009[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_8652167330854030009[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_8652167330854030009[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_8652167330854030009[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_8652167330854030009[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_8652167330854030009[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_8652167330854030009[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_8652167330854030009[53] = -9.8100000000000005*dt;
   out_8652167330854030009[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_8652167330854030009[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_8652167330854030009[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_8652167330854030009[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_8652167330854030009[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_8652167330854030009[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_8652167330854030009[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_8652167330854030009[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_8652167330854030009[62] = 0;
   out_8652167330854030009[63] = 0;
   out_8652167330854030009[64] = 0;
   out_8652167330854030009[65] = 0;
   out_8652167330854030009[66] = 0;
   out_8652167330854030009[67] = 0;
   out_8652167330854030009[68] = 0;
   out_8652167330854030009[69] = 0;
   out_8652167330854030009[70] = 1;
   out_8652167330854030009[71] = 0;
   out_8652167330854030009[72] = 0;
   out_8652167330854030009[73] = 0;
   out_8652167330854030009[74] = 0;
   out_8652167330854030009[75] = 0;
   out_8652167330854030009[76] = 0;
   out_8652167330854030009[77] = 0;
   out_8652167330854030009[78] = 0;
   out_8652167330854030009[79] = 0;
   out_8652167330854030009[80] = 1;
}
void h_25(double *state, double *unused, double *out_8967817308417867396) {
   out_8967817308417867396[0] = state[6];
}
void H_25(double *state, double *unused, double *out_6915130511779790274) {
   out_6915130511779790274[0] = 0;
   out_6915130511779790274[1] = 0;
   out_6915130511779790274[2] = 0;
   out_6915130511779790274[3] = 0;
   out_6915130511779790274[4] = 0;
   out_6915130511779790274[5] = 0;
   out_6915130511779790274[6] = 1;
   out_6915130511779790274[7] = 0;
   out_6915130511779790274[8] = 0;
}
void h_24(double *state, double *unused, double *out_5626992763966467179) {
   out_5626992763966467179[0] = state[4];
   out_5626992763966467179[1] = state[5];
}
void H_24(double *state, double *unused, double *out_9092344935386940247) {
   out_9092344935386940247[0] = 0;
   out_9092344935386940247[1] = 0;
   out_9092344935386940247[2] = 0;
   out_9092344935386940247[3] = 0;
   out_9092344935386940247[4] = 1;
   out_9092344935386940247[5] = 0;
   out_9092344935386940247[6] = 0;
   out_9092344935386940247[7] = 0;
   out_9092344935386940247[8] = 0;
   out_9092344935386940247[9] = 0;
   out_9092344935386940247[10] = 0;
   out_9092344935386940247[11] = 0;
   out_9092344935386940247[12] = 0;
   out_9092344935386940247[13] = 0;
   out_9092344935386940247[14] = 1;
   out_9092344935386940247[15] = 0;
   out_9092344935386940247[16] = 0;
   out_9092344935386940247[17] = 0;
}
void h_30(double *state, double *unused, double *out_1597654114537635035) {
   out_1597654114537635035[0] = state[4];
}
void H_30(double *state, double *unused, double *out_9013280603422512715) {
   out_9013280603422512715[0] = 0;
   out_9013280603422512715[1] = 0;
   out_9013280603422512715[2] = 0;
   out_9013280603422512715[3] = 0;
   out_9013280603422512715[4] = 1;
   out_9013280603422512715[5] = 0;
   out_9013280603422512715[6] = 0;
   out_9013280603422512715[7] = 0;
   out_9013280603422512715[8] = 0;
}
void h_26(double *state, double *unused, double *out_2200969999214112552) {
   out_2200969999214112552[0] = state[7];
}
void H_26(double *state, double *unused, double *out_3173627192905734050) {
   out_3173627192905734050[0] = 0;
   out_3173627192905734050[1] = 0;
   out_3173627192905734050[2] = 0;
   out_3173627192905734050[3] = 0;
   out_3173627192905734050[4] = 0;
   out_3173627192905734050[5] = 0;
   out_3173627192905734050[6] = 0;
   out_3173627192905734050[7] = 1;
   out_3173627192905734050[8] = 0;
}
void h_27(double *state, double *unused, double *out_2568775791004018250) {
   out_2568775791004018250[0] = state[3];
}
void H_27(double *state, double *unused, double *out_6789686532238569498) {
   out_6789686532238569498[0] = 0;
   out_6789686532238569498[1] = 0;
   out_6789686532238569498[2] = 0;
   out_6789686532238569498[3] = 1;
   out_6789686532238569498[4] = 0;
   out_6789686532238569498[5] = 0;
   out_6789686532238569498[6] = 0;
   out_6789686532238569498[7] = 0;
   out_6789686532238569498[8] = 0;
}
void h_29(double *state, double *unused, double *out_4202059435346332686) {
   out_4202059435346332686[0] = state[1];
}
void H_29(double *state, double *unused, double *out_8503049259108120531) {
   out_8503049259108120531[0] = 0;
   out_8503049259108120531[1] = 1;
   out_8503049259108120531[2] = 0;
   out_8503049259108120531[3] = 0;
   out_8503049259108120531[4] = 0;
   out_8503049259108120531[5] = 0;
   out_8503049259108120531[6] = 0;
   out_8503049259108120531[7] = 0;
   out_8503049259108120531[8] = 0;
}
void h_28(double *state, double *unused, double *out_1625265938102355815) {
   out_1625265938102355815[0] = state[0];
}
void H_28(double *state, double *unused, double *out_4861295797531900511) {
   out_4861295797531900511[0] = 1;
   out_4861295797531900511[1] = 0;
   out_4861295797531900511[2] = 0;
   out_4861295797531900511[3] = 0;
   out_4861295797531900511[4] = 0;
   out_4861295797531900511[5] = 0;
   out_4861295797531900511[6] = 0;
   out_4861295797531900511[7] = 0;
   out_4861295797531900511[8] = 0;
}
void h_31(double *state, double *unused, double *out_982266084027944228) {
   out_982266084027944228[0] = state[8];
}
void H_31(double *state, double *unused, double *out_2547419090672382574) {
   out_2547419090672382574[0] = 0;
   out_2547419090672382574[1] = 0;
   out_2547419090672382574[2] = 0;
   out_2547419090672382574[3] = 0;
   out_2547419090672382574[4] = 0;
   out_2547419090672382574[5] = 0;
   out_2547419090672382574[6] = 0;
   out_2547419090672382574[7] = 0;
   out_2547419090672382574[8] = 1;
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
void car_err_fun(double *nom_x, double *delta_x, double *out_3334009454934201680) {
  err_fun(nom_x, delta_x, out_3334009454934201680);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_6969690774377442740) {
  inv_err_fun(nom_x, true_x, out_6969690774377442740);
}
void car_H_mod_fun(double *state, double *out_5434705195323386726) {
  H_mod_fun(state, out_5434705195323386726);
}
void car_f_fun(double *state, double dt, double *out_958362648498333800) {
  f_fun(state,  dt, out_958362648498333800);
}
void car_F_fun(double *state, double dt, double *out_8652167330854030009) {
  F_fun(state,  dt, out_8652167330854030009);
}
void car_h_25(double *state, double *unused, double *out_8967817308417867396) {
  h_25(state, unused, out_8967817308417867396);
}
void car_H_25(double *state, double *unused, double *out_6915130511779790274) {
  H_25(state, unused, out_6915130511779790274);
}
void car_h_24(double *state, double *unused, double *out_5626992763966467179) {
  h_24(state, unused, out_5626992763966467179);
}
void car_H_24(double *state, double *unused, double *out_9092344935386940247) {
  H_24(state, unused, out_9092344935386940247);
}
void car_h_30(double *state, double *unused, double *out_1597654114537635035) {
  h_30(state, unused, out_1597654114537635035);
}
void car_H_30(double *state, double *unused, double *out_9013280603422512715) {
  H_30(state, unused, out_9013280603422512715);
}
void car_h_26(double *state, double *unused, double *out_2200969999214112552) {
  h_26(state, unused, out_2200969999214112552);
}
void car_H_26(double *state, double *unused, double *out_3173627192905734050) {
  H_26(state, unused, out_3173627192905734050);
}
void car_h_27(double *state, double *unused, double *out_2568775791004018250) {
  h_27(state, unused, out_2568775791004018250);
}
void car_H_27(double *state, double *unused, double *out_6789686532238569498) {
  H_27(state, unused, out_6789686532238569498);
}
void car_h_29(double *state, double *unused, double *out_4202059435346332686) {
  h_29(state, unused, out_4202059435346332686);
}
void car_H_29(double *state, double *unused, double *out_8503049259108120531) {
  H_29(state, unused, out_8503049259108120531);
}
void car_h_28(double *state, double *unused, double *out_1625265938102355815) {
  h_28(state, unused, out_1625265938102355815);
}
void car_H_28(double *state, double *unused, double *out_4861295797531900511) {
  H_28(state, unused, out_4861295797531900511);
}
void car_h_31(double *state, double *unused, double *out_982266084027944228) {
  h_31(state, unused, out_982266084027944228);
}
void car_H_31(double *state, double *unused, double *out_2547419090672382574) {
  H_31(state, unused, out_2547419090672382574);
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
