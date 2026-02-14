from __future__ import annotations
import abc

class ED(abc.ABC):
  OP_REPLACE = 0
  OP_INSERT = 1
  OP_DELETE = 2

  def __init__(self, S: str, T: str) -> None:
    self.S, self.T = S, T
  
  @staticmethod
  @abc.abstractmethod
  def run(S: str, T: str) -> list[list[int]]:
    pass

  def display_edit(self, edit_sequence: list[list[int]]):
    a, b = self.S, self.T
    for edit in edit_sequence:
      op, args = edit[0], edit[1:]
      
      next_a = ""
      if op == self.OP_REPLACE:
        print("change a[%s] to b[%s]" % args)
        ai = args[0]
        bi = args[1]
        prefix = ""
        infill = ""
        suffix = ""
        if bi > 0:
          infill = b[bi - 1]
        if ai > 0:
          prefix = a[:ai - 1]
          suffix = a[ai:]
        else:
          prefix = ""
          suffix = a
        next_a = prefix + infill + suffix
      elif op == self.OP_INSERT:
        print("insert b[%s] between a[%s] and a[%s]" % args)
        bi = args[0]
        assert bi > 0
        ai = args[1]
        aj = args[2]
        assert aj == ai + 1
        prefix = ""
        infill = b[bi - 1]
        suffix = ""
        if ai > 0:
          prefix = a[:ai]
          suffix = a[ai:]
        else:
          prefix = ""
          suffix = a
        next_a = prefix + infill + suffix
      elif op == self.OP_DELETE:
        print("delete a[%s]" % args)
        ai = args[0]
        assert ai > 0
        prefix = a[:ai - 1]
        suffix = a[ai:]
        next_a = prefix + suffix
      else:
        raise ValueError(edit)
      print(a,'->', next_a)
      a = next_a
