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
void car_err_fun(double *nom_x, double *delta_x, double *out_4284318502583774505);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_9062210534970943629);
void car_H_mod_fun(double *state, double *out_5030433578682660012);
void car_f_fun(double *state, double dt, double *out_284361389350267453);
void car_F_fun(double *state, double dt, double *out_3718156729130345156);
void car_h_25(double *state, double *unused, double *out_8289407373313103834);
void car_H_25(double *state, double *unused, double *out_4223122269114654367);
void car_h_24(double *state, double *unused, double *out_2858732842488423539);
void car_H_24(double *state, double *unused, double *out_6448830053093522929);
void car_h_30(double *state, double *unused, double *out_350470103405438050);
void car_H_30(double *state, double *unused, double *out_7306931463103280494);
void car_h_26(double *state, double *unused, double *out_2686315948038894635);
void car_H_26(double *state, double *unused, double *out_481618950240598143);
void car_h_27(double *state, double *unused, double *out_9175252607061149670);
void car_H_27(double *state, double *unused, double *out_8965049298805846211);
void car_h_29(double *state, double *unused, double *out_1293979956307100485);
void car_H_29(double *state, double *unused, double *out_6796700118788888310);
void car_h_28(double *state, double *unused, double *out_4173579339400304025);
void car_H_28(double *state, double *unused, double *out_6567644937851132732);
void car_h_31(double *state, double *unused, double *out_3386501474046096846);
void car_H_31(double *state, double *unused, double *out_4253768230991614795);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}