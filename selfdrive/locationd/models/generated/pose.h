#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_4095137122478409328);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_698683085994115938);
void pose_H_mod_fun(double *state, double *out_2509686983776334379);
void pose_f_fun(double *state, double dt, double *out_3854505650261104831);
void pose_F_fun(double *state, double dt, double *out_8250233670229774683);
void pose_h_4(double *state, double *unused, double *out_3198903065177037567);
void pose_H_4(double *state, double *unused, double *out_5661603524506085814);
void pose_h_10(double *state, double *unused, double *out_6914485878183044840);
void pose_H_10(double *state, double *unused, double *out_8926024378503649402);
void pose_h_13(double *state, double *unused, double *out_5850172911985206166);
void pose_H_13(double *state, double *unused, double *out_2449329699173753013);
void pose_h_14(double *state, double *unused, double *out_348759912654094184);
void pose_H_14(double *state, double *unused, double *out_1698362668166601285);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}