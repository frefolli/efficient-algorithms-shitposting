from __future__ import annotations
from ed import ED
import numpy

class DPED(ED):
  DIR_NOP = 0
  DIR_TOP = 1
  DIR_LEFT = 2
  DIR_TOP_LEFT = 3

  def __init__(self, S: str, T: str) -> None:
    self.S, self.T = S, T
    self.n, self.m = len(S), len(T)
    self.C = numpy.zeros(shape=(self.n + 1, self.m + 1), dtype=numpy.uint32)
    self.D = numpy.zeros(shape=(self.n + 1, self.m + 1), dtype=numpy.uint8)
    for i in range(1, self.n + 1):
      self.C[i, 0] = i
      self.D[i, 0] = self.DIR_TOP
    for j in range(1, self.m + 1):
      self.C[0, j] = j
      self.D[0, j] = self.DIR_LEFT
    self.C[0, 0] = 0
    self.D[0, 0] = self.DIR_NOP

  def execute(self) -> DPED:
    for i in range(1, self.n + 1):
      for j in range(1, self.m + 1):
        if self.S[i - 1] == self.T[j - 1]:
          self.C[i, j] = self.C[i - 1, j - 1]
          self.D[i, j] = self.DIR_TOP_LEFT
        else:
          choices = []
          directions = []

          choices.append(self.C[i - 1, j])
          directions.append(self.DIR_TOP)

          choices.append(self.C[i, j - 1])
          directions.append(self.DIR_LEFT)
          
          choices.append(self.C[i - 1, j - 1])
          directions.append(self.DIR_TOP_LEFT)

          ti = numpy.argmin(choices)
          t = choices[ti]
          d = directions[ti]
          self.C[i, j] = t + 1
          self.D[i, j] = d
    print('BEGIN DP')
    print(self.C)
    print(self.D)
    print('END   DP')
    return self

  def digest(self) -> str:
    result = ""
    i, j = self.n, self.m
    while i >= 0 and j >= 0 and self.D[i, j] != self.DIR_NOP:
      if self.D[i, j] == self.DIR_TOP_LEFT:
        if self.S[i - 1] == self.T[j - 1]:
          result += self.S[i - 1]
        i, j = i - 1, j - 1
      elif self.D[i, j] == self.DIR_TOP:
        i = i - 1
      elif self.D[i, j] == self.DIR_LEFT:
        j = j - 1
    if i < 0 or j < 0:
      raise RuntimeError("It shouldnt be possible to reach i=%d j=%d" % (i, j))
    return result[::-1]

  @staticmethod
  def run(S: str, T: str) -> str:
    return DPED(S, T).execute().digest()
