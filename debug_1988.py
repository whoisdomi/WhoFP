import csv
import sys

f = 'C:/Users/user/Desktop/Turns/unwind_1988.csv'
print(f"--- {f} ---")
with open(f, 'r') as csvfile:
    reader = csv.DictReader(csvfile)
    rows = list(reader)
    
    start_idx = next((i for i, r in enumerate(rows) if int(r['unwind_detected']) == 1), -1)
    if start_idx != -1:
        overshoot_start = -1
        for i, r in enumerate(rows[start_idx:]):
            if abs(float(r['steering_angle_deg'])) < 2.0:
                overshoot_start = start_idx + i
                break
        if overshoot_start != -1:
            for r in rows[overshoot_start-10:overshoot_start+15]:
                print(f"  T={r['time_s']} Steer={float(r['steering_angle_deg']):>6.1f} Trq={float(r['torque_request']):>6.3f} Err={float(r['error']):>6.3f} DesCurv={float(r['desired_curvature']):>8.5f} Unw={r['unwind_detected']}")
