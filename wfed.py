from __future__ import annotations
from ed import ED
import numpy

def abs(x: int) -> int:
  if x < 0:
    return -x
  return x

class WFED(ED):
  DIR_NOP: numpy.uint8 = numpy.uint8(0)
  DIR_TOP: numpy.uint8 = numpy.uint8(1)
  DIR_LEFT: numpy.uint8 = numpy.uint8(2)
  DIR_TOP_LEFT: numpy.uint8 = numpy.uint8(3)
  INFTY = -10

  def dir_to_str(self, dir: numpy.uint8) -> str:
    if dir == self.DIR_NOP:
      return "·"
    elif dir == self.DIR_TOP:
      return "↗"
    elif dir == self.DIR_LEFT:
      return "↖"
    elif dir == self.DIR_TOP_LEFT:
      return "↑"
    raise ValueError(dir)

  def safe_t_sum(self, x: int, y: int):
    if x == self.INFTY or y == self.INFTY:
      return self.INFTY
    return x + y

  def diag_idx_to_starter(self, x: int) -> tuple[int, int]:
    assert x >= self.mind
    assert x <= self.maxd
    r = 1
    if x < 0:
      return (r-x, r)
    else:
      return (r, x+r)

  def get_diag(self, x: int) -> int:
    assert x >= self.mind
    assert x <= self.maxd
    rel = x - self.mind
    assert rel >= 0
    assert rel < self.diagno
    return rel

  def __init__(self, a: str, b: str, k: int) -> None:
    self.a, self.b = a, b
    self.k = k
    self.m, self.n = len(a), len(b)
    self.mind = max(- self.k, - self.m)
    self.maxd = min(self.k, self.n)
    self.diagno = -self.mind + self.maxd + 1
    self.W = numpy.zeros(shape=(self.diagno, self.k + 1), dtype=numpy.int32)
    self.D = numpy.zeros(shape=(self.diagno, self.k + 1), dtype=numpy.uint8)
    for i in range(self.diagno):
      for j in range(self.k + 1):
        self.W[i, j] = self.INFTY
        self.D[i, j] = self.DIR_NOP
    
    for k in range(self.mind, self.maxd + 1):
      p = abs(k) - 1
      if k < 0:
        self.W[self.get_diag(k), p] = p
      elif k > 0:
        self.W[self.get_diag(k), p] = -1
    
    k = 0
    p = 0
    t = 0
    while self.ok_on_A(t + 1) and self.ok_on_B(t + k + 1) and self.get_from_A(t + 1) == self.get_from_B(t + k + 1):
      t += 1
    self.W[self.get_diag(k), p] = t

  def display(self) -> WFED:
    W_lines = []
    W_lines.append("|" + " | ".join(["   "] + [str(x).center(3) for x in range(self.mind, self.maxd + 1)]) + " |")
    W_lines.append("|" + " | ".join(["---"] + ["---" for x in range(self.mind, self.maxd + 1)]) + " |")
    for p in range(self.k + 1):
      line = []
      for k in range(self.diagno):
        v = self.W[k, p]
        if v == self.INFTY:
          v = "   ".rjust(3)
        else:
          v = str(v).center(3)
        line.append(v)
      W_lines.append("|" + str(p).center(4) + "| " + " | ".join(line) + " |")

    D_lines = []
    D_lines.append("|" + " | ".join(["   "] + [str(x).center(3) for x in range(self.mind, self.maxd + 1)]) + " |")
    D_lines.append("|" + " | ".join(["---"] + ["---" for x in range(self.mind, self.maxd + 1)]) + " |")
    for p in range(self.k + 1):
      line = []
      for k in range(self.diagno):
        v = self.D[k, p]
        v = self.dir_to_str(v).center(3)
        line.append(v)
      D_lines.append("|" + str(p).center(4) + "| " + " | ".join(line) + " |")
    with open("COPY.md", mode='r') as source:
      with open("README.md", mode='w') as destination:
        destination.write(source.read() + '\n')
        destination.write("\n## W Matrix\n")
        destination.write("\n".join(W_lines))
        destination.write("\n## D Matrix\n")
        destination.write("\n".join(D_lines))

    M = numpy.zeros(shape=(self.m + 1, self.n + 1), dtype=numpy.int32)
    for i in range(self.m + 1):
      for j in range(self.n + 1):
        M[i, j] = self.INFTY
    for i in range(self.m + 1):
      for j in range(self.n + 1):
        k = j - i
        p = 0
        while p < self.k and self.W[self.get_diag(k), p] < i:
          p += 1
        M[i, j] = p
    print('BEGIN WF')
    print(self.W)
    print(M)
    print('END   WF')
    return self

  def ok_on_A(self, i: int) -> bool:
    return i >= 1 and i <= self.m

  def ok_on_B(self, j: int) -> bool:
    return j >= 1 and j <= self.n

  def get_from_A(self, i: int) -> str:
    return self.a[i - 1]

  def get_from_B(self, j: int) -> str:
    return self.b[j - 1]

  def ok_diag(self, x: int) -> bool:
    return x >= self.mind and x <= self.maxd

  def WF(self, k: int, p: int) -> int:
    assert k >= 0
    assert k < self.diagno
    assert p >= 0
    assert p <= self.k
    return self.W[k, p]

  def ok_on_Dk(self, t: int, k: int):
    # Is a valid point on the k-diagonal of the D matrix
    return (t >= 0 and t <= self.m) and (t + k >= 0 and t + k <= self.n)

  def execute(self) -> WFED:
    self.cost = self.INFTY
    # DIAGNOSTICS
    #G = numpy.array([
    #    [self.INFTY,self.INFTY,self.INFTY,self.INFTY,0,self.INFTY,self.INFTY,self.INFTY,self.INFTY,self.INFTY],
    #    [self.INFTY,self.INFTY,self.INFTY, 2 ,1  , 2 ,self.INFTY,self.INFTY,self.INFTY,self.INFTY],
    #    [self.INFTY,self.INFTY, 3 , 3 ,4  , 3 , 2 ,self.INFTY,self.INFTY,self.INFTY],
    #    [self.INFTY, 4 , 4 , 4 ,self.INFTY, 4 , 3 , 2 ,self.INFTY,self.INFTY],
    #    [ 4 ,self.INFTY,self.INFTY,self.INFTY,self.INFTY,self.INFTY,self.INFTY,self.INFTY,1  ,self.INFTY],
    #    [self.INFTY,self.INFTY,self.INFTY,self.INFTY,self.INFTY,self.INFTY,self.INFTY,self.INFTY,self.INFTY, 0],
    #])
    for p in range(1, self.k + 1):
      for k in range(self.mind, self.maxd + 1):
        if p >= abs(k):
          t = self.INFTY
          d = self.DIR_NOP

          # Substitution
          if self.ok_diag(k):
            candidate_t = self.WF(self.get_diag(k), p - 1)
            candidate_t_plus_1 = self.safe_t_sum(candidate_t, 1)
            if candidate_t_plus_1 != self.INFTY and self.ok_on_Dk(candidate_t_plus_1, k) and (candidate_t_plus_1 > t or t == self.INFTY):
              t = candidate_t_plus_1
              d = self.DIR_TOP_LEFT
          # Insertion
          if self.ok_diag(k - 1):
            candidate_t = self.WF(self.get_diag(k - 1), p - 1)
            if candidate_t != self.INFTY and self.ok_on_Dk(candidate_t, k) and (candidate_t > t or t == self.INFTY):
              t = candidate_t
              d = self.DIR_LEFT
          # Deletion
          if self.ok_diag(k + 1):
            candidate_t = self.WF(self.get_diag(k + 1), p - 1)
            candidate_t_plus_1 = self.safe_t_sum(candidate_t, 1)
            if candidate_t_plus_1 != self.INFTY and self.ok_on_Dk(candidate_t_plus_1, k) and (candidate_t_plus_1 > t or t == self.INFTY):
              t = candidate_t_plus_1
              d = self.DIR_TOP

          # Reject non increasing t-sequences
          if t != self.INFTY:
            if t <= max(self.W[self.get_diag(k), :]):
              t = self.INFTY

          if t != self.INFTY:
            if t <= self.m and t + k <= self.n:
              while (
                self.ok_on_A(t + 1) and
                self.ok_on_B(t + k + 1) and
                self.get_from_A(t + 1) == self.get_from_B(t + k + 1)
              ):
                t += 1
              if t > self.m or t + k > self.n:
                t = self.INFTY
            else:
              t = self.INFTY
          self.W[self.get_diag(k), p] = t
          self.D[self.get_diag(k), p] = d
          # DIAGNOSTICS
          #if self.W[self.get_diag(k), p] != G[p, self.get_diag(k)]:
          #  print("(k=%s, p=%s) = %s instead of %s" % (k, p, self.W[self.get_diag(k), p], G[p, self.get_diag(k)]))

          if self.WF(self.get_diag(self.n - self.m), p) == self.m:
            self.cost = p
            #break
    return self.display()

  def digest(self) -> str:
    assert self.cost != self.INFTY
    p = self.cost
    k = self.n - self.m
    i = self.m
    while p > 0:
      choice = self.D[self.get_diag(k), p]
      print(self.dir_to_str(choice))
      if choice == self.DIR_TOP_LEFT:
        prev_i = self.W[self.get_diag(k), p - 1]
        for r in range(prev_i, i):
          ai = r
          bj = r + k
          if self.ok_on_A(ai) and self.ok_on_B(bj):
            print('-', self.get_from_A(ai))
        i = prev_i
        p -= 1
      elif choice == self.DIR_LEFT:
        i = self.W[self.get_diag(k - 1), p - 1]
        k -= 1
        p -= 1
      elif choice == self.DIR_TOP:
        i = self.W[self.get_diag(k + 1), p - 1]
        k += 1
        p -= 1
      elif choice == self.DIR_NOP:
        break
    while i > 0:
      ai = i - 1
      bj = ai + k
      if self.ok_on_B(bj):
        print(self.get_from_B(bj))
      i -= 1
    return self.a

  @staticmethod
  def run(S: str, T: str) -> str:
    k = max(len(S), len(T))
    return WFED(S, T, k).execute().digest()
