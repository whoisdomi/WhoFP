import csv
with open(r'C:\Users\user\Desktop\Turns\unwind_1422.csv', 'r') as f:
    reader = csv.DictReader(f)
    print(f"{'time':>8} {'angle':>8} {'speed':>6} {'req_torq':>10} {'pid_p':>8} {'pid_i':>8} {'setpoint':>10} {'measure':>10} {'unwind':>8} {'damp_bst':>10}")
    for i, r in enumerate(reader):
        if i % 5 == 0:
            print(f"{float(r['time_s']):8.2f} {float(r['steering_angle_deg']):8.1f} {float(r['speed_mph']):6.1f} {float(r['torque_request']):10.4f} {float(r['pid_p']):8.4f} {float(r['pid_i']):8.4f} {float(r['setpoint']):10.4f} {float(r['measurement']):10.4f} {r['unwind_detected']:>8} {r['damp_boost_active']:>10}")
