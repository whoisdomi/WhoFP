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
void car_err_fun(double *nom_x, double *delta_x, double *out_7094358906068685897);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_6974837954023764737);
void car_H_mod_fun(double *state, double *out_8987002573912775346);
void car_f_fun(double *state, double dt, double *out_7968249393634225210);
void car_F_fun(double *state, double dt, double *out_933118493395579449);
void car_h_25(double *state, double *unused, double *out_1524326060687862043);
void car_H_25(double *state, double *unused, double *out_6350445648897349152);
void car_h_24(double *state, double *unused, double *out_5671295414922481183);
void car_H_24(double *state, double *unused, double *out_2868233238743007239);
void car_h_30(double *state, double *unused, double *out_4642787979283272260);
void car_H_30(double *state, double *unused, double *out_1822749318769740954);
void car_h_26(double *state, double *unused, double *out_8566727210287675458);
void car_H_26(double *state, double *unused, double *out_2608942330023292928);
void car_h_27(double *state, double *unused, double *out_6608429876303459109);
void car_H_27(double *state, double *unused, double *out_352013993030683957);
void car_h_29(double *state, double *unused, double *out_8059061268932342276);
void car_H_29(double *state, double *unused, double *out_2332980663084133138);
void car_h_28(double *state, double *unused, double *out_965700005222502274);
void car_H_28(double *state, double *unused, double *out_4296610934649459389);
void car_h_31(double *state, double *unused, double *out_2974957453316745210);
void car_H_31(double *state, double *unused, double *out_1982734227789941452);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}