import csv
import glob
import os

files = glob.glob(r"C:\Users\user\Desktop\Turns\unwind_*.csv")
files.sort(key=os.path.getmtime, reverse=True)

for fpath in files[:10]:
    with open(fpath, 'r') as f:
        reader = list(csv.DictReader(f))
        
        trqs = [abs(float(r['torque_request'])) for r in reader]
        max_trq = max(trqs) if trqs else 0
        max_ang = max([abs(float(r['steering_angle_deg'])) for r in reader]) if reader else 0
        unwinds = sum(1 for r in reader if r['unwind_detected'] == '1')
        print(f"{os.path.basename(fpath):20} | MaxTrq: {max_trq:5.2f} | MaxAng: {max_ang:5.1f} | UnwindFrames: {unwinds}")
