import csv
with open(r'C:\Users\user\Desktop\Turns\unwind_3021.csv', 'r') as f:
    reader = list(csv.DictReader(f))
    for r in reader:
        t = float(r['time_s'])
        if 3021.0 < t < 3022.5:
            err = float(r['error'])
            print(f"{t:.2f} | ang:{r['steering_angle_deg']} | trq:{r['torque_request']} | unw:{r['unwind_detected']} | err:{err:.4f} | p:{r['pid_p']} | i:{r['pid_i']} | f:{r['pid_f']}")
