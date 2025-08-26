THIS IS A TESTING ENVIORNMENT AND MIGHT GET YOU BANNED IF YOU UPLOAD TO COMMA. I HAVE NO IDEA. UPLOAD TO KONIK.

This is my STABLEish fork of StarPilot (Dom branch), which is a fork of FrogPilot Master, which is a fork of OPGM, which is a fork of Comma OpenPilot.

THIS IS AN IONIQ 6 TUNING SPECIFIC HKG BRANCH.

Notible Fork Changes:

FrogPilot Master
+ StarPilot Model Switcher
+ Ioniq 6 Tuning Values
+ HKG Damp_Factor
+ AutoUpdate off
+ HKG Flag fix

Tuning Changes:
Max Steer: 610 //Max available torque, this is very car specific, I only know how much torque I have because someone used Plotjuggler or Cavana to look at requested torque. Even then, there might be differences by spec or year of car depending on EPS motor used

Steer Up: 5 //Rate at which torque can increase. 3 is safe but less aggressive, 5 has a tiny bit of overshoot.

Steer Down: 10 //Rate at which torque can decrease. A higher rate will allow OP to "let go" so that the steering wheel spins back to center after a turn

max_rt_delta: 90 //Master rate change per frame, it will start clipping torque. Having this lower number really smoothed out steering and prevented overshoot when going back to center

Damp_Factor: 100 //Dampens steering wheel, making it firmer and less suseptible to wobble. 3-200 values are the range. Comma stock value is 100 and from reading Discord testing is the best value. Having lower or higher damp is not necessarily proportional, meaning lower doesn't mean less dampening, and higher doesn't mean firmer, and both can introduce unwanted things.

HKG Flag fix: Added flags to values.py so that Ioniq 6 uses 32bit address in DBC and not 16bit address. From testing it didn't change anything.
https://claude.ai/public/artifacts/18e1941a-9d7c-4546-89fc-ec3c231bfa51


