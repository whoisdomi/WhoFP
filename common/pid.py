import numpy as np
from numbers import Number

class PIDController:
  def __init__(self, k_p, k_i, k_f=0., k_d=0., pos_limit=1e308, neg_limit=-1e308, rate=100, unwind_multiplier=1.0, ki_deadband=0.0):
    self._k_p = k_p
    self._k_i = k_i
    self._k_d = k_d
    self.k_f = k_f   # feedforward gain
    self.unwind_multiplier = unwind_multiplier  # decay factor for integrator when unwinding (0.95 = 5% decay per cycle)
    self.ki_deadband = ki_deadband  # error threshold below which integrator stops accumulating (anti-windup)
    if isinstance(self._k_p, Number):
      self._k_p = [[0], [self._k_p]]
    if isinstance(self._k_i, Number):
      self._k_i = [[0], [self._k_i]]
    if isinstance(self._k_d, Number):
      self._k_d = [[0], [self._k_d]]

    self.pos_limit = pos_limit
    self.neg_limit = neg_limit

    self.i_unwind_rate = 0.3 / rate
    self.i_rate = 1.0 / rate
    self.speed = 0.0

    self.reset()

  @property
  def k_p(self):
    return np.interp(self.speed, self._k_p[0], self._k_p[1])

  @property
  def k_i(self):
    return np.interp(self.speed, self._k_i[0], self._k_i[1])

  @property
  def k_d(self):
    return np.interp(self.speed, self._k_d[0], self._k_d[1])

  @property
  def error_integral(self):
    return self.i/self.k_i

  def reset(self):
    self.p = 0.0
    self.i = 0.0
    self.d = 0.0
    self.f = 0.0
    self.control = 0

  def update(self, error, error_rate=0.0, speed=0.0, override=False, feedforward=0., freeze_integrator=False):
    self.speed = speed

    self.p = float(error) * self.k_p
    self.f = feedforward * self.k_f
    self.d = error_rate * self.k_d

    if override:
      self.i -= self.i_unwind_rate * float(np.sign(self.i))
    else:
      if not freeze_integrator:
        # Anti-windup: don't accumulate integrator for errors below deadband threshold
        # Prevents slow integrator drift from tiny noise-driven errors (highway oscillation)
        # P and FF still handle centering during these moments
        if self.ki_deadband > 0 and abs(error) < self.ki_deadband:
          pass  # Skip integrator accumulation
        else:
          self.i = self.i + error * self.k_i * self.i_rate

        # Decay integrator when unwinding (error opposes integrator)
        if self.unwind_multiplier < 1.0 and np.sign(error) != np.sign(self.i) and abs(self.i) > 0.01:
          self.i *= self.unwind_multiplier

        # Clip i to prevent exceeding control limits
        control_no_i = self.p + self.d + self.f
        control_no_i = np.clip(control_no_i, self.neg_limit, self.pos_limit)
        self.i = np.clip(self.i, self.neg_limit - control_no_i, self.pos_limit - control_no_i)

    control = self.p + self.i + self.d + self.f

    self.control = np.clip(control, self.neg_limit, self.pos_limit)
    return self.control
