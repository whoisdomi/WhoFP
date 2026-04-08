import csv
import glob
import os

files = glob.glob(r"C:\Users\user\Desktop\Turns\unwind_*.csv")
files.sort(key=os.path.getmtime, reverse=True)

# Look at the most recent file
for fpath in files[:2]:
    print(f"\n--- Analyzing {os.path.basename(fpath)} ---")
    with open(fpath, 'r') as f:
        reader = list(csv.DictReader(f))
        
        last_unwind = '0'
        for i in range(1, len(reader)):
            r = reader[i]
            prev_r = reader[i-1]
            
            unwind = r['unwind_detected']
            angle = abs(float(r['steering_angle_deg']))
            torque = abs(float(r['torque_request']))
            prev_torque = abs(float(prev_r['torque_request']))
            
            # Print if torque drops drastically while angle is large
            if angle > 40 and prev_torque > 0.8 and torque < 0.2:
                print(f"Time {float(r['time_s']):.2f}: Torque Drop! Angle: {angle:.1f}, Torque: {prev_torque:.2f} -> {torque:.2f}")
                # Print context
                for j in range(max(0, i-5), min(len(reader), i+5)):
                    cr = reader[j]
                    print(f"  {cr['time_s']} ang:{cr['steering_angle_deg']} spd:{cr['speed_mph']} trq:{cr['torque_request']} curv:{cr['desired_curvature']} unw:{cr['unwind_detected']}")
                break
