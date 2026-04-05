import csv
import sys
import numpy as np

f = 'C:/Users/user/Desktop/Turns/unwind_578.csv'
with open(f, 'r') as csvfile:
    reader = csv.DictReader(csvfile)
    rows = list(reader)
    
    start_idx = next((i for i, r in enumerate(rows) if int(r['unwind_detected']) == 1), -1)
    if start_idx != -1:
        print("Max Trq later in unwind:")
        max_trq = 0.0
        max_trq_row = None
        for i, r in enumerate(rows[start_idx:]):
            if abs(float(r['torque_request'])) > abs(max_trq):
                max_trq = float(r['torque_request'])
                max_trq_row = r
                
        print(max_trq_row)
