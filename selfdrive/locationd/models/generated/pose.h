#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_4086162596914389259);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_628093630908614818);
void pose_H_mod_fun(double *state, double *out_920885569904575623);
void pose_f_fun(double *state, double dt, double *out_4638827357690861687);
void pose_F_fun(double *state, double dt, double *out_2122913628783415921);
void pose_h_4(double *state, double *unused, double *out_8610552428979210253);
void pose_H_4(double *state, double *unused, double *out_898813156001721238);
void pose_h_10(double *state, double *unused, double *out_6115253449497749874);
void pose_H_10(double *state, double *unused, double *out_795420154396972986);
void pose_h_13(double *state, double *unused, double *out_7913228155057385150);
void pose_H_13(double *state, double *unused, double *out_6711818052314979691);
void pose_h_14(double *state, double *unused, double *out_6835226830806042526);
void pose_H_14(double *state, double *unused, double *out_3064427700337763291);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}