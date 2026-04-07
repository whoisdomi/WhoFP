import os
import csv
import glob
from datetime import datetime

directory = r"C:\Users\user\Desktop\Turns"
csv_files = glob.glob(os.path.join(directory, "*.csv"))

results = []

for file in csv_files:
    mtime = os.path.getmtime(file)
    date = datetime.fromtimestamp(mtime)
    if date.month == 4 and date.day == 7 and date.year == 2026:
        with open(file, 'r') as f:
            reader = csv.DictReader(f)
            data = list(reader)
            if not data: continue
            
            angles = [float(row['steering_angle_deg']) for row in data]
            speeds = [float(row['speed_mph']) for row in data]
            
            max_angle = max(angles)
            min_angle = min(angles)
            
            turn_is_positive = abs(max_angle) > abs(min_angle)
            peak_angle = max_angle if turn_is_positive else min_angle
            
            crossed_zero = False
            overshoot = 0
            overshoot_torques = []
            
            for row in data:
                angle = float(row['steering_angle_deg'])
                torque = float(row.get('steering_torque', 0))
                req_torque = float(row.get('torque_request', 0))
                
                if not crossed_zero:
                    if turn_is_positive and angle < 0:
                        crossed_zero = True
                    elif not turn_is_positive and angle > 0:
                        crossed_zero = True
                        
                if crossed_zero:
                    if turn_is_positive:
                        overshoot = min(overshoot, angle)
                    else:
                        overshoot = max(overshoot, angle)
                    overshoot_torques.append(abs(req_torque))
            
            avg_speed = sum(speeds)/len(speeds)
            max_overshoot_torque = max(overshoot_torques) if overshoot_torques else 0
            
            results.append({
                'file': os.path.basename(file),
                'peak_angle': peak_angle,
                'overshoot': overshoot,
                'avg_speed': avg_speed,
                'max_overshoot_torque': max_overshoot_torque
            })

results.sort(key=lambda x: abs(x['overshoot']), reverse=True)

print(f"{'File':<20} | {'Peak Angle':<12} | {'Overshoot':<12} | {'Speed':<8} | {'Max Req Torque (Overshoot)':<25}")
print("-" * 85)
for r in results:
    print(f"{r['file']:<20} | {r['peak_angle']:<12.2f} | {r['overshoot']:<12.2f} | {r['avg_speed']:<8.1f} | {r['max_overshoot_torque']:<25.4f}")
