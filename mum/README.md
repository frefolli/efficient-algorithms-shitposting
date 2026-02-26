# Support Data Structures

```python
class Interval:
  def __init__(self, length, lb, rb):
    # Length of common prefix
    self.length = length
    # left bound
    self.lb = lb
    # right bound
    self.rb = rb
```

```python
class Palindromes:
  def __init__(self):
    # Length of palindromes
    self.length = 0
    # Starting points of suffixes
    self.starts = []
```

# Bottom Up Traversal of Suffix Array

```python
# |LCP| = n
# T(n) = O(n)
def BottomUpTraversal(LCP[1:n]):
  stack = Stack()
  stack.push(Interval(length=0, lb=1, rb=n))

  for q in [2, 3, ..., n - 1, n]:
    if LCP[q] > LCP[q - 1]:
      stack.push(Interval(length=LCP[q], lb=q - 1, rb = NaN))
    elif LCP[q] < LCP[q - 1]:
      lb = q - 1
      l = stack.top().length
      while l > LCP[q]:
        I = stack.pop()
        l = I.length
        I.rb = q - 1
        lb = I.lb
        yield I
      if stack.top().length != LCP[q]:
        stack.push(Interval(length=LCP[q], lb=lb, rb = NaN))

  while not stack.empty():
    I = stack.pop()
    I.rb = n - 1
    yield I
```

# Evaluation of LCP-Interval

```python
# |SA| = n
# |I| = k
# T(n, k) = O(k^2)
def EvaluateLCPInterval(SA[1:n], pivot, P, I):
  if I.length >= P.length:
    # The length of T + '#' + T^-1
    len = n - 1
    lcp = I.length
    for i in [I.lb, I.lb + 1, ..., I.rb - 1, I.rb]:
      for j in [i + 1, ..., I.rb - 1, I.rb]:
        min_ij = min(SA[i], SA[j])
        max_ij = max(SA[i], SA[j])
        # On on the left of the '#'
        # On on the right of the '#'
        if min_ij < pivot < max_ij:
          if max_ij = len - min_ij - lcp:
            if lcp > P.length:
              P.length = lcp
              P.starts = []
            P.starts.append(min_ij)
```

# Full

```python
# |A| = a
# |S| = m
def MummyWhereAreMyLongestPalindromes(A, S[1:m]):
  # NB: S doesn't end with '$'
  # |T| = n = 2 * m + 2
  T = S + '#' + reverse(S) + '$'
  # |SA| = n
  # T<SA>(n) = O(n)
  SA = SuffixArray(T)
  # |LCP| = n
  # T<LCP>(n) = O(n)
  LCP = LCPArray(SA, T)
  P = Palindromes()
  # T<BottomUpTraversal>(n) = O(n)
  # T<BottomUpTraversal x EvaluateLCPInterval>(n, k) = O(nk^2)
  for I in BottomUpTraversal(LCP):
    # |I| = k
    # 1 <= k <= n
    # T<EvaluateLCPInterval>(n, k) = O(k^2) = O(n^2)
    EvaluateLCPInterval(SA, |S| + 1, P, I)
  return P
```

# Example

```python
def main():
  A = Alphabet("acgt", terminal='$', separator='#')
  S = Text("ccacatggccagatgtcagaatcgggtct")
  P = MummyWhereAreMyLongestPalindromes(S)
  for start in S.starts:
    print("|>", S[start : start + P.length - 1])
# Output:
#|> aca
#|> aga
#|> aga
#|> cac
#|> ggg
#|> tct
#|> tgt
```

# Time Analysis

