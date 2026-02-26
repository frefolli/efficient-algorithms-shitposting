# Edit Distance

| Symbol | Type          | Description |
| ------ | ------------- | ----------- |
| A[1:m] | Input         |  String `a` defined with characters from 1 to m |
| B[1:n] | Input         |  String `b` defined with characters from 1 to n |
| P      | Input         |  Upper bound of Edit Distance cost |
| Ps     | Set           |  Set of Costs from 0 to P ($|Ps| = P + 1$) |
| mind   | Diagonal      |  Lowest Negative Diagonal (max of -P and -m) |
| maxd   | Diagonal      |  Highest Positive Diagonal (min of P and n) |
| Ks     | Set           |  Set of diagonals from `mind` to `maxd` ($|Ks| = maxd - mind + 1$) |
| NaN    | Numeric Value |  Not-a-Number Value. It is assumed to be like $-\infty$, therefore for every finite integer $x$, it holds that $NaN < x$ and $NaN + x = NaN$ |
| D      | Matrix        |  Edit Distance Costs Matrix. D[i,j] = d is the Edit Distance of a[:i] and b[:j] |
| W      | Matrix        |  Matrix (Diag x Cost) -> RowIdx. Each cell W[k,p] = t is the largest row index t such that D[t, t + k] = p. |
| C      | Matrix        |  Matrix (Diag x Cost) -> Choice. Each cell W[k,p] = c is the choice made, which is the equivalent choice inside the Edit Distance cost matrix. Choice = Enum {Nop, Ins, Del, Sub}, meaning No-Operation, Insert, Deletion, Substitution. |

## Init

```python
def Wavefront_Init(A[1:m], B[1:n], P):
  mind = max(-P, -m)
  maxd = min(P, n)
  Ks = {mind, mind+1, ..., maxd-1, maxd}
  Ps = {0, 1, ..., P - 1, P}

  W = Matrix_New(|Ks|, |Ps|)
  C = Matrix_New(|Ks|, |Ps|)

  for k in Ks:
    for p in Ps:
      W[k, p] = NaN
      C[k, p] = Nop

  for k in Ks:
    if k > 0:
      W[k, |k| - 1] = -1
    elif k < 0:
      W[k, |k| - 1] = |k| - 1

  t = 0
  while t + 1 <= m and t + 1 <= n and A[t + 1] = B[t + 1]:
    t = t + 1
  W[0, 0] = t

  return (W, C, Ks, Ps)
```

## Iterate

```python
def Wavefront_Iterate(A[1:m], B[1:n], W, C, Ks, p):
  for k in Ks:
    if p >= |k|:
      t = NaN
      c = Nop

      # Insertion
      if (k - 1) in Ks:
        t_ins = W[k - 1, p - 1]
        if 0 <= t_ins <= m and 0 <= t_ins + k <= n and t_ins > t:
          t = t_ins
          c = Ins

      # Substitution
      t_sub = W[k, p - 1] + 1
      if 0 <= t_sub <= m and 0 <= t_sub + k <= n and t_sub > t:
        t = t_sub
        c = Sub

      # Deletion
      if (k + 1) in Ks:
        t_del = W[k + 1, p - 1] + 1
        if 0 <= t_del <= m and 0 <= t_del + k <= n and t_del > t:
          t = t_del
          c = Del

      if t <= max(W[k, p - 1]):
        t = NaN
        c = Nop

      if t != NaN:
        while t + 1 <= m and t + k + 1 <= n and A[t + 1] = B[t + k + 1]:
          t = t + 1

      if t > m or t + k > n:
        t = NaN
        c = Nop

      W[k, p] = t
      C[k, p] = c
```

## Reconstruct

