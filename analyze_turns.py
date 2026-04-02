import csv
import sys
import numpy as np

with open('C:/Users/user/Desktop/Turns/unwind_2924.csv', 'r') as csvfile:
    reader = csv.DictReader(csvfile)
    rows = list(reader)
    
    start_idx = next((i for i, r in enumerate(rows) if int(r['damp_boost_active']) == 1), -1)
    if start_idx != -1:
        print("--- unwind_2924.csv Unwind sequence (before fix) ---")
        for r in rows[start_idx-5:start_idx+20]:
            print(f"Steer: {float(r['steering_angle_deg']):>6.1f} | TrqReq: {float(r['torque_request']):>6.2f} | DesCurv: {float(r['desired_curvature']):>8.5f} | Err: {float(r['error']):>7.4f} | Damp: {r['damp_factor']}")
