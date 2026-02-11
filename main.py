from __future__ import annotations
from dplcs import DPLCS
from dped import DPED
from wflcs import WFLCS
from wfed import WFED
import random
import sys

import argparse

def random_string():
  alphabet = 'ACGT'
  length = random.randint(5, 15)
  return "".join([random.choice(alphabet) for _ in range(length)])

def main():
  argument_parser = argparse.ArgumentParser()
  argument_parser.add_argument('-m', '--mode', choices=['lcs', 'ed'], default='ed')
  argument_parser.add_argument('-s', '--source', choices=['wiki', 'uko', 'dna'], default='uko')
  argument_parser.add_argument('-S', '--seed', type=int, default=170701, nargs='?')
  cli = argument_parser.parse_args(sys.argv[1:])

  random.seed(cli.seed)
  
  S, T = "", ""

  if cli.source == 'wiki':
    S = "XMJYAUZ"
    T = "MZJAWXU"
  elif cli.source == 'uko':
   S = "yxxz"
   T = "xyxzy"
  elif cli.source == 'dna':
    S = random_string()
    T = random_string()

  print('S', S)
  print('T', T)

  expected, obtained = "", ""
  if cli.mode == 'lcs':
    expected = DPLCS.run(S, T)
    obtained = WFLCS.run(S, T)
  elif cli.mode == 'ed':
    expected = DPED.run(S, T)
    obtained = WFED.run(S, T)

  if expected == obtained:
    print("LCS(%s, %s) := %s == %s" % (S, T, expected, obtained))
  else:
    print("LCS(%s, %s) := %s <> %s" % (S, T, expected, obtained))

if __name__ == '__main__':
  main()
