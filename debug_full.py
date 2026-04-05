import csv
import sys
import numpy as np

f = 'C:/Users/user/Desktop/Turns/unwind_1472.csv'
print(f"--- {f} ---")
with open(f, 'r') as csvfile:
    reader = csv.DictReader(csvfile)
    rows = list(reader)
    
    for r in rows:
        if 1476.2 < float(r['time_s']) < 1477.4:
            print(f"T={r['time_s']} Steer={float(r['steering_angle_deg']):>6.1f} TrqReq={r['torque_request']} Unw={r['unwind_detected']} ManTrq={r['steering_torque']}")
