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
void err_fun(double *nom_x, double *delta_x, double *out_7094358906068685897) {
   out_7094358906068685897[0] = delta_x[0] + nom_x[0];
   out_7094358906068685897[1] = delta_x[1] + nom_x[1];
   out_7094358906068685897[2] = delta_x[2] + nom_x[2];
   out_7094358906068685897[3] = delta_x[3] + nom_x[3];
   out_7094358906068685897[4] = delta_x[4] + nom_x[4];
   out_7094358906068685897[5] = delta_x[5] + nom_x[5];
   out_7094358906068685897[6] = delta_x[6] + nom_x[6];
   out_7094358906068685897[7] = delta_x[7] + nom_x[7];
   out_7094358906068685897[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_6974837954023764737) {
   out_6974837954023764737[0] = -nom_x[0] + true_x[0];
   out_6974837954023764737[1] = -nom_x[1] + true_x[1];
   out_6974837954023764737[2] = -nom_x[2] + true_x[2];
   out_6974837954023764737[3] = -nom_x[3] + true_x[3];
   out_6974837954023764737[4] = -nom_x[4] + true_x[4];
   out_6974837954023764737[5] = -nom_x[5] + true_x[5];
   out_6974837954023764737[6] = -nom_x[6] + true_x[6];
   out_6974837954023764737[7] = -nom_x[7] + true_x[7];
   out_6974837954023764737[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_8987002573912775346) {
   out_8987002573912775346[0] = 1.0;
   out_8987002573912775346[1] = 0.0;
   out_8987002573912775346[2] = 0.0;
   out_8987002573912775346[3] = 0.0;
   out_8987002573912775346[4] = 0.0;
   out_8987002573912775346[5] = 0.0;
   out_8987002573912775346[6] = 0.0;
   out_8987002573912775346[7] = 0.0;
   out_8987002573912775346[8] = 0.0;
   out_8987002573912775346[9] = 0.0;
   out_8987002573912775346[10] = 1.0;
   out_8987002573912775346[11] = 0.0;
   out_8987002573912775346[12] = 0.0;
   out_8987002573912775346[13] = 0.0;
   out_8987002573912775346[14] = 0.0;
   out_8987002573912775346[15] = 0.0;
   out_8987002573912775346[16] = 0.0;
   out_8987002573912775346[17] = 0.0;
   out_8987002573912775346[18] = 0.0;
   out_8987002573912775346[19] = 0.0;
   out_8987002573912775346[20] = 1.0;
   out_8987002573912775346[21] = 0.0;
   out_8987002573912775346[22] = 0.0;
   out_8987002573912775346[23] = 0.0;
   out_8987002573912775346[24] = 0.0;
   out_8987002573912775346[25] = 0.0;
   out_8987002573912775346[26] = 0.0;
   out_8987002573912775346[27] = 0.0;
   out_8987002573912775346[28] = 0.0;
   out_8987002573912775346[29] = 0.0;
   out_8987002573912775346[30] = 1.0;
   out_8987002573912775346[31] = 0.0;
   out_8987002573912775346[32] = 0.0;
   out_8987002573912775346[33] = 0.0;
   out_8987002573912775346[34] = 0.0;
   out_8987002573912775346[35] = 0.0;
   out_8987002573912775346[36] = 0.0;
   out_8987002573912775346[37] = 0.0;
   out_8987002573912775346[38] = 0.0;
   out_8987002573912775346[39] = 0.0;
   out_8987002573912775346[40] = 1.0;
   out_8987002573912775346[41] = 0.0;
   out_8987002573912775346[42] = 0.0;
   out_8987002573912775346[43] = 0.0;
   out_8987002573912775346[44] = 0.0;
   out_8987002573912775346[45] = 0.0;
   out_8987002573912775346[46] = 0.0;
   out_8987002573912775346[47] = 0.0;
   out_8987002573912775346[48] = 0.0;
   out_8987002573912775346[49] = 0.0;
   out_8987002573912775346[50] = 1.0;
   out_8987002573912775346[51] = 0.0;
   out_8987002573912775346[52] = 0.0;
   out_8987002573912775346[53] = 0.0;
   out_8987002573912775346[54] = 0.0;
   out_8987002573912775346[55] = 0.0;
   out_8987002573912775346[56] = 0.0;
   out_8987002573912775346[57] = 0.0;
   out_8987002573912775346[58] = 0.0;
   out_8987002573912775346[59] = 0.0;
   out_8987002573912775346[60] = 1.0;
   out_8987002573912775346[61] = 0.0;
   out_8987002573912775346[62] = 0.0;
   out_8987002573912775346[63] = 0.0;
   out_8987002573912775346[64] = 0.0;
   out_8987002573912775346[65] = 0.0;
   out_8987002573912775346[66] = 0.0;
   out_8987002573912775346[67] = 0.0;
   out_8987002573912775346[68] = 0.0;
   out_8987002573912775346[69] = 0.0;
   out_8987002573912775346[70] = 1.0;
   out_8987002573912775346[71] = 0.0;
   out_8987002573912775346[72] = 0.0;
   out_8987002573912775346[73] = 0.0;
   out_8987002573912775346[74] = 0.0;
   out_8987002573912775346[75] = 0.0;
   out_8987002573912775346[76] = 0.0;
   out_8987002573912775346[77] = 0.0;
   out_8987002573912775346[78] = 0.0;
   out_8987002573912775346[79] = 0.0;
   out_8987002573912775346[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_7968249393634225210) {
   out_7968249393634225210[0] = state[0];
   out_7968249393634225210[1] = state[1];
   out_7968249393634225210[2] = state[2];
   out_7968249393634225210[3] = state[3];
   out_7968249393634225210[4] = state[4];
   out_7968249393634225210[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8100000000000005*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_7968249393634225210[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_7968249393634225210[7] = state[7];
   out_7968249393634225210[8] = state[8];
}
void F_fun(double *state, double dt, double *out_933118493395579449) {
   out_933118493395579449[0] = 1;
   out_933118493395579449[1] = 0;
   out_933118493395579449[2] = 0;
   out_933118493395579449[3] = 0;
   out_933118493395579449[4] = 0;
   out_933118493395579449[5] = 0;
   out_933118493395579449[6] = 0;
   out_933118493395579449[7] = 0;
   out_933118493395579449[8] = 0;
   out_933118493395579449[9] = 0;
   out_933118493395579449[10] = 1;
   out_933118493395579449[11] = 0;
   out_933118493395579449[12] = 0;
   out_933118493395579449[13] = 0;
   out_933118493395579449[14] = 0;
   out_933118493395579449[15] = 0;
   out_933118493395579449[16] = 0;
   out_933118493395579449[17] = 0;
   out_933118493395579449[18] = 0;
   out_933118493395579449[19] = 0;
   out_933118493395579449[20] = 1;
   out_933118493395579449[21] = 0;
   out_933118493395579449[22] = 0;
   out_933118493395579449[23] = 0;
   out_933118493395579449[24] = 0;
   out_933118493395579449[25] = 0;
   out_933118493395579449[26] = 0;
   out_933118493395579449[27] = 0;
   out_933118493395579449[28] = 0;
   out_933118493395579449[29] = 0;
   out_933118493395579449[30] = 1;
   out_933118493395579449[31] = 0;
   out_933118493395579449[32] = 0;
   out_933118493395579449[33] = 0;
   out_933118493395579449[34] = 0;
   out_933118493395579449[35] = 0;
   out_933118493395579449[36] = 0;
   out_933118493395579449[37] = 0;
   out_933118493395579449[38] = 0;
   out_933118493395579449[39] = 0;
   out_933118493395579449[40] = 1;
   out_933118493395579449[41] = 0;
   out_933118493395579449[42] = 0;
   out_933118493395579449[43] = 0;
   out_933118493395579449[44] = 0;
   out_933118493395579449[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_933118493395579449[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_933118493395579449[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_933118493395579449[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_933118493395579449[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_933118493395579449[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_933118493395579449[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_933118493395579449[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_933118493395579449[53] = -9.8100000000000005*dt;
   out_933118493395579449[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_933118493395579449[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_933118493395579449[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_933118493395579449[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_933118493395579449[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_933118493395579449[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_933118493395579449[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_933118493395579449[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_933118493395579449[62] = 0;
   out_933118493395579449[63] = 0;
   out_933118493395579449[64] = 0;
   out_933118493395579449[65] = 0;
   out_933118493395579449[66] = 0;
   out_933118493395579449[67] = 0;
   out_933118493395579449[68] = 0;
   out_933118493395579449[69] = 0;
   out_933118493395579449[70] = 1;
   out_933118493395579449[71] = 0;
   out_933118493395579449[72] = 0;
   out_933118493395579449[73] = 0;
   out_933118493395579449[74] = 0;
   out_933118493395579449[75] = 0;
   out_933118493395579449[76] = 0;
   out_933118493395579449[77] = 0;
   out_933118493395579449[78] = 0;
   out_933118493395579449[79] = 0;
   out_933118493395579449[80] = 1;
}
void h_25(double *state, double *unused, double *out_1524326060687862043) {
   out_1524326060687862043[0] = state[6];
}
void H_25(double *state, double *unused, double *out_6350445648897349152) {
   out_6350445648897349152[0] = 0;
   out_6350445648897349152[1] = 0;
   out_6350445648897349152[2] = 0;
   out_6350445648897349152[3] = 0;
   out_6350445648897349152[4] = 0;
   out_6350445648897349152[5] = 0;
   out_6350445648897349152[6] = 1;
   out_6350445648897349152[7] = 0;
   out_6350445648897349152[8] = 0;
}
void h_24(double *state, double *unused, double *out_5671295414922481183) {
   out_5671295414922481183[0] = state[4];
   out_5671295414922481183[1] = state[5];
}
void H_24(double *state, double *unused, double *out_2868233238743007239) {
   out_2868233238743007239[0] = 0;
   out_2868233238743007239[1] = 0;
   out_2868233238743007239[2] = 0;
   out_2868233238743007239[3] = 0;
   out_2868233238743007239[4] = 1;
   out_2868233238743007239[5] = 0;
   out_2868233238743007239[6] = 0;
   out_2868233238743007239[7] = 0;
   out_2868233238743007239[8] = 0;
   out_2868233238743007239[9] = 0;
   out_2868233238743007239[10] = 0;
   out_2868233238743007239[11] = 0;
   out_2868233238743007239[12] = 0;
   out_2868233238743007239[13] = 0;
   out_2868233238743007239[14] = 1;
   out_2868233238743007239[15] = 0;
   out_2868233238743007239[16] = 0;
   out_2868233238743007239[17] = 0;
}
void h_30(double *state, double *unused, double *out_4642787979283272260) {
   out_4642787979283272260[0] = state[4];
}
void H_30(double *state, double *unused, double *out_1822749318769740954) {
   out_1822749318769740954[0] = 0;
   out_1822749318769740954[1] = 0;
   out_1822749318769740954[2] = 0;
   out_1822749318769740954[3] = 0;
   out_1822749318769740954[4] = 1;
   out_1822749318769740954[5] = 0;
   out_1822749318769740954[6] = 0;
   out_1822749318769740954[7] = 0;
   out_1822749318769740954[8] = 0;
}
void h_26(double *state, double *unused, double *out_8566727210287675458) {
   out_8566727210287675458[0] = state[7];
}
void H_26(double *state, double *unused, double *out_2608942330023292928) {
   out_2608942330023292928[0] = 0;
   out_2608942330023292928[1] = 0;
   out_2608942330023292928[2] = 0;
   out_2608942330023292928[3] = 0;
   out_2608942330023292928[4] = 0;
   out_2608942330023292928[5] = 0;
   out_2608942330023292928[6] = 0;
   out_2608942330023292928[7] = 1;
   out_2608942330023292928[8] = 0;
}
void h_27(double *state, double *unused, double *out_6608429876303459109) {
   out_6608429876303459109[0] = state[3];
}
void H_27(double *state, double *unused, double *out_352013993030683957) {
   out_352013993030683957[0] = 0;
   out_352013993030683957[1] = 0;
   out_352013993030683957[2] = 0;
   out_352013993030683957[3] = 1;
   out_352013993030683957[4] = 0;
   out_352013993030683957[5] = 0;
   out_352013993030683957[6] = 0;
   out_352013993030683957[7] = 0;
   out_352013993030683957[8] = 0;
}
void h_29(double *state, double *unused, double *out_8059061268932342276) {
   out_8059061268932342276[0] = state[1];
}
void H_29(double *state, double *unused, double *out_2332980663084133138) {
   out_2332980663084133138[0] = 0;
   out_2332980663084133138[1] = 1;
   out_2332980663084133138[2] = 0;
   out_2332980663084133138[3] = 0;
   out_2332980663084133138[4] = 0;
   out_2332980663084133138[5] = 0;
   out_2332980663084133138[6] = 0;
   out_2332980663084133138[7] = 0;
   out_2332980663084133138[8] = 0;
}
void h_28(double *state, double *unused, double *out_965700005222502274) {
   out_965700005222502274[0] = state[0];
}
void H_28(double *state, double *unused, double *out_4296610934649459389) {
   out_4296610934649459389[0] = 1;
   out_4296610934649459389[1] = 0;
   out_4296610934649459389[2] = 0;
   out_4296610934649459389[3] = 0;
   out_4296610934649459389[4] = 0;
   out_4296610934649459389[5] = 0;
   out_4296610934649459389[6] = 0;
   out_4296610934649459389[7] = 0;
   out_4296610934649459389[8] = 0;
}
void h_31(double *state, double *unused, double *out_2974957453316745210) {
   out_2974957453316745210[0] = state[8];
}
void H_31(double *state, double *unused, double *out_1982734227789941452) {
   out_1982734227789941452[0] = 0;
   out_1982734227789941452[1] = 0;
   out_1982734227789941452[2] = 0;
   out_1982734227789941452[3] = 0;
   out_1982734227789941452[4] = 0;
   out_1982734227789941452[5] = 0;
   out_1982734227789941452[6] = 0;
   out_1982734227789941452[7] = 0;
   out_1982734227789941452[8] = 1;
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
void car_err_fun(double *nom_x, double *delta_x, double *out_7094358906068685897) {
  err_fun(nom_x, delta_x, out_7094358906068685897);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_6974837954023764737) {
  inv_err_fun(nom_x, true_x, out_6974837954023764737);
}
void car_H_mod_fun(double *state, double *out_8987002573912775346) {
  H_mod_fun(state, out_8987002573912775346);
}
void car_f_fun(double *state, double dt, double *out_7968249393634225210) {
  f_fun(state,  dt, out_7968249393634225210);
}
void car_F_fun(double *state, double dt, double *out_933118493395579449) {
  F_fun(state,  dt, out_933118493395579449);
}
void car_h_25(double *state, double *unused, double *out_1524326060687862043) {
  h_25(state, unused, out_1524326060687862043);
}
void car_H_25(double *state, double *unused, double *out_6350445648897349152) {
  H_25(state, unused, out_6350445648897349152);
}
void car_h_24(double *state, double *unused, double *out_5671295414922481183) {
  h_24(state, unused, out_5671295414922481183);
}
void car_H_24(double *state, double *unused, double *out_2868233238743007239) {
  H_24(state, unused, out_2868233238743007239);
}
void car_h_30(double *state, double *unused, double *out_4642787979283272260) {
  h_30(state, unused, out_4642787979283272260);
}
void car_H_30(double *state, double *unused, double *out_1822749318769740954) {
  H_30(state, unused, out_1822749318769740954);
}
void car_h_26(double *state, double *unused, double *out_8566727210287675458) {
  h_26(state, unused, out_8566727210287675458);
}
void car_H_26(double *state, double *unused, double *out_2608942330023292928) {
  H_26(state, unused, out_2608942330023292928);
}
void car_h_27(double *state, double *unused, double *out_6608429876303459109) {
  h_27(state, unused, out_6608429876303459109);
}
void car_H_27(double *state, double *unused, double *out_352013993030683957) {
  H_27(state, unused, out_352013993030683957);
}
void car_h_29(double *state, double *unused, double *out_8059061268932342276) {
  h_29(state, unused, out_8059061268932342276);
}
void car_H_29(double *state, double *unused, double *out_2332980663084133138) {
  H_29(state, unused, out_2332980663084133138);
}
void car_h_28(double *state, double *unused, double *out_965700005222502274) {
  h_28(state, unused, out_965700005222502274);
}
void car_H_28(double *state, double *unused, double *out_4296610934649459389) {
  H_28(state, unused, out_4296610934649459389);
}
void car_h_31(double *state, double *unused, double *out_2974957453316745210) {
  h_31(state, unused, out_2974957453316745210);
}
void car_H_31(double *state, double *unused, double *out_1982734227789941452) {
  H_31(state, unused, out_1982734227789941452);
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
