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
 *                       Code generated with SymPy 1.12                       *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_7200208511236226712) {
   out_7200208511236226712[0] = delta_x[0] + nom_x[0];
   out_7200208511236226712[1] = delta_x[1] + nom_x[1];
   out_7200208511236226712[2] = delta_x[2] + nom_x[2];
   out_7200208511236226712[3] = delta_x[3] + nom_x[3];
   out_7200208511236226712[4] = delta_x[4] + nom_x[4];
   out_7200208511236226712[5] = delta_x[5] + nom_x[5];
   out_7200208511236226712[6] = delta_x[6] + nom_x[6];
   out_7200208511236226712[7] = delta_x[7] + nom_x[7];
   out_7200208511236226712[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_2386257476273920445) {
   out_2386257476273920445[0] = -nom_x[0] + true_x[0];
   out_2386257476273920445[1] = -nom_x[1] + true_x[1];
   out_2386257476273920445[2] = -nom_x[2] + true_x[2];
   out_2386257476273920445[3] = -nom_x[3] + true_x[3];
   out_2386257476273920445[4] = -nom_x[4] + true_x[4];
   out_2386257476273920445[5] = -nom_x[5] + true_x[5];
   out_2386257476273920445[6] = -nom_x[6] + true_x[6];
   out_2386257476273920445[7] = -nom_x[7] + true_x[7];
   out_2386257476273920445[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_5593318448147211148) {
   out_5593318448147211148[0] = 1.0;
   out_5593318448147211148[1] = 0;
   out_5593318448147211148[2] = 0;
   out_5593318448147211148[3] = 0;
   out_5593318448147211148[4] = 0;
   out_5593318448147211148[5] = 0;
   out_5593318448147211148[6] = 0;
   out_5593318448147211148[7] = 0;
   out_5593318448147211148[8] = 0;
   out_5593318448147211148[9] = 0;
   out_5593318448147211148[10] = 1.0;
   out_5593318448147211148[11] = 0;
   out_5593318448147211148[12] = 0;
   out_5593318448147211148[13] = 0;
   out_5593318448147211148[14] = 0;
   out_5593318448147211148[15] = 0;
   out_5593318448147211148[16] = 0;
   out_5593318448147211148[17] = 0;
   out_5593318448147211148[18] = 0;
   out_5593318448147211148[19] = 0;
   out_5593318448147211148[20] = 1.0;
   out_5593318448147211148[21] = 0;
   out_5593318448147211148[22] = 0;
   out_5593318448147211148[23] = 0;
   out_5593318448147211148[24] = 0;
   out_5593318448147211148[25] = 0;
   out_5593318448147211148[26] = 0;
   out_5593318448147211148[27] = 0;
   out_5593318448147211148[28] = 0;
   out_5593318448147211148[29] = 0;
   out_5593318448147211148[30] = 1.0;
   out_5593318448147211148[31] = 0;
   out_5593318448147211148[32] = 0;
   out_5593318448147211148[33] = 0;
   out_5593318448147211148[34] = 0;
   out_5593318448147211148[35] = 0;
   out_5593318448147211148[36] = 0;
   out_5593318448147211148[37] = 0;
   out_5593318448147211148[38] = 0;
   out_5593318448147211148[39] = 0;
   out_5593318448147211148[40] = 1.0;
   out_5593318448147211148[41] = 0;
   out_5593318448147211148[42] = 0;
   out_5593318448147211148[43] = 0;
   out_5593318448147211148[44] = 0;
   out_5593318448147211148[45] = 0;
   out_5593318448147211148[46] = 0;
   out_5593318448147211148[47] = 0;
   out_5593318448147211148[48] = 0;
   out_5593318448147211148[49] = 0;
   out_5593318448147211148[50] = 1.0;
   out_5593318448147211148[51] = 0;
   out_5593318448147211148[52] = 0;
   out_5593318448147211148[53] = 0;
   out_5593318448147211148[54] = 0;
   out_5593318448147211148[55] = 0;
   out_5593318448147211148[56] = 0;
   out_5593318448147211148[57] = 0;
   out_5593318448147211148[58] = 0;
   out_5593318448147211148[59] = 0;
   out_5593318448147211148[60] = 1.0;
   out_5593318448147211148[61] = 0;
   out_5593318448147211148[62] = 0;
   out_5593318448147211148[63] = 0;
   out_5593318448147211148[64] = 0;
   out_5593318448147211148[65] = 0;
   out_5593318448147211148[66] = 0;
   out_5593318448147211148[67] = 0;
   out_5593318448147211148[68] = 0;
   out_5593318448147211148[69] = 0;
   out_5593318448147211148[70] = 1.0;
   out_5593318448147211148[71] = 0;
   out_5593318448147211148[72] = 0;
   out_5593318448147211148[73] = 0;
   out_5593318448147211148[74] = 0;
   out_5593318448147211148[75] = 0;
   out_5593318448147211148[76] = 0;
   out_5593318448147211148[77] = 0;
   out_5593318448147211148[78] = 0;
   out_5593318448147211148[79] = 0;
   out_5593318448147211148[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_8230770176607773732) {
   out_8230770176607773732[0] = state[0];
   out_8230770176607773732[1] = state[1];
   out_8230770176607773732[2] = state[2];
   out_8230770176607773732[3] = state[3];
   out_8230770176607773732[4] = state[4];
   out_8230770176607773732[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8000000000000007*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_8230770176607773732[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_8230770176607773732[7] = state[7];
   out_8230770176607773732[8] = state[8];
}
void F_fun(double *state, double dt, double *out_8899636511369837029) {
   out_8899636511369837029[0] = 1;
   out_8899636511369837029[1] = 0;
   out_8899636511369837029[2] = 0;
   out_8899636511369837029[3] = 0;
   out_8899636511369837029[4] = 0;
   out_8899636511369837029[5] = 0;
   out_8899636511369837029[6] = 0;
   out_8899636511369837029[7] = 0;
   out_8899636511369837029[8] = 0;
   out_8899636511369837029[9] = 0;
   out_8899636511369837029[10] = 1;
   out_8899636511369837029[11] = 0;
   out_8899636511369837029[12] = 0;
   out_8899636511369837029[13] = 0;
   out_8899636511369837029[14] = 0;
   out_8899636511369837029[15] = 0;
   out_8899636511369837029[16] = 0;
   out_8899636511369837029[17] = 0;
   out_8899636511369837029[18] = 0;
   out_8899636511369837029[19] = 0;
   out_8899636511369837029[20] = 1;
   out_8899636511369837029[21] = 0;
   out_8899636511369837029[22] = 0;
   out_8899636511369837029[23] = 0;
   out_8899636511369837029[24] = 0;
   out_8899636511369837029[25] = 0;
   out_8899636511369837029[26] = 0;
   out_8899636511369837029[27] = 0;
   out_8899636511369837029[28] = 0;
   out_8899636511369837029[29] = 0;
   out_8899636511369837029[30] = 1;
   out_8899636511369837029[31] = 0;
   out_8899636511369837029[32] = 0;
   out_8899636511369837029[33] = 0;
   out_8899636511369837029[34] = 0;
   out_8899636511369837029[35] = 0;
   out_8899636511369837029[36] = 0;
   out_8899636511369837029[37] = 0;
   out_8899636511369837029[38] = 0;
   out_8899636511369837029[39] = 0;
   out_8899636511369837029[40] = 1;
   out_8899636511369837029[41] = 0;
   out_8899636511369837029[42] = 0;
   out_8899636511369837029[43] = 0;
   out_8899636511369837029[44] = 0;
   out_8899636511369837029[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_8899636511369837029[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_8899636511369837029[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_8899636511369837029[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_8899636511369837029[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_8899636511369837029[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_8899636511369837029[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_8899636511369837029[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_8899636511369837029[53] = -9.8000000000000007*dt;
   out_8899636511369837029[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_8899636511369837029[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_8899636511369837029[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_8899636511369837029[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_8899636511369837029[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_8899636511369837029[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_8899636511369837029[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_8899636511369837029[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_8899636511369837029[62] = 0;
   out_8899636511369837029[63] = 0;
   out_8899636511369837029[64] = 0;
   out_8899636511369837029[65] = 0;
   out_8899636511369837029[66] = 0;
   out_8899636511369837029[67] = 0;
   out_8899636511369837029[68] = 0;
   out_8899636511369837029[69] = 0;
   out_8899636511369837029[70] = 1;
   out_8899636511369837029[71] = 0;
   out_8899636511369837029[72] = 0;
   out_8899636511369837029[73] = 0;
   out_8899636511369837029[74] = 0;
   out_8899636511369837029[75] = 0;
   out_8899636511369837029[76] = 0;
   out_8899636511369837029[77] = 0;
   out_8899636511369837029[78] = 0;
   out_8899636511369837029[79] = 0;
   out_8899636511369837029[80] = 1;
}
void h_25(double *state, double *unused, double *out_4956029848088728254) {
   out_4956029848088728254[0] = state[6];
}
void H_25(double *state, double *unused, double *out_6287126815192398706) {
   out_6287126815192398706[0] = 0;
   out_6287126815192398706[1] = 0;
   out_6287126815192398706[2] = 0;
   out_6287126815192398706[3] = 0;
   out_6287126815192398706[4] = 0;
   out_6287126815192398706[5] = 0;
   out_6287126815192398706[6] = 1;
   out_6287126815192398706[7] = 0;
   out_6287126815192398706[8] = 0;
}
void h_24(double *state, double *unused, double *out_9171526715805895907) {
   out_9171526715805895907[0] = state[4];
   out_9171526715805895907[1] = state[5];
}
void H_24(double *state, double *unused, double *out_367078642915787051) {
   out_367078642915787051[0] = 0;
   out_367078642915787051[1] = 0;
   out_367078642915787051[2] = 0;
   out_367078642915787051[3] = 0;
   out_367078642915787051[4] = 1;
   out_367078642915787051[5] = 0;
   out_367078642915787051[6] = 0;
   out_367078642915787051[7] = 0;
   out_367078642915787051[8] = 0;
   out_367078642915787051[9] = 0;
   out_367078642915787051[10] = 0;
   out_367078642915787051[11] = 0;
   out_367078642915787051[12] = 0;
   out_367078642915787051[13] = 0;
   out_367078642915787051[14] = 1;
   out_367078642915787051[15] = 0;
   out_367078642915787051[16] = 0;
   out_367078642915787051[17] = 0;
}
void h_30(double *state, double *unused, double *out_6133055380926781425) {
   out_6133055380926781425[0] = state[4];
}
void H_30(double *state, double *unused, double *out_7631920928389544712) {
   out_7631920928389544712[0] = 0;
   out_7631920928389544712[1] = 0;
   out_7631920928389544712[2] = 0;
   out_7631920928389544712[3] = 0;
   out_7631920928389544712[4] = 1;
   out_7631920928389544712[5] = 0;
   out_7631920928389544712[6] = 0;
   out_7631920928389544712[7] = 0;
   out_7631920928389544712[8] = 0;
}
void h_26(double *state, double *unused, double *out_375070466933043469) {
   out_375070466933043469[0] = state[7];
}
void H_26(double *state, double *unused, double *out_8418113939643096686) {
   out_8418113939643096686[0] = 0;
   out_8418113939643096686[1] = 0;
   out_8418113939643096686[2] = 0;
   out_8418113939643096686[3] = 0;
   out_8418113939643096686[4] = 0;
   out_8418113939643096686[5] = 0;
   out_8418113939643096686[6] = 0;
   out_8418113939643096686[7] = 1;
   out_8418113939643096686[8] = 0;
}
void h_27(double *state, double *unused, double *out_7992815899533060939) {
   out_7992815899533060939[0] = state[3];
}
void H_27(double *state, double *unused, double *out_5457157616589119801) {
   out_5457157616589119801[0] = 0;
   out_5457157616589119801[1] = 0;
   out_5457157616589119801[2] = 0;
   out_5457157616589119801[3] = 1;
   out_5457157616589119801[4] = 0;
   out_5457157616589119801[5] = 0;
   out_5457157616589119801[6] = 0;
   out_5457157616589119801[7] = 0;
   out_5457157616589119801[8] = 0;
}
void h_29(double *state, double *unused, double *out_3683092947826139741) {
   out_3683092947826139741[0] = state[1];
}
void H_29(double *state, double *unused, double *out_8142152272703936896) {
   out_8142152272703936896[0] = 0;
   out_8142152272703936896[1] = 1;
   out_8142152272703936896[2] = 0;
   out_8142152272703936896[3] = 0;
   out_8142152272703936896[4] = 0;
   out_8142152272703936896[5] = 0;
   out_8142152272703936896[6] = 0;
   out_8142152272703936896[7] = 0;
   out_8142152272703936896[8] = 0;
}
void h_28(double *state, double *unused, double *out_3594863818251431927) {
   out_3594863818251431927[0] = state[0];
}
void H_28(double *state, double *unused, double *out_3059753255634406322) {
   out_3059753255634406322[0] = 1;
   out_3059753255634406322[1] = 0;
   out_3059753255634406322[2] = 0;
   out_3059753255634406322[3] = 0;
   out_3059753255634406322[4] = 0;
   out_3059753255634406322[5] = 0;
   out_3059753255634406322[6] = 0;
   out_3059753255634406322[7] = 0;
   out_3059753255634406322[8] = 0;
}
void h_31(double *state, double *unused, double *out_2504528916160160071) {
   out_2504528916160160071[0] = state[8];
}
void H_31(double *state, double *unused, double *out_7791905837409745210) {
   out_7791905837409745210[0] = 0;
   out_7791905837409745210[1] = 0;
   out_7791905837409745210[2] = 0;
   out_7791905837409745210[3] = 0;
   out_7791905837409745210[4] = 0;
   out_7791905837409745210[5] = 0;
   out_7791905837409745210[6] = 0;
   out_7791905837409745210[7] = 0;
   out_7791905837409745210[8] = 1;
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
void car_err_fun(double *nom_x, double *delta_x, double *out_7200208511236226712) {
  err_fun(nom_x, delta_x, out_7200208511236226712);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_2386257476273920445) {
  inv_err_fun(nom_x, true_x, out_2386257476273920445);
}
void car_H_mod_fun(double *state, double *out_5593318448147211148) {
  H_mod_fun(state, out_5593318448147211148);
}
void car_f_fun(double *state, double dt, double *out_8230770176607773732) {
  f_fun(state,  dt, out_8230770176607773732);
}
void car_F_fun(double *state, double dt, double *out_8899636511369837029) {
  F_fun(state,  dt, out_8899636511369837029);
}
void car_h_25(double *state, double *unused, double *out_4956029848088728254) {
  h_25(state, unused, out_4956029848088728254);
}
void car_H_25(double *state, double *unused, double *out_6287126815192398706) {
  H_25(state, unused, out_6287126815192398706);
}
void car_h_24(double *state, double *unused, double *out_9171526715805895907) {
  h_24(state, unused, out_9171526715805895907);
}
void car_H_24(double *state, double *unused, double *out_367078642915787051) {
  H_24(state, unused, out_367078642915787051);
}
void car_h_30(double *state, double *unused, double *out_6133055380926781425) {
  h_30(state, unused, out_6133055380926781425);
}
void car_H_30(double *state, double *unused, double *out_7631920928389544712) {
  H_30(state, unused, out_7631920928389544712);
}
void car_h_26(double *state, double *unused, double *out_375070466933043469) {
  h_26(state, unused, out_375070466933043469);
}
void car_H_26(double *state, double *unused, double *out_8418113939643096686) {
  H_26(state, unused, out_8418113939643096686);
}
void car_h_27(double *state, double *unused, double *out_7992815899533060939) {
  h_27(state, unused, out_7992815899533060939);
}
void car_H_27(double *state, double *unused, double *out_5457157616589119801) {
  H_27(state, unused, out_5457157616589119801);
}
void car_h_29(double *state, double *unused, double *out_3683092947826139741) {
  h_29(state, unused, out_3683092947826139741);
}
void car_H_29(double *state, double *unused, double *out_8142152272703936896) {
  H_29(state, unused, out_8142152272703936896);
}
void car_h_28(double *state, double *unused, double *out_3594863818251431927) {
  h_28(state, unused, out_3594863818251431927);
}
void car_H_28(double *state, double *unused, double *out_3059753255634406322) {
  H_28(state, unused, out_3059753255634406322);
}
void car_h_31(double *state, double *unused, double *out_2504528916160160071) {
  h_31(state, unused, out_2504528916160160071);
}
void car_H_31(double *state, double *unused, double *out_7791905837409745210) {
  H_31(state, unused, out_7791905837409745210);
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
