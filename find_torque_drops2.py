import csv
import glob
import os

files = glob.glob(r"C:\Users\user\Desktop\Turns\unwind_*.csv")
files.sort(key=os.path.getmtime, reverse=True)
recent_files = files[:10]

for fpath in recent_files:
    with open(fpath, 'r') as f:
        reader = list(csv.DictReader(f))
        
        # We are looking for mid-turn torque drops.
        # So we only consider times when angle > 40 degrees.
        for i in range(10, len(reader)):
            r = reader[i]
            angle = abs(float(r['steering_angle_deg']))
            if angle > 40:
                # Check for a drop over 10 frames (0.1 seconds)
                curr_trq = abs(float(r['torque_request']))
                past_trq = abs(float(reader[i-10]['torque_request']))
                
                # if torque drops by more than 1.0
                if past_trq - curr_trq > 1.0:
                    print(f"File {os.path.basename(fpath)} at {r['time_s']}: Drop {past_trq:.2f} -> {curr_trq:.2f} (Angle {angle:.1f})")
                    break
