from __future__ import annotations
from dplcs import DPLCS
from wflcs import WFLCS
from dped import DPED
from wfed import WFED
from dplte import DPLTE
from wflte import WFLTE
import random
import sys

import argparse

def random_string(length: int):
  alphabet = 'ACGT'
  return "".join([random.choice(alphabet) for _ in range(length)])

def main():
  argument_parser = argparse.ArgumentParser()
  argument_parser.add_argument('-m', '--mode', choices=['lcs', 'ed', 'lte'], default='ed')
  argument_parser.add_argument('-s', '--source', choices=['wiki', 'uko', 'dna'], default='uko')
  argument_parser.add_argument('-S', '--seed', type=int, default=170701, nargs='?')
  cli = argument_parser.parse_args(sys.argv[1:])

  seed = cli.seed
  if seed is None:
    seed = random.randint(0, 100000)
  print('seed', seed)
  random.seed(seed)
  
  S, T = "", ""

  if cli.source == 'wiki':
    S = "XMJYAUZ"
    T = "MZJAWXU"
  elif cli.source == 'uko':
   S = "yxxz"
   T = "xyxzy"
  elif cli.source == 'dna':
    length = random.randint(5, 15)
    S = random_string(length)
    T = random_string(length)

  print('S', S)
  print('T', T)

  expected, obtained = "", ""
  ok = True
  if cli.mode == 'lcs':
    expected = DPLCS.run(S, T)
    obtained = WFLCS.run(S, T)
    ok = (len(obtained) == len(expected))
  elif cli.mode == 'ed':
    expected = str(len(DPED.run(S, T)))
    obtained = str(len(WFED.run(S, T)))
    ok = (obtained == expected)
  elif cli.mode == 'lte':
    expected = str(len(DPLTE.run(S, T)))
    obtained = str(len(WFLTE.run(S, T)))
    ok = (obtained == expected)

  verdict = 'OK'
  if not ok:
    verdict = 'KO'

  fields = [cli.mode.upper(), S, T, expected, obtained, verdict]
  heads = ["OP", "S", "T", "DP", "WF", "RES"]
  length = max([len(_) for _ in fields+heads])
  fields = [_.center(length, ' ') for _ in fields]
  heads = [_.center(length, ' ') for _ in heads]
  seps = ['-' * length for _ in heads]

  print("|" + "|".join(heads) + "|")
  print("|" + "|".join(seps) + "|")
  print("|" + "|".join(fields) + "|")


if __name__ == '__main__':
  main()
