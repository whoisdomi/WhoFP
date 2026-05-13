#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_3243830430541554586);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_6240387057974568702);
void pose_H_mod_fun(double *state, double *out_8043295050634907169);
void pose_f_fun(double *state, double dt, double *out_1922886578098749649);
void pose_F_fun(double *state, double dt, double *out_3887137653751174596);
void pose_h_4(double *state, double *unused, double *out_6757832056893505158);
void pose_H_4(double *state, double *unused, double *out_4271487840790999597);
void pose_h_10(double *state, double *unused, double *out_1896753776489155926);
void pose_H_10(double *state, double *unused, double *out_5115826266223761764);
void pose_h_13(double *state, double *unused, double *out_3373504601145192164);
void pose_H_13(double *state, double *unused, double *out_3706885921109155493);
void pose_h_14(double *state, double *unused, double *out_6472387792798353966);
void pose_H_14(double *state, double *unused, double *out_7354276273086371893);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}