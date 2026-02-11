from __future__ import annotations
import abc

class LCS(abc.ABC):
  @staticmethod
  @abc.abstractmethod
  def run(S: str, T: str) -> str:
    pass
