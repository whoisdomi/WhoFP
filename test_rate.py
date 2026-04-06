import csv
import sys
import numpy as np

f = 'C:/Users/user/Desktop/Turns/unwind_2727.csv'
print(f"--- {f} ---")
with open(f, 'r') as csvfile:
    reader = csv.DictReader(csvfile)
    rows = list(reader)
    
    # manual turn, so we just trace the whole thing
    smoothed_rate = 0.0
    last_angle = float(rows[0]['steering_angle_deg'])
    
    # Find peak
    angles = [abs(float(r['steering_angle_deg'])) for r in rows]
    peak_idx = angles.index(max(angles))
    
    for r in rows[peak_idx-10:peak_idx+50]:
        steer = float(r['steering_angle_deg'])
        dt = 0.01
        
        rate = (abs(last_angle) - abs(steer)) / dt
        smoothed_rate += 0.2 * (rate - smoothed_rate)
        
        print(f"T={r['time_s']} Steer={steer:>6.1f} Rate={rate:>6.1f} SmRate={smoothed_rate:>6.1f} Trq={float(r['torque_request']):>6.3f} Man={float(r['steering_torque']):>6.1f}")
        
        last_angle = steer
