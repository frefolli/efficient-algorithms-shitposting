## Alphabet
$\Sigma$ := (Characters = [a, b, c, d], Terminal = `$`, Separator = `#`)


## TextS
$T_1$ := abcdaaaa$

$T_2$ := bbbabcd$

## Text
T := abcdaaaa#bbbabcd$

## Local Maximums
### [5,6]
lcp := 3
match := `aaa`
### [7,8]
lcp := 4
match := `abcd`
### [10,11]
lcp := 2
match := `bb`
### [12,13]
lcp := 3
match := `bcd`
### [14,15]
lcp := 2
match := `cd`
## Global Maximums
### [5,6]
lcp := 3
match := `aaa`
### [7,8]
lcp := 4
match := `abcd`
### [10,11]
lcp := 2
match := `bb`
## Maximal Unique Matches
### [7,8]
lcp := 4
match := `abcd`
## Suffix Array, LCP and BWT
| `#` | SA  | LCP |  B  | SUFFIX |
| --- | --- | --- | --- | ------ |
|1|17|-1|d|`$`|
|2|9|0|a|`#bbbabcd$`|
|3|8|0|a|`a#bbbabcd$`|
|4|7|1|a|`aa#bbbabcd$`|
|5|6|2|a|`aaa#bbbabcd$`|
|6|5|3|d|`aaaa#bbbabcd$`|
|7|13|1|b|`abcd$`|
|8|1|4|$|`abcdaaaa#bbbabcd$`|
|9|12|0|b|`babcd$`|
|10|11|1|b|`bbabcd$`|
|11|10|2|#|`bbbabcd$`|
|12|14|1|a|`bcd$`|
|13|2|3|a|`bcdaaaa#bbbabcd$`|
|14|15|0|b|`cd$`|
|15|3|2|b|`cdaaaa#bbbabcd$`|
|16|16|0|c|`d$`|
|17|4|1|c|`daaaa#bbbabcd$`|

## FM Index
| $\sigma$ | $C[\sigma]$ |
| -------- | ----------- |
|$|0|
|#|1|
|a|2|
|b|8|
|c|13|
|d|15|

| $i$ | $Occ[$`$`$]$ | $Occ[$`#`$]$ | $Occ[$`a`$]$ | $Occ[$`b`$]$ | $Occ[$`c`$]$ | $Occ[$`d`$]$ |
| --- | -------- | -------- | -------- | -------- | -------- | -------- |
| 1 | 0| 0| 0| 0| 0| 0|
| 2 | 0| 0| 0| 0| 0| 1|
| 3 | 0| 0| 1| 0| 0| 1|
| 4 | 0| 0| 2| 0| 0| 1|
| 5 | 0| 0| 3| 0| 0| 1|
| 6 | 0| 0| 4| 0| 0| 1|
| 7 | 0| 0| 4| 0| 0| 2|
| 8 | 0| 0| 4| 1| 0| 2|
| 9 | 1| 0| 4| 1| 0| 2|
| 10 | 1| 0| 4| 2| 0| 2|
| 11 | 1| 0| 4| 3| 0| 2|
| 12 | 1| 1| 4| 3| 0| 2|
| 13 | 1| 1| 5| 3| 0| 2|
| 14 | 1| 1| 6| 3| 0| 2|
| 15 | 1| 1| 6| 4| 0| 2|
| 16 | 1| 1| 6| 5| 0| 2|
| 17 | 1| 1| 6| 5| 1| 2|
