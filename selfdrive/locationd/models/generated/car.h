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
void car_err_fun(double *nom_x, double *delta_x, double *out_3334009454934201680);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_6969690774377442740);
void car_H_mod_fun(double *state, double *out_5434705195323386726);
void car_f_fun(double *state, double dt, double *out_958362648498333800);
void car_F_fun(double *state, double dt, double *out_8652167330854030009);
void car_h_25(double *state, double *unused, double *out_8967817308417867396);
void car_H_25(double *state, double *unused, double *out_6915130511779790274);
void car_h_24(double *state, double *unused, double *out_5626992763966467179);
void car_H_24(double *state, double *unused, double *out_9092344935386940247);
void car_h_30(double *state, double *unused, double *out_1597654114537635035);
void car_H_30(double *state, double *unused, double *out_9013280603422512715);
void car_h_26(double *state, double *unused, double *out_2200969999214112552);
void car_H_26(double *state, double *unused, double *out_3173627192905734050);
void car_h_27(double *state, double *unused, double *out_2568775791004018250);
void car_H_27(double *state, double *unused, double *out_6789686532238569498);
void car_h_29(double *state, double *unused, double *out_4202059435346332686);
void car_H_29(double *state, double *unused, double *out_8503049259108120531);
void car_h_28(double *state, double *unused, double *out_1625265938102355815);
void car_H_28(double *state, double *unused, double *out_4861295797531900511);
void car_h_31(double *state, double *unused, double *out_982266084027944228);
void car_H_31(double *state, double *unused, double *out_2547419090672382574);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}