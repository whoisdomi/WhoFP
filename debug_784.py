import csv
import sys
import numpy as np

f = 'C:/Users/user/Desktop/Turns/unwind_784.csv'
print(f"--- {f} ---")
with open(f, 'r') as csvfile:
    reader = csv.DictReader(csvfile)
    rows = list(reader)
    
    start_idx = next((i for i, r in enumerate(rows) if int(r['unwind_detected']) == 1), -1)
    if start_idx != -1:
        print(f"Unwind Start: Steer={float(rows[start_idx]['steering_angle_deg']):.1f}, Speed={rows[start_idx]['speed_mph']}")
        for r in rows[start_idx:start_idx+15]:
            print(f"  T={r['time_s']} Steer={float(r['steering_angle_deg']):>6.1f} Trq={float(r['torque_request']):>6.3f} Err={float(r['error']):>6.3f} ManTrq={float(r['steering_torque']):>6.1f} Damp={r['damp_factor']} Unw={r['unwind_detected']}")
            
        print("...")
        # Print where it gets stuck or max manual torque
        max_trq_row = None
        max_trq = 0
        for r in rows[start_idx:]:
            if abs(float(r['steering_torque'])) > max_trq:
                max_trq = abs(float(r['steering_torque']))
                max_trq_row = r
                
        if max_trq_row:
            print(f"Max Man Trq at T={max_trq_row['time_s']}, Steer={float(max_trq_row['steering_angle_deg']):.1f}, OP_Trq={float(max_trq_row['torque_request']):.3f}, Damp={max_trq_row['damp_factor']}")
        
        # check end of log
        print("End of log:")
        for r in rows[-5:]:
            print(f"  T={r['time_s']} Steer={float(r['steering_angle_deg']):>6.1f} Trq={float(r['torque_request']):>6.3f} Err={float(r['error']):>6.3f} ManTrq={float(r['steering_torque']):>6.1f} Damp={r['damp_factor']} Unw={r['unwind_detected']}")
