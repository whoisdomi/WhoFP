import csv
import sys
import numpy as np
from pathlib import Path
from datetime import datetime

csv_dir = Path('C:/Users/user/Desktop/Turns')
csv_files = sorted([f for f in csv_dir.glob('*.csv')], key=lambda p: p.stat().st_mtime)

print(f"{'Filename':15} | {'Spd':4} | {'PeakA':5} | {'OvrSht':6} | {'Time':5} | {'Rate':5} | {'Resist':6} | {'Status'}")
print("-" * 90)

for f in csv_files:
    mtime = datetime.fromtimestamp(f.stat().st_mtime)
    # Only 4/2
    if mtime.year != 2026 or mtime.month != 4 or mtime.day != 2:
        continue

    try:
        with open(f, 'r') as csvfile:
            reader = csv.DictReader(csvfile)
            rows = list(reader)
            if not rows: continue
            
            # Find the peak angle and start of unwind
            angles = [abs(float(r['steering_angle_deg'])) for r in rows]
            peak_angle = max(angles)
            peak_idx = angles.index(peak_angle)
            
            # Use unwind_detected trigger
            start_idx = next((i for i, r in enumerate(rows) if int(r['unwind_detected']) == 1), -1)
            if start_idx == -1: continue
            
            init_sign = np.sign(float(rows[start_idx]['steering_angle_deg']))
            
            # Define "unwind period" as until it first crosses < 2 degrees
            # OR until 5 seconds have passed
            unwind_window = []
            settle_idx_in_window = -1
            for i, r in enumerate(rows[start_idx:]):
                unwind_window.append(r)
                if abs(float(r['steering_angle_deg'])) < 2.0:
                    settle_idx_in_window = i
                    break
                if i > 500: # 5 seconds max
                    break
            
            if not unwind_window: continue
            
            v_ego_mph = float(rows[start_idx]['speed_mph'])
            start_angle = abs(float(rows[start_idx]['steering_angle_deg']))
            
            # Max overshoot in the 2 seconds AFTER settle
            overshoot_val = 0.0
            if settle_idx_in_window != -1:
                settle_global_idx = start_idx + settle_idx_in_window
                post_settle = rows[settle_global_idx : settle_global_idx + 200] # 2 seconds
                overshoots = [abs(float(r['steering_angle_deg'])) for r in post_settle if np.sign(float(r['steering_angle_deg'])) == -init_sign]
                overshoot_val = max(overshoots) if overshoots else 0.0
            
            # Unwind time and rate
            unwind_time = 0.0
            unwind_rate = 0.0
            if settle_idx_in_window != -1:
                unwind_time = float(unwind_window[settle_idx_in_window]['time_s']) - float(unwind_window[0]['time_s'])
                if unwind_time > 0:
                    unwind_rate = (start_angle - 2.0) / unwind_time
            
            # Resistance
            resistances = []
            for r in unwind_window:
                trq = float(r['torque_request'])
                res = trq * init_sign 
                resistances.append(res)
            avg_resist = np.mean(resistances) if resistances else 0.0
            
            status = "OK"
            if overshoot_val > 5.0: status = "OVERSHOOT"
            elif unwind_rate < 40.0 and start_angle > 30: status = "SLOW"
            
            print(f"{f.name:15} | {v_ego_mph:4.1f} | {start_angle:5.1f} | {overshoot_val:6.1f} | {unwind_time:5.2f} | {unwind_rate:5.1f} | {avg_resist:6.2f} | {status}")
            
    except Exception as e:
        pass
