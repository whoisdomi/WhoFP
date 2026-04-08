import csv
with open(r'C:\Users\user\Desktop\Turns\unwind_2963.csv', 'r') as f:
    reader = list(csv.DictReader(f))
    for i, r in enumerate(reader):
        unw = r['unwind_detected']
        angle = abs(float(r['steering_angle_deg']))
        trq = abs(float(r['torque_request']))
        
        if i % 10 == 0 or (i > 0 and unw != reader[i-1]['unwind_detected']):
            print(f"{r['time_s']} | Ang: {r['steering_angle_deg']} | Trq: {r['torque_request']} | Unw: {unw}")
