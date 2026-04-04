import csv
import sys

f = 'C:/Users/user/Desktop/Turns/unwind_1726.csv'
print(f"--- {f} ---")
with open(f, 'r') as csvfile:
    reader = csv.DictReader(csvfile)
    rows = list(reader)
    
    start_idx = next((i for i, r in enumerate(rows) if int(r['unwind_detected']) == 1), -1)
    if start_idx != -1:
        print(f"Unwind Start: Steer={float(rows[start_idx]['steering_angle_deg']):.1f}, Speed={rows[start_idx]['speed_mph']}")
        for r in rows[start_idx:start_idx+15]:
            print(f"  T={r['time_s']} Steer={float(r['steering_angle_deg']):>6.1f} Trq={float(r['torque_request']):>6.3f} ManTrq={float(r['steering_torque']):>6.1f} Unw={r['unwind_detected']}")
