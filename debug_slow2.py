import csv
import sys
import numpy as np

for f in ['C:/Users/user/Desktop/Turns/unwind_490.csv', 'C:/Users/user/Desktop/Turns/unwind_471.csv']:
    print(f"--- {f} ---")
    with open(f, 'r') as csvfile:
        reader = csv.DictReader(csvfile)
        rows = list(reader)
        
        start_idx = next((i for i, r in enumerate(rows) if int(r['unwind_detected']) == 1), -1)
        if start_idx != -1:
            print(f"Unwind Start: Steer={float(rows[start_idx]['steering_angle_deg']):.1f}, Speed={rows[start_idx]['speed_mph']}")
            for r in rows[start_idx:start_idx+20]:
                print(f"  T={r['time_s']} Steer={float(r['steering_angle_deg']):>6.1f} Trq={float(r['torque_request']):>6.3f} Err={float(r['error']):>6.3f} ManTrq={float(r['steering_torque']):>6.1f} Unw={r['unwind_detected']}")
            
            print("...")
            
            # Find max torque applied
            max_trq = 0.0
            max_row = None
            for r in rows[start_idx:]:
                if abs(float(r['torque_request'])) > abs(max_trq):
                    max_trq = float(r['torque_request'])
                    max_row = r
            if max_row:
                print(f"Max Trq Request: {max_trq} at T={max_row['time_s']}, Steer={float(max_row['steering_angle_deg']):.1f}, Err={float(max_row['error']):.3f}")
