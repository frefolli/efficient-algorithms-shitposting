from __future__ import annotations
from ed import ED
import numpy

class DPED(ED):
  DIR_NOP = 0
  DIR_TOP = 1
  DIR_LEFT = 2
  DIR_TOP_LEFT = 3

  DIRMODE=True
  def dir_to_str(self, dir: numpy.uint8) -> str:
    if self.DIRMODE:
      if dir == self.DIR_NOP:
        return "·"
      elif dir == self.DIR_TOP:
        return "↑"
      elif dir == self.DIR_LEFT:
        return "←"
      elif dir == self.DIR_TOP_LEFT:
        return "↖"
      raise ValueError(dir)
    else:
      if dir == self.DIR_NOP:
        return "·"
      elif dir == self.DIR_TOP:
        return "↗"
      elif dir == self.DIR_LEFT:
        return "↖"
      elif dir == self.DIR_TOP_LEFT:
        return "↑"
      raise ValueError(dir)

  def __init__(self, S: str, T: str) -> None:
    super().__init__(S, T)
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
          t = self.C[i, j - 1]
          d = self.DIR_LEFT
        
          if self.C[i - 1, j - 1] < t:
            t = self.C[i - 1, j - 1]
            d = self.DIR_TOP_LEFT
        
          if self.C[i - 1, j] < t:
            t = self.C[i - 1, j]
            d = self.DIR_TOP

          self.C[i, j] = t + 1
          self.D[i, j] = d

    print('BEGIN DP')
    print("".center(3), end="")
    for i in range(0, self.m + 1):
      c = ""
      if i > 0:
        c = self.T[i - 1]
      print("", c.center(3), end="")
    print()
    for i in range(0, self.n + 1):
      c = ""
      if i > 0:
        c = self.S[i - 1]
      print(c.center(3), end="")
      for j in range(0, self.m + 1):
        print("", str(self.C[i, j]).center(3), end="")
      print()

    print("".center(3), end="")
    for i in range(0, self.m + 1):
      c = ""
      if i > 0:
        c = self.T[i - 1]
      print("", c.center(3), end="")
    print()
    for i in range(0, self.n + 1):
      c = ""
      if i > 0:
        c = self.S[i - 1]
      print(c.center(3), end="")
      for j in range(0, self.m + 1):
        print("", self.dir_to_str(self.D[i, j]).center(3), end="")
      print()
    print('END   DP')
    return self

  def digest(self) -> list[list[int]]:
    result = []
    i, j = self.n, self.m
    while i >= 0 and j >= 0 and self.D[i, j] != self.DIR_NOP:
      if self.D[i, j] == self.DIR_TOP_LEFT:
        if self.S[i - 1] != self.T[j - 1]:
          result.append((self.OP_REPLACE, i, j))
        i, j = i - 1, j - 1
      elif self.D[i, j] == self.DIR_LEFT:
        result.append((self.OP_INSERT, j, i, i + 1))
        j = j - 1
      elif self.D[i, j] == self.DIR_TOP:
        result.append((self.OP_DELETE, i))
        i = i - 1
    if i < 0 or j < 0:
      raise RuntimeError("It shouldnt be possible to reach i=%d j=%d" % (i, j))
    self.display_edit(result)
    return result[::-1]

  @staticmethod
  def run(S: str, T: str) -> list[list[int]]:
    return DPED(S, T).execute().digest()
