import csv
import glob
import os

files = glob.glob(r"C:\Users\user\Desktop\Turns\unwind_*.csv")
files.sort(key=os.path.getmtime, reverse=True)
recent_files = files[:5]

for fpath in recent_files:
    print(f"\n--- Analyzing {os.path.basename(fpath)} ---")
    with open(fpath, 'r') as f:
        reader = csv.DictReader(f)
        last_unwind = '0'
        for i, r in enumerate(reader):
            unwind = r['unwind_detected']
            angle = float(r['steering_angle_deg'])
            torque = float(r['torque_request'])
            if unwind == '1' and last_unwind == '0' and abs(angle) > 20:
                print(f"Time {float(r['time_s']):.2f}: Unwind triggered! Angle: {angle:.1f}, Torque: {torque:.4f}")
                # print the next few lines
            if unwind == '1' and abs(angle) > 20 and i % 50 == 0:
                # just sample
                pass
            last_unwind = unwind
