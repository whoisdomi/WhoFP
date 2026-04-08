import sys

with open(r'C:\Users\user\Documents\GitHub\openpilot\selfdrive\controls\lib\latcontrol_torque.py', 'r') as f:
    content = f.read()

# Fix 1: Add steering rate requirement to unwind condition
old_unwind = """    # Unwind condition: angle decreasing from a real turn (>5 deg peak), same sign
    unwind_condition = (self.peak_steering_angle > 5.0 and
                        abs_steer < abs(self.unwind_last_angle) and
                        (np.sign(steering_angle) == np.sign(self.unwind_last_angle) if self.unwind_last_angle != 0 else False))"""

new_unwind = """    # Unwind condition: angle decreasing from a real turn (>5 deg peak), same sign
    unwind_condition = (self.peak_steering_angle > 5.0 and
                        self.smoothed_steering_rate > 15.0 and
                        (np.sign(steering_angle) == np.sign(self.unwind_last_angle) if self.unwind_last_angle != 0 else False))"""

content = content.replace(old_unwind, new_unwind)

# Fix 2: Lower the speed limit for freezing the integrator
old_freeze = """      freeze_integrator = steer_limited_by_controls or CS.steeringPressed or CS.vEgo < 1.5"""
new_freeze = """      freeze_integrator = steer_limited_by_controls or CS.steeringPressed or CS.vEgo < 0.3"""

content = content.replace(old_freeze, new_freeze)

with open(r'C:\Users\user\Documents\GitHub\openpilot\selfdrive\controls\lib\latcontrol_torque.py', 'w') as f:
    f.write(content)

print("Modifications applied successfully.")
