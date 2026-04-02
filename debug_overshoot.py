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
        init_sign = np.sign(float(rows[start_idx]['steering_angle_deg']))
        print(f"Unwind Start: Steer={float(rows[start_idx]['steering_angle_deg']):.1f}, Speed={rows[start_idx]['speed_mph']}")
        
        # Check for sign change
        for i, r in enumerate(rows[start_idx:]):
            steer = float(r['steering_angle_deg'])
            if np.sign(steer) == -init_sign and abs(steer) > 2.0:
                print(f"Sign Change at row {start_idx+i}: Steer={steer:.1f}, Time={r['time_s']}")
                # print some context
                for r2 in rows[start_idx+i-5:start_idx+i+15]:
                    print(f"  T={r2['time_s']} Steer={float(r2['steering_angle_deg']):>6.1f} Trq={float(r2['torque_request']):>6.3f} Err={float(r2['error']):>6.3f}")
                break
