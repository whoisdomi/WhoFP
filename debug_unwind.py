import csv
import sys
import numpy as np

f = 'C:/Users/user/Desktop/Turns/unwind_10002.csv'
print(f"--- {f} ---")
with open(f, 'r') as csvfile:
    reader = csv.DictReader(csvfile)
    rows = list(reader)
    
    start_idx = next((i for i, r in enumerate(rows) if int(r['unwind_detected']) == 1), -1)
    if start_idx != -1:
        print(f"Unwind Start: Steer={float(rows[start_idx]['steering_angle_deg']):.1f}")
        # Settle point
        settle_idx = -1
        for i, r in enumerate(rows[start_idx:]):
            if abs(float(r['steering_angle_deg'])) < 2.0:
                settle_idx = start_idx + i
                print(f"Settle at row {settle_idx}: Time={r['time_s']}, Steer={r['steering_angle_deg']}")
                break
        
        if settle_idx == -1:
            print("NEVER SETTLED in log.")
            # check the end of the log
            for r in rows[-5:]:
                print(f"  T={r['time_s']} Steer={float(r['steering_angle_deg']):>6.1f} Trq={float(r['torque_request']):>6.3f}")
