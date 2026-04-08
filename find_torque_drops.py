import csv
import glob
import os

files = glob.glob(r"C:\Users\user\Desktop\Turns\unwind_*.csv")
files.sort(key=os.path.getmtime, reverse=True)
recent_files = files[:5]

for fpath in recent_files:
    print(f"\n--- Analyzing {os.path.basename(fpath)} ---")
    with open(fpath, 'r') as f:
        reader = list(csv.DictReader(f))
        
        for i in range(1, len(reader)):
            r = reader[i]
            prev_r = reader[i-1]
            
            angle = abs(float(r['steering_angle_deg']))
            torque = abs(float(r['torque_request']))
            prev_torque = abs(float(prev_r['torque_request']))
            
            # If we are deep in a turn and torque suddenly drops by a lot
            if angle > 40 and prev_torque > 1.0 and torque < 0.2:
                print(f"Time {float(r['time_s']):.2f}: Sudden drop! Angle: {angle:.1f}, Torque: {prev_torque:.2f} -> {torque:.2f}")
                # Print context
                for j in range(max(0, i-2), min(len(reader), i+3)):
                    cr = reader[j]
                    print(f"  {cr['time_s']} ang:{cr['steering_angle_deg']} spd:{cr['speed_mph']} trq:{cr['torque_request']} curv:{cr['desired_curvature']} setp:{cr['setpoint']} meas:{cr['measurement']} unw:{cr['unwind_detected']}")
                break # Just show the first per file
