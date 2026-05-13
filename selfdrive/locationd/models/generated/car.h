#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_err_fun(double *nom_x, double *delta_x, double *out_2845208887048640800);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_7468417448096213155);
void car_H_mod_fun(double *state, double *out_2689788822724729528);
void car_f_fun(double *state, double dt, double *out_3024710561763927263);
void car_F_fun(double *state, double dt, double *out_2653129196283778933);
void car_h_25(double *state, double *unused, double *out_7068450151461527312);
void car_H_25(double *state, double *unused, double *out_7593480647909394368);
void car_h_24(double *state, double *unused, double *out_4610623337872525875);
void car_H_24(double *state, double *unused, double *out_8676049002193007275);
void car_h_30(double *state, double *unused, double *out_6793256089177021423);
void car_H_30(double *state, double *unused, double *out_3065784317781786170);
void car_h_26(double *state, double *unused, double *out_8945578436283843025);
void car_H_26(double *state, double *unused, double *out_3851977329035338144);
void car_h_27(double *state, double *unused, double *out_6465134266785049795);
void car_H_27(double *state, double *unused, double *out_891021005981361259);
void car_h_29(double *state, double *unused, double *out_3517408276429564834);
void car_H_29(double *state, double *unused, double *out_3576015662096178354);
void car_h_28(double *state, double *unused, double *out_2999640634917661056);
void car_H_28(double *state, double *unused, double *out_5539645933661504605);
void car_h_31(double *state, double *unused, double *out_7740844393994001954);
void car_H_31(double *state, double *unused, double *out_3225769226801986668);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}