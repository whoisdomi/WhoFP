import csv
import glob
import os

files = glob.glob(r"C:\Users\user\Desktop\Turns\unwind_*.csv")
files.sort(key=os.path.getmtime, reverse=True)

for fpath in files:
    with open(fpath, 'r') as f:
        reader = list(csv.DictReader(f))
        
        for i in range(10, len(reader) - 20):
            r = reader[i]
            angle = abs(float(r['steering_angle_deg']))
            if angle > 40:
                past_trq = abs(float(reader[i-10]['torque_request']))
                curr_trq = abs(float(r['torque_request']))
                future_trq = abs(float(reader[i+20]['torque_request']))
                
                # drop then resume
                if past_trq > 0.8 and curr_trq < 0.3 and future_trq > 0.8:
                    print(f"File {os.path.basename(fpath)} at {r['time_s']}: Trq drops {past_trq:.2f} -> {curr_trq:.2f} -> {future_trq:.2f} (Angle {angle:.1f})")
                    break
