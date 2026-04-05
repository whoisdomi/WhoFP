import csv
import sys
import numpy as np

f = 'C:/Users/user/Desktop/Turns/unwind_1472.csv'
print(f"--- {f} ---")
with open(f, 'r') as csvfile:
    reader = csv.DictReader(csvfile)
    rows = list(reader)
    
    start_idx = next((i for i, r in enumerate(rows) if int(r['unwind_detected']) == 1), -1)
    if start_idx != -1:
        for i, r in enumerate(rows[start_idx:]):
            if int(r['unwind_detected']) == 0:
                print(f"Unwind cancelled at row {start_idx+i}: Time={r['time_s']}, Steer={r['steering_angle_deg']}")
                for r2 in rows[start_idx+i-10:start_idx+i+5]:
                    print(f"  T={r2['time_s']} Steer={float(r2['steering_angle_deg']):>6.1f} Trq={float(r2['torque_request']):>6.3f} Err={float(r2['error']):>6.3f} ManTrq={float(r2['steering_torque']):>6.1f} Unw={r2['unwind_detected']}")
                break
