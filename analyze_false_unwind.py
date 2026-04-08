import csv
import glob
import os

files = glob.glob(r"C:\Users\user\Desktop\Turns\unwind_*.csv")
files.sort(key=os.path.getmtime, reverse=True)

# Look at the most recent files
for fpath in files[:10]:
    with open(fpath, 'r') as f:
        reader = list(csv.DictReader(f))
        
        for i in range(1, len(reader)):
            r = reader[i]
            
            unwind = r['unwind_detected']
            angle = float(r['steering_angle_deg'])
            
            # Left turn means positive angle. If unwind triggers mid-turn (angle > 40)
            if unwind == '1' and angle > 40:
                print(f"File {os.path.basename(fpath)} Time {float(r['time_s']):.2f}: False Unwind! Angle: {angle:.1f}, Torque: {r['torque_request']}")
                # Print context
                for j in range(max(0, i-2), min(len(reader), i+3)):
                    cr = reader[j]
                    print(f"  {cr['time_s']} ang:{cr['steering_angle_deg']} spd:{cr['speed_mph']} trq:{cr['torque_request']} curv:{cr['desired_curvature']} unw:{cr['unwind_detected']}")
                break
