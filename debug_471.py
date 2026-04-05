import csv
import sys
import numpy as np

f = 'C:/Users/user/Desktop/Turns/unwind_471.csv'
print(f"--- {f} ---")
with open(f, 'r') as csvfile:
    reader = csv.DictReader(csvfile)
    rows = list(reader)
    
    # We want to see the actual unwind that was tracked for overshoot
    # deep_analyze uses the FIRST time unwind_detected == 1.
    start_idx = next((i for i, r in enumerate(rows) if int(r['unwind_detected']) == 1), -1)
    if start_idx != -1:
        # find where it actually settled
        settle_idx = -1
        for i, r in enumerate(rows[start_idx:]):
            if abs(float(r['steering_angle_deg'])) < 2.0:
                settle_idx = start_idx + i
                break
        
        if settle_idx != -1:
            print(f"Settle at T={rows[settle_idx]['time_s']} Steer={rows[settle_idx]['steering_angle_deg']}")
            for r in rows[settle_idx-10:settle_idx+20]:
                print(f"  T={r['time_s']} Steer={float(r['steering_angle_deg']):>6.1f} Trq={float(r['torque_request']):>6.3f} ManTrq={float(r['steering_torque']):>6.1f} Unw={r['unwind_detected']}")