```python
def Wavefront_Reconstruct(A[1:m], B[1:n], W, C, Ps, cost):
  p = cost
  k = n - m

  edits = []
  while p > 0:
    c = D[k, p]
    if c = Ins:
      t = W[k - 1, p - 1]
      edits.append(Insert(t + k, t, t + t))
      k = k - 1
    elif c = Sub:
      t = W[k, p - 1] + 1
      edits.append(Substitute(t, t + k))
    elif c = Del:
      t = W[k + 1, p - 1] + 1
      edits.append(Delete(t))
      k = k + 1
  return edits
```

## Full

```python
def Wavefront(A[1:m], B[1:n], P):
  (W, C, Ks, Ps) = Wavefront_init(A, B, P)
  cost = NaN
  for p in Ps \ {0}:
    Wavefront_Iterate(A, B, W, C, Ks, p)
    if W[n - m, p] = m:
      cost = p
      break
  if cost = NaN:
    return Err("The real ED cost overhauls the upper bound")
  else:
    return Ok(Wavefront_Reconstruct(A, B, W, C, Ps, cost))
```

# LCS

## Init

```python
def Wavefront_Init(A[1:m], B[1:n], P):
  mind = max(-P, -m)
  maxd = min(P, n)
  Ks = {mind, mind+1, ..., maxd-1, maxd}
  Ps = {0, 1, ..., P - 1, P}

  W = Matrix_New(|Ks|, |Ps|)
  C = Matrix_New(|Ks|, |Ps|)

  for k in Ks:
    for p in Ps:
      W[k, p] = NaN
      C[k, p] = Nop

  for k in Ks:
    if k > 0:
      W[k, |k| - 1] = -1
    elif k < 0:
      W[k, |k| - 1] = |k| - 1

  t = 0
  while t + 1 <= m and t + 1 <= n and A[t + 1] = B[t + 1]:
    t = t + 1
  W[0, 0] = t

  return (W, C, Ks, Ps)
```

## Iterate

```python
def Wavefront_Iterate(A[1:m], B[1:n], W, C, Ks, p):
  for k in Ks:
    if p >= |k|:
      t = NaN
      c = Nop

      # Insertion
      if (k - 1) in Ks:
        t_ins = W[k - 1, p - 1]
        if 0 <= t_ins <= m and 0 <= t_ins + k <= n and t_ins > t:
          t = t_ins
          c = Ins

      # Substitution
      if p = |k|:
        t_sub = W[k, p - 1] + 1
        if 0 <= t_sub <= m and 0 <= t_sub + k <= n and t_sub > t:
          t = t_sub
          c = Sub

      # Deletion
      if (k + 1) in Ks:
        t_del = W[k + 1, p - 1] + 1
        if 0 <= t_del <= m and 0 <= t_del + k <= n and t_del > t:
          t = t_del
          c = Del

      if t <= max(W[k, p - 1]):
        t = NaN
        c = Nop

      if t != NaN:
        while t + 1 <= m and t + k + 1 <= n and A[t + 1] = B[t + k + 1]:
          t = t + 1

      if t > m or t + k > n:
        t = NaN
        c = Nop

      W[k, p] = t
      C[k, p] = c
```

## Reconstruct

```python
def Wavefront_Reconstruct(A[1:m], B[1:n], W, C, Ps, cost):
    p = cost
    k = n - m

    result = ""
    while p >= 0:
      c = C[k, p]
      i = W[k, p]

      if c = Ins:
        t = W[k - 1, p - 1]
        while i > t and A[i] == B[i + k]:
          result += A[i]
          i -= 1
        k -= 1
      elif c = Sub:
        t = W[k, p - 1] + 1
        while i > t and A[i] == B[i + k]:
          result += A[i]
          i -= 1
      elif c = Del:
        t = W[k + 1, p - 1]
        while i > t and A[i] == B[i + k]:
          result += A[i]
          i -= 1
        k += 1
      elif c = Nop:
        t = W[k, p - 1] + 1
        while i > t and A[i] == B[i + k]:
          result += A[i]
          i -= 1
      p -= 1
    return reverse(result)
```
