#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_3175539824002796945);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_2251372369976179325);
void pose_H_mod_fun(double *state, double *out_5180136592465146401);
void pose_f_fun(double *state, double dt, double *out_8240783831830882114);
void pose_F_fun(double *state, double dt, double *out_8106921975506752195);
void pose_h_4(double *state, double *unused, double *out_8584561754466655706);
void pose_H_4(double *state, double *unused, double *out_1408329382621238829);
void pose_h_10(double *state, double *unused, double *out_8425326832718263788);
void pose_H_10(double *state, double *unused, double *out_7742185847493432477);
void pose_h_13(double *state, double *unused, double *out_3427524069920046808);
void pose_H_13(double *state, double *unused, double *out_1803944442711093972);
void pose_h_14(double *state, double *unused, double *out_3360686240400799473);
void pose_H_14(double *state, double *unused, double *out_2554911473718245700);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}