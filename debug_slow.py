import csv
import sys
import numpy as np

f = 'C:/Users/user/Desktop/Turns/unwind_264.csv'
print(f"--- {f} ---")
with open(f, 'r') as csvfile:
    reader = csv.DictReader(csvfile)
    rows = list(reader)
    
    start_idx = next((i for i, r in enumerate(rows) if int(r['unwind_detected']) == 1), -1)
    if start_idx != -1:
        print(f"Unwind Start: Steer={float(rows[start_idx]['steering_angle_deg']):.1f}, Speed={rows[start_idx]['speed_mph']}")
        for r in rows[start_idx:start_idx+20]:
            print(f"  T={r['time_s']} Steer={float(r['steering_angle_deg']):>6.1f} Trq={float(r['torque_request']):>6.3f} Err={float(r['error']):>6.3f} DesCurv={float(r['desired_curvature']):>8.5f}")
        print("...")
        for r in rows[-10:]:
            print(f"  T={r['time_s']} Steer={float(r['steering_angle_deg']):>6.1f} Trq={float(r['torque_request']):>6.3f} Err={float(r['error']):>6.3f} DesCurv={float(r['desired_curvature']):>8.5f}")
