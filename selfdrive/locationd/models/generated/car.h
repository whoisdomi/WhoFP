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
void car_err_fun(double *nom_x, double *delta_x, double *out_602274555864179715);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_1813808766499165488);
void car_H_mod_fun(double *state, double *out_6993529450035416664);
void car_f_fun(double *state, double dt, double *out_8128556128886940709);
void car_F_fun(double *state, double dt, double *out_197190331083741563);
void car_h_25(double *state, double *unused, double *out_3672452708460500252);
void car_H_25(double *state, double *unused, double *out_6658383780746265843);
void car_h_24(double *state, double *unused, double *out_7683485618314791650);
void car_H_24(double *state, double *unused, double *out_2560295106894090548);
void car_h_30(double *state, double *unused, double *out_4208819942293548933);
void car_H_30(double *state, double *unused, double *out_2130687450618657645);
void car_h_26(double *state, double *unused, double *out_792853325367296712);
void car_H_26(double *state, double *unused, double *out_2916880461872209619);
void car_h_27(double *state, double *unused, double *out_1329220559200345393);
void car_H_27(double *state, double *unused, double *out_44075861181767266);
void car_h_29(double *state, double *unused, double *out_6434044697620327048);
void car_H_29(double *state, double *unused, double *out_2640918794933049829);
void car_h_28(double *state, double *unused, double *out_7431739994933539783);
void car_H_28(double *state, double *unused, double *out_4604549066498376080);
void car_h_31(double *state, double *unused, double *out_4824207972803239740);
void car_H_31(double *state, double *unused, double *out_2290672359638858143);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}