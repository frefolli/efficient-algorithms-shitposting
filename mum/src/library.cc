#include <chrono>
#include <library.hh>
#define ENABLE_TELEMETRY

Alphabet::Alphabet(const std::string& characters, char terminal, char separator) {
  this->symbols = "";
  assert(!characters.empty());
  assert(!characters.contains(terminal));
  assert(!characters.contains(separator));
  assert(terminal != separator);
  this->symbols.push_back(terminal);
  this->symbols.push_back(separator);
  this->symbols.append(characters);
}

intmax_t Alphabet::terminal() const {
  return 0;
}

intmax_t Alphabet::separator() const {
  return 1;
}

std::string_view Alphabet::characters() const {
  return std::string_view(symbols).substr(2);
}

intmax_t Alphabet::size() const {
  return this->symbols.size();
}

intmax_t Alphabet::sample() const {
  return (std::rand() % this->characters().size()) + 2;
}

std::ostream& Alphabet::to_markdown(std::ostream& out) const {
  out << "(Characters = [";
  bool first = true;
  for (char c : this->characters()) {
    if (first) {
      first = false;
    } else {
      out << ", ";
    }
    out << c;
  }
  out << "], Terminal = `"
    << this->symbols[this->terminal()]
    << "`, Separator = `"
    << this->symbols[this->separator()]
    << "`)";
  return out;
}

Text Text::from(const Alphabet& alphabet, const std::string& raw_text) {
  Text text;
  text.symbols.resize(raw_text.size());
  for (intmax_t idx = 0; idx < raw_text.size(); ++idx) {
    intmax_t char_idx = alphabet.symbols.find(raw_text[idx]);
    assert(char_idx < alphabet.size());
    if (char_idx == alphabet.terminal()) {
      assert(idx == raw_text.size() - 1);
    }
    text.symbols[idx] = char_idx;
  }
  if (text.back() != alphabet.terminal()) {
    text.symbols.push_back(alphabet.terminal());
  }
  return text;
}

Text Text::sequence(const Alphabet& alphabet, intmax_t min_length, intmax_t max_length) {
  assert(max_length >= min_length);
  intmax_t delta_length = max_length - min_length;
  intmax_t length = min_length + (std::rand() % delta_length);
  Text text;
  text.symbols.resize(length + 1);
  for (intmax_t idx = 0; idx < length; ++idx) {
    text.symbols[idx] = alphabet.sample();
  }
  text.symbols[length] = alphabet.terminal();
  return text;
}

intmax_t Text::back() const {
  return symbols.back();
}

intmax_t Text::size() const {
  return symbols.size();
}

std::ostream& Text::to_markdown(std::ostream& out, const Alphabet& alphabet) const {
  for (intmax_t char_idx : this->symbols) {
    out << alphabet.symbols[char_idx];
  }
  return out;
}

std::string Text::slice_suffix(const Alphabet& alphabet, intmax_t start_idx) const {
  std::string result = "";
  for (intmax_t idx = start_idx; idx < symbols.size(); ++idx) {
    result += alphabet.symbols[symbols[idx]];
  }
  return result;
}

Text Text::invert() const {
  Text text;
  text.symbols.resize(this->symbols.size());
  for (intmax_t idx = 0; idx < this->symbols.size() - 1; ++idx) {
    text.symbols[text.symbols.size() - 2 - idx] = this->symbols[idx];
  }
  text.symbols[text.symbols.size() - 1] = this->symbols.back();
  return text;
}

Text Text::join(const Text& other, const Alphabet& alphabet) const {
  Text text;
  text.symbols.resize(this->symbols.size() + other.symbols.size());
  for (intmax_t idx = 0; idx < this->symbols.size(); ++idx) {
    text.symbols[idx] = this->symbols[idx];
  }
  text.symbols[this->symbols.size() - 1] = alphabet.separator();
  for (intmax_t idx = 0; idx < other.symbols.size(); ++idx) {
    text.symbols[idx + this->symbols.size()] = other.symbols[idx];
  }
  return text;
}


SuffixArray SuffixArray::build(const Text& text) {
  SuffixArray suffix_array;
  intmax_t length = text.symbols.size();
  suffix_array.indexes.resize(length);

  for (intmax_t idx = 0; idx < length; ++idx) {
    suffix_array.indexes[idx] = idx;
  }

  std::sort(suffix_array.indexes.begin(), suffix_array.indexes.end(), [&text, &length](intmax_t i, intmax_t j) {
    while (i < length && j < length) {
      if (text.symbols[i] < text.symbols[j]) {
        return true;
      } else if (text.symbols[i] > text.symbols[j]) {
        return false;
      } else {
        i += 1;
        j += 1;
      }
    }

    if (i >= length) {
      return true;
    } else {
      return false;
    }
  });

  return suffix_array;
}


BurrowsWheelerTransform BurrowsWheelerTransform::build(const Text& text, const SuffixArray& suffix_array) {
  BurrowsWheelerTransform burrows_wheeler_transform;
  intmax_t length = text.symbols.size();
  burrows_wheeler_transform.symbols.resize(length);

  for (intmax_t idx = 0; idx < length; ++idx) {
    intmax_t charidx = (suffix_array.indexes[idx] > 0) ? (suffix_array.indexes[idx] - 1) : (text.size() - 1);
    burrows_wheeler_transform.symbols[idx] = text.symbols[charidx];
  }

  return burrows_wheeler_transform;
}

FMIndex FMIndex::build(const Alphabet& alphabet, const BurrowsWheelerTransform& burrows_wheeler_transform) {
  intmax_t alphabet_length = alphabet.size();
  intmax_t bwt_length = burrows_wheeler_transform.symbols.size();

  FMIndex fm_index;
  fm_index.C.resize(alphabet_length);
  fm_index.Occ.resize(alphabet_length);
  for (intmax_t char_idx = 0; char_idx < alphabet_length; ++char_idx) {
    fm_index.Occ[char_idx].resize(bwt_length);
  }

  for (intmax_t symbol_idx = 0; symbol_idx < bwt_length; ++symbol_idx) {
    intmax_t current_char_idx = burrows_wheeler_transform.symbols[symbol_idx];
    for (intmax_t higher_char_idx = current_char_idx + 1; higher_char_idx < alphabet_length; ++higher_char_idx) {
      fm_index.C[higher_char_idx]++;
    }
    if (symbol_idx < bwt_length - 1) {
      for (intmax_t char_idx = 0; char_idx < alphabet_length; ++char_idx) {
        if (char_idx == current_char_idx) {
          fm_index.Occ[char_idx][symbol_idx + 1] = fm_index.Occ[char_idx][symbol_idx] + 1;
        } else {
          fm_index.Occ[char_idx][symbol_idx + 1] = fm_index.Occ[char_idx][symbol_idx];
        }
      }
    }
  }

  return fm_index;
}

std::ostream& FMIndex::to_markdown(std::ostream& out, const Alphabet& alphabet) {
  intmax_t alphabet_length = alphabet.size();
  intmax_t bwt_length = Occ.front().size();

  out << "| $\\sigma$ | $C[\\sigma]$ |" << std::endl;
  out << "| -------- | ----------- |" << std::endl;
  for (intmax_t char_idx = 0; char_idx < alphabet_length; ++char_idx) {
    out << "|" << alphabet.symbols[char_idx] << "|" << C[char_idx] << "|" << std::endl;
  }
  out << std::endl;

  out << "| $i$ |";
  for (intmax_t char_idx = 0; char_idx < alphabet_length; ++char_idx) {
    out << " $Occ[$`" << alphabet.symbols[char_idx] << "`$]$ |";
  }
  out << std::endl;
  out << "| --- |";
  for (intmax_t char_idx = 0; char_idx < alphabet_length; ++char_idx) {
    out << " -------- |";
  }
  out << std::endl;
  for (intmax_t symbol_idx = 0; symbol_idx < bwt_length; ++symbol_idx) {
    out << "| " << symbol_idx + 1 << " |";
    for (intmax_t char_idx = 0; char_idx < alphabet_length; ++char_idx) {
      out << " " << Occ[char_idx][symbol_idx] << "|";
    }
    out << std::endl;
  }
  return out;
}

LCPArray LCPArray::build(const Text& text, const SuffixArray& suffix_array) {
  intmax_t length = text.size();
  LCPArray lcp_array = {};
  lcp_array.lcps.resize(length);

  std::vector<intmax_t> rank;
  rank.resize(length);
  for (intmax_t idx = 0; idx < length; ++idx) {
    rank[suffix_array.indexes[idx]] = idx;
  }
  intmax_t h = 0;
  for (intmax_t idx = 0; idx < length; ++idx) {
    if (rank[idx] > 0) {
      intmax_t previous = rank[idx] - 1;
      intmax_t k = suffix_array.indexes[previous];
      while (idx + h < length && k + h < length && text.symbols[idx + h] == text.symbols[k + h]) {
        h++;
      }
      lcp_array.lcps[rank[idx]] = h;
      if (h > 0) {
        h--;
      }
    } else {
      lcp_array.lcps[rank[idx]] = -1;
    }
  }
  return lcp_array;
}


LocalMaximumIterator::LocalMaximumIterator(const LCPArray& lcp_array) : lcp_array(lcp_array) {
  this->q = 1;
  this->i = -1;
  this->j = -1;
}

bool LocalMaximumIterator::has_next() {
  intmax_t length = lcp_array.lcps.size();
  i = -1;
  j = -1;
  while (q < length) {
    if (lcp_array.lcps[q - 1] < lcp_array.lcps[q]) {
      i = q - 1;
      j = -1;
    } else if (lcp_array.lcps[q - 1] > lcp_array.lcps[q]) {
      if (i != -1) {
        j = q - 1;
      }
    } else if (lcp_array.lcps[q - 1] == lcp_array.lcps[q]) {
    }

    q++;
    if (i != -1 && j != -1) {
      break;
    }
  }

  if (i != -1 && j == -1) {
    j = length - 1;
  }
  return (i != -1 && j != -1);
}

std::pair<intmax_t, intmax_t> LocalMaximumIterator::next() {
  assert(i != -1 && j != -1);
  std::pair<intmax_t, intmax_t> res = {i, j};
  i = -1;
  j = -1;
  return res;
}


GlobalMaximumIterator::GlobalMaximumIterator(const LCPArray& lcp_array,
    BurrowsWheelerTransform& burrows_wheeler_transform)
  : local_maximum_iterator(lcp_array),
  burrows_wheeler_transform(burrows_wheeler_transform) {
    this->i = -1;
    this->j = -1;
  }

bool GlobalMaximumIterator::has_next() {
  this->i = -1;
  this->j = -1;

  bool ok = false;
  while (local_maximum_iterator.has_next()) {
    auto local_maximum = local_maximum_iterator.next();
    intmax_t ci = local_maximum.first;
    intmax_t cj = local_maximum.second;

    ok = true;
    for (intmax_t a = ci; a <= cj; ++a) {
      for (intmax_t b = a + 1; b <= cj; ++b) {
        assert(a != b);
        if (burrows_wheeler_transform.symbols[a] == burrows_wheeler_transform.symbols[b]) {
          ok = false;
        }
      }
    }

    if (ok) {
      i = ci;
      j = cj;
      break;
    }
  }

  return ok;
}

std::pair<intmax_t, intmax_t> GlobalMaximumIterator::next() {
  assert(i != -1 && j != -1);
  std::pair<intmax_t, intmax_t> res = {i, j};
  i = -1;
  j = -1;
  return res;
}


MaximalUniqueMatchesIterator::MaximalUniqueMatchesIterator(const LCPArray& lcp_array,
    BurrowsWheelerTransform& burrows_wheeler_transform,
    const SuffixArray& suffix_array,
    intmax_t separator_position)
  : global_maximum_iterator(lcp_array, burrows_wheeler_transform),
  suffix_array(suffix_array) {
    this->separator_position = separator_position;
    this->i = -1;
    this->j = -1;
  }

bool MaximalUniqueMatchesIterator::has_next() {
  this->i = -1;
  this->j = -1;

  bool ok = false;
  while (global_maximum_iterator.has_next()) {
    auto global_maximum = global_maximum_iterator.next();
    if (global_maximum.second != global_maximum.first + 1) {
      continue;
    }
    intmax_t ci = global_maximum.first;
    intmax_t cj = global_maximum.second;

    intmax_t min_ij = std::min(suffix_array.indexes[ci], suffix_array.indexes[cj]);
    intmax_t max_ij = std::max(suffix_array.indexes[ci], suffix_array.indexes[cj]);

    if (min_ij < separator_position && max_ij > separator_position) {
      i = ci;
      j = cj;
      ok = true;
      break;
    } else {
    }
  }

  return ok;
}

std::pair<intmax_t, intmax_t> MaximalUniqueMatchesIterator::next() {
  assert(i != -1 && j != -1);
  std::pair<intmax_t, intmax_t> res = {i, j};
  i = -1;
  j = -1;
  return res;
}

MaximalPalindromeMatchesIterator::MaximalPalindromeMatchesIterator(
    const LCPArray& lcp_array,
    const BurrowsWheelerTransform& burrows_wheeler_transform,
    const SuffixArray& suffix_array,
    intmax_t separator_position)
  : local_maximum_iterator(lcp_array),
  suffix_array(suffix_array),
  burrows_wheeler_transform(burrows_wheeler_transform) {
    this->separator_position = separator_position;
    this->i = -1;
    this->j = -1;
  }

// #define DEBUG
bool MaximalPalindromeMatchesIterator::has_next() {
  this->i = -1;
  this->j = -1;

  bool ok = false;
  while (local_maximum_iterator.has_next()) {
    auto local_maximum = local_maximum_iterator.next();
    intmax_t ci = local_maximum.first;
    intmax_t cj = local_maximum.second;

    intmax_t _2n_plus_1 = suffix_array.indexes.size();
    intmax_t _2n = _2n_plus_1 - 1;
    intmax_t lcp = local_maximum_iterator.lcp_array.lcps[ci + 1];
    for (intmax_t ki = ci; ki <= cj; ++ki) {
      for (intmax_t kj = ki + 1; kj <= cj; ++kj) {
        intmax_t min_ij = std::min(suffix_array.indexes[ki], suffix_array.indexes[kj]);
        intmax_t max_ij = std::max(suffix_array.indexes[ki], suffix_array.indexes[kj]);
        if (min_ij < separator_position && max_ij > separator_position) {
          assert(ki!=kj);
          if (burrows_wheeler_transform.symbols[ki] != burrows_wheeler_transform.symbols[kj]) {
            // min_ij is a start
            // max_ij is an end
            // the length of the text is L = n+m = 2n since n=m and $ terminates the string
            // but, if we consider only a string of type T1#T2, then we can use n+m-1 = 2n-1
            // now, the last (0-based) position is 2n-2 and since it is 0-based, the last character is 2n-2-0, the second last character is 2n-2-1=2n-3, the k-th last character is 2n-1-k
            // if min_ij starts at position s, then max_ij shall end at 2n-1-s
            // therefore i shall check that max_ij + lcp - 1 = 2n - 1 - min_ij
            // therefore i shall check that max_ij = 2n - min_ij - lcp
            if (max_ij == _2n - min_ij - lcp){
              i = ki;
              j = kj;
              ok = true;
              #ifdef  DEBUG
                std::cout << "cj, ci := " << cj << "," << ci << std::endl;
                std::cout << "cj-ci := " << cj-ci << std::endl;
                std::cout << "OK -> " << max_ij << " == " << _2n << " - " << min_ij << " - " << lcp << " = " << _2n - min_ij - lcp << std::endl;
              #endif//DEBUG
            } else {
              #ifdef  DEBUG
                std::cout << "KO -> " << max_ij << " != " << _2n << " - " << min_ij << " - " << lcp << " = " << _2n - min_ij - lcp << std::endl;
              #endif//DEBUG
            }
          }
          break;
        } else {
          #ifdef  DEBUG
            std::cout << "KO -> " << min_ij+1 << " < " << separator_position+1 << " < " << max_ij+1 << std::endl;
          #endif//DEBUG
        }
      }
    }

    if (ok) {
      break;
    }
  }

  return ok;
}

std::pair<intmax_t, intmax_t> MaximalPalindromeMatchesIterator::next() {
  assert(i != -1 && j != -1);
  std::pair<intmax_t, intmax_t> res = {i, j};
  i = -1;
  j = -1;
  return res;
}

std::ostream& operator<<(std::ostream& out, const Interval& interval) {
  return out << interval.length << "-[" << interval.lb+1 << "," << interval.rb+1 << "]";
}

void evaluate_lcp_interval(const SuffixArray& suffix_array,
                           intmax_t separator_position,
                           Palindromes& palindromes,
                           const Interval& interval) {
  #ifdef  ENABLE_TELEMETRY
    TELEMETRY.interval_count++;
    TELEMETRY.interval_size += interval.rb - interval.lb + 1;
  #endif//ENABLE_TELEMETRY
  if (interval.length >= palindromes.length) {
    std::cout << "|> " << interval << std::endl;
    #ifdef  DEBUG
      std::cout << "|> " << interval << std::endl;
    #endif//DEBUG
    intmax_t _2n_plus_1 = suffix_array.indexes.size();
    intmax_t _2n = _2n_plus_1 - 1;
    intmax_t lcp = interval.length;
    for (intmax_t i = interval.lb; i <= interval.rb; ++i) {
      for (intmax_t j = i + 1; j <= interval.rb; ++j) {
        intmax_t min_ij = std::min(suffix_array.indexes[i], suffix_array.indexes[j]);
        intmax_t max_ij = std::max(suffix_array.indexes[i], suffix_array.indexes[j]);
        #ifdef  ENABLE_TELEMETRY
          TELEMETRY.comparison_count++;
        #endif//ENABLE_TELEMETRY
        if (min_ij < separator_position && max_ij > separator_position) {
          assert(i!=j);
          #ifdef  DEBUG
          std::cout << "IF: SA[" << i + 1 << "] = " << suffix_array.indexes[i] + 1 << std::endl;
          std::cout << "IF: SA[" << j + 1 << "] = " << suffix_array.indexes[j] + 1 << std::endl;
          #endif//DEBUG
          // if (burrows_wheeler_transform.symbols[i] != burrows_wheeler_transform.symbols[j]) {
            // min_ij is a start
            // max_ij is an end
            // the length of the text is L = n+m = 2n since n=m and $ terminates the string
            // but, if we consider only a string of type T1#T2, then we can use n+m-1 = 2n-1
            // now, the last (0-based) position is 2n-2 and since it is 0-based, the last character is 2n-2-0, the second last character is 2n-2-1=2n-3, the k-th last character is 2n-1-k
            // if min_ij starts at position s, then max_ij shall end at 2n-1-s
            // therefore i shall check that max_ij + lcp - 1 = 2n - 1 - min_ij
            // therefore i shall check that max_ij = 2n - min_ij - lcp
            if (max_ij == _2n - min_ij - lcp){
              if (lcp > palindromes.length) {
                palindromes.length = lcp;
                palindromes.starts.clear();
              }
              assert(lcp == palindromes.length);
              palindromes.starts.push_back(min_ij);
              #ifdef  DEBUG
                std::cout << "OK -> " << max_ij << " == " << _2n << " - " << min_ij << " - " << lcp << " = " << _2n - min_ij - lcp << std::endl;
              #endif//DEBUG
            } else {
              #ifdef  DEBUG
                std::cout << "KO -> " << max_ij << " != " << _2n << " - " << min_ij << " - " << lcp << " = " << _2n - min_ij - lcp << std::endl;
              #endif//DEBUG
            }
          // } else {
          // #ifdef  DEBUG
          //   std::cout << "KO: BWT[" << i+1 << "] == BWT[" << j+1 << "] :: " << burrows_wheeler_transform.symbols[i] << " == " << burrows_wheeler_transform.symbols[j] << std::endl;
          // #endif//DEBUG
          // }
        } else {
        #ifdef  DEBUG
          std::cout << "KO: " << max_ij << " < " << separator_position << " < " << min_ij << std::endl;
        #endif//DEBUG
        }
      }
    }
  }
}

void bottom_up_traversal(const LCPArray& lcp_array, std::function<void (const Interval&)> process) {
  intmax_t n = lcp_array.lcps.size();
  std::vector<Interval> stack = {};
  stack.push_back(Interval {
    .length = 0,
    .lb = 0,
    .rb = n-1
  });
  // std::cout << "PUSH " << stack.back() << std::endl;
  #ifdef  ENABLE_TELEMETRY
    TELEMETRY.push_count++;
  #endif//ENABLE_TELEMETRY

  for (intmax_t q = 2; q < n; ++q) {
    // std::cout << lcp_array.lcps[q] << (lcp_array.lcps[q] > lcp_array.lcps[q - 1] ? " > " : (lcp_array.lcps[q] < lcp_array.lcps[q - 1] ? " < " : " = ")) << lcp_array.lcps[q - 1] << std::endl;
    if (lcp_array.lcps[q] > lcp_array.lcps[q - 1]) {
      stack.push_back(Interval {
        .length = lcp_array.lcps[q],
        .lb = q - 1,
        .rb = 0
      });
      #ifdef  ENABLE_TELEMETRY
        TELEMETRY.push_count++;
      #endif//ENABLE_TELEMETRY
      // std::cout << "PUSH " << stack.back() << std::endl;
    } else if (lcp_array.lcps[q] < lcp_array.lcps[q - 1]) {
      assert (!stack.empty());
      intmax_t lb = q - 1;
      while (stack.back().length > lcp_array.lcps[q]) {
        assert (!stack.empty());
        Interval interval = stack.back();
        stack.pop_back();
        #ifdef  ENABLE_TELEMETRY
          TELEMETRY.pop_count++;
        #endif//ENABLE_TELEMETRY
        interval.rb = q - 1;
        lb = interval.lb;
        process(interval);
      }
      
      assert (!stack.empty());
      if (stack.back().length != lcp_array.lcps[q]) {
        stack.push_back(Interval {
          .length = lcp_array.lcps[q],
          .lb = lb,
          .rb = 0
        });
        // std::cout << "PUSH " << stack.back() << std::endl;
        #ifdef  ENABLE_TELEMETRY
          TELEMETRY.push_count++;
        #endif//ENABLE_TELEMETRY
      }
    }
  }

  while (!stack.empty()) {
    Interval interval = stack.back();
    stack.pop_back();
    #ifdef  ENABLE_TELEMETRY
      TELEMETRY.pop_count++;
    #endif//ENABLE_TELEMETRY
    interval.rb = n-1;
    process(interval);
  }
}

Palindromes please_mummy_find_the_longest_palindromes_for_me(
    const LCPArray& lcp_array,
    const SuffixArray& suffix_array,
    intmax_t separator_position) {
  Palindromes palindromes = {
    .length = 0,
    .starts = {}
  };
  auto process = [&suffix_array, &separator_position, &palindromes](const Interval& interval){
    evaluate_lcp_interval(suffix_array, separator_position, palindromes, interval);
  };
  bottom_up_traversal(lcp_array, process);
  return palindromes;
}


void Telemetry::start() {
  this->push_count = 0;
  this->pop_count = 0;
  this->interval_count = 0;
  this->interval_size = 0;
  this->comparison_count = 0;
  this->elapsed_time = 0;
  this->timer_start = std::chrono::high_resolution_clock::now();
}

void Telemetry::end() {
  this->interval_size = this->interval_size / this->interval_count;
  this->timer_end = std::chrono::high_resolution_clock::now();
  this->elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(this->timer_end - this->timer_start).count();
}

Telemetry TELEMETRY;
