#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <ostream>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

struct Alphabet {
  std::string symbols;

  Alphabet(const std::string& characters, char terminal, char separator) {
    this->symbols = "";
    assert(!characters.empty());
    assert(!characters.contains(terminal));
    assert(!characters.contains(separator));
    assert(terminal != separator);
    this->symbols.push_back(terminal);
    this->symbols.push_back(separator);
    this->symbols.append(characters);
  }

  inline uintmax_t terminal() const {
    return 0;
  }

  inline uintmax_t separator() const {
    return 1;
  }

  inline std::string_view characters() const {
    return std::string_view(symbols).substr(2);
  }

  inline uintmax_t size() const {
    return this->symbols.size();
  }

  inline uintmax_t sample() const {
    return (std::rand() % this->characters().size()) + 2;
  }

  inline std::ostream& to_markdown(std::ostream& out) const {
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
};

struct Text {
  std::vector<uintmax_t> symbols;

  inline static Text from(const Alphabet& alphabet, const std::string& raw_text) {
    Text text;
    text.symbols.resize(raw_text.size());
    for (uintmax_t idx = 0; idx < raw_text.size(); ++idx) {
      uintmax_t char_idx = alphabet.symbols.find(raw_text[idx]);
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

  inline static Text sequence(const Alphabet& alphabet, uintmax_t min_length, uintmax_t max_length) {
    assert(max_length >= min_length);
    uintmax_t delta_length = max_length - min_length;
    uintmax_t length = min_length + (std::rand() % delta_length);
    Text text;
    text.symbols.resize(length + 1);
    for (uintmax_t idx = 0; idx < length; ++idx) {
      text.symbols[idx] = alphabet.sample();
    }
    text.symbols[length] = alphabet.terminal();
    return text;
  }

  inline uintmax_t back() const {
    return symbols.back();
  }

  inline uintmax_t size() const {
    return symbols.size();
  }

  inline std::ostream& to_markdown(std::ostream& out, const Alphabet& alphabet) const {
    for (uintmax_t char_idx : this->symbols) {
      out << alphabet.symbols[char_idx];
    }
    return out;
  }

  std::string slice_suffix(const Alphabet& alphabet, uintmax_t start_idx) const {
    std::string result = "";
    for (uintmax_t idx = start_idx; idx < symbols.size(); ++idx) {
      result += alphabet.symbols[symbols[idx]];
    }
    return result;
  }

  inline Text invert() const {
    Text text;
    text.symbols.resize(this->symbols.size());
    for (uintmax_t idx = 0; idx < this->symbols.size() - 1; ++idx) {
      text.symbols[text.symbols.size() - 2 - idx] = this->symbols[idx];
    }
    text.symbols[text.symbols.size() - 1] = this->symbols.back();
    return text;
  }

  inline Text join(const Text& other, const Alphabet& alphabet) const {
    Text text;
    text.symbols.resize(this->symbols.size() + other.symbols.size());
    for (uintmax_t idx = 0; idx < this->symbols.size(); ++idx) {
      text.symbols[idx] = this->symbols[idx];
    }
    text.symbols[this->symbols.size() - 1] = alphabet.separator();
    for (uintmax_t idx = 0; idx < other.symbols.size(); ++idx) {
      text.symbols[idx + this->symbols.size()] = other.symbols[idx];
    }
    return text;
  }
};

struct SuffixArray {
  std::vector<uintmax_t> indexes;

  inline static SuffixArray build(const Text& text) {
    SuffixArray suffix_array;
    uintmax_t length = text.symbols.size();
    suffix_array.indexes.resize(length);

    for (uintmax_t idx = 0; idx < length; ++idx) {
      suffix_array.indexes[idx] = idx;
    }

    std::sort(suffix_array.indexes.begin(), suffix_array.indexes.end(), [&text, &length](uintmax_t i, uintmax_t j) {
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
};

struct BurrowsWheelerTransform {
  std::vector<uintmax_t> symbols;

  inline static BurrowsWheelerTransform build(const Text& text, const SuffixArray& suffix_array) {
    BurrowsWheelerTransform burrows_wheeler_transform;
    uintmax_t length = text.symbols.size();
    burrows_wheeler_transform.symbols.resize(length);

    for (uintmax_t idx = 0; idx < length; ++idx) {
      uintmax_t charidx = (suffix_array.indexes[idx] > 0) ? (suffix_array.indexes[idx] - 1) : (text.size() - 1);
      burrows_wheeler_transform.symbols[idx] = text.symbols[charidx];
    }

    return burrows_wheeler_transform;
  }
};

struct FMIndex {
  std::vector<uintmax_t> C;
  std::vector<std::vector<uintmax_t>> Occ;

  inline static FMIndex build(const Alphabet& alphabet, const BurrowsWheelerTransform& burrows_wheeler_transform) {
    uintmax_t alphabet_length = alphabet.size();
    uintmax_t bwt_length = burrows_wheeler_transform.symbols.size();

    FMIndex fm_index;
    fm_index.C.resize(alphabet_length);
    fm_index.Occ.resize(alphabet_length);
    for (uintmax_t char_idx = 0; char_idx < alphabet_length; ++char_idx) {
      fm_index.Occ[char_idx].resize(bwt_length);
    }

    for (uintmax_t symbol_idx = 0; symbol_idx < bwt_length; ++symbol_idx) {
      uintmax_t current_char_idx = burrows_wheeler_transform.symbols[symbol_idx];
      for (uintmax_t higher_char_idx = current_char_idx + 1; higher_char_idx < alphabet_length; ++higher_char_idx) {
        fm_index.C[higher_char_idx]++;
      }
      if (symbol_idx < bwt_length - 1) {
        for (uintmax_t char_idx = 0; char_idx < alphabet_length; ++char_idx) {
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

  inline std::ostream& to_markdown(std::ostream& out, const Alphabet& alphabet) {
    uintmax_t alphabet_length = alphabet.size();
    uintmax_t bwt_length = Occ.front().size();

    out << "| $\\sigma$ | $C[\\sigma]$ |" << std::endl;
    out << "| -------- | ----------- |" << std::endl;
    for (uintmax_t char_idx = 0; char_idx < alphabet_length; ++char_idx) {
      out << "|" << alphabet.symbols[char_idx] << "|" << C[char_idx] << "|" << std::endl;
    }
    out << std::endl;

    out << "| $i$ |";
    for (uintmax_t char_idx = 0; char_idx < alphabet_length; ++char_idx) {
      out << " $Occ[$`" << alphabet.symbols[char_idx] << "`$]$ |";
    }
    out << std::endl;
    out << "| --- |";
    for (uintmax_t char_idx = 0; char_idx < alphabet_length; ++char_idx) {
      out << " -------- |";
    }
    out << std::endl;
    for (uintmax_t symbol_idx = 0; symbol_idx < bwt_length; ++symbol_idx) {
      out << "| " << symbol_idx + 1 << " |";
      for (uintmax_t char_idx = 0; char_idx < alphabet_length; ++char_idx) {
        out << " " << Occ[char_idx][symbol_idx] << "|";
      }
      out << std::endl;
    }
    return out;
  }
};

struct LCPArray {
  std::vector<uintmax_t> lcps;

  inline static LCPArray build(const Text& text, const SuffixArray& suffix_array) {
    uintmax_t length = text.size();
    LCPArray lcp_array = {};
    lcp_array.lcps.resize(length);
    
    std::vector<uintmax_t> rank;
    rank.resize(length);
    for (uintmax_t idx = 0; idx < length; ++idx) {
      rank[suffix_array.indexes[idx]] = idx;
    }
    uintmax_t h = 0;
    for (uintmax_t idx = 0; idx < length; ++idx) {
      if (rank[idx] > 0) {
        uintmax_t previous = (rank[idx] > 0) ? (rank[idx] - 1) : (length-1);
        uintmax_t k = suffix_array.indexes[previous];
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
};

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec) {
  out << "[";
  bool first = true;
  for (const T& t : vec) {
    if (first) {
      first = false;
    } else {
      out << " ";
    };
    out << t;
  }
  out << "]";
  return out;
}

struct LocalMaximumIterator {
  const LCPArray& lcp_array;
  uintmax_t q;
  intmax_t i, j;

  LocalMaximumIterator(const LCPArray& lcp_array) : lcp_array(lcp_array) {
    this->q = 1;
    this->i = -1;
    this->j = -1;
  }
  
  inline bool has_next() {
    uintmax_t length = lcp_array.lcps.size();
    i = -1;
    j = -1;
    while (q < length) {
      if (lcp_array.lcps[q - 1] < lcp_array.lcps[q]) {
        i = q - 1;
        j = -1;
      } else if (lcp_array.lcps[q - 1] > lcp_array.lcps[q]) {
        j = q - 1;
      }

      if (i != -1 && j != -1) {
        break;
      }
      q++;
    }
    return (i != -1 && j != -1);
  }

  std::pair<uintmax_t, uintmax_t> next() {
    assert(i != -1 && j != -1);
    std::pair<uintmax_t, uintmax_t> res = {i, j};
    i = -1;
    j = -1;
    return res;
  }
};

struct GlobalMaximumIterator {
  LocalMaximumIterator local_maximum_iterator;
  BurrowsWheelerTransform& burrows_wheeler_transform;
  intmax_t i, j;

  GlobalMaximumIterator(const LCPArray& lcp_array,
                        BurrowsWheelerTransform& burrows_wheeler_transform)
    : local_maximum_iterator(lcp_array),
      burrows_wheeler_transform(burrows_wheeler_transform) {
    this->i = -1;
    this->j = -1;
  }

  inline bool has_next() {
    this->i = -1;
    this->j = -1;
    if (!local_maximum_iterator.has_next()) {
      return false;
    }
    auto local_maximum = local_maximum_iterator.next();
    uintmax_t ci = local_maximum.first;
    uintmax_t cj = local_maximum.second;

    bool ok = true;
    for (uintmax_t a = ci; a <= cj; ++a) {
      for (uintmax_t b = a + 1; b <= cj; ++b) {
        assert(a != b);
        if (burrows_wheeler_transform.symbols[a] == burrows_wheeler_transform.symbols[b]) {
          ok = false;
        }

        if (!ok) {
          break;
        }
      }

      if (!ok) {
        break;
      }
    }

    if (!ok) {
      i = -1;
      j = -1;
    } else {
      i = ci;
      j = cj;
    }

    return ok;
  }

  std::pair<uintmax_t, uintmax_t> next() {
    assert(i != -1 && j != -1);
    std::pair<uintmax_t, uintmax_t> res = {i, j};
    i = -1;
    j = -1;
    return res;
  }
};

struct MaximalUniqueMatchesIterator {
  GlobalMaximumIterator global_maximum_iterator;
  const SuffixArray& suffix_array;
  uintmax_t separator_position;
  intmax_t i, j;

  MaximalUniqueMatchesIterator(const LCPArray& lcp_array,
                               BurrowsWheelerTransform& burrows_wheeler_transform,
                               const SuffixArray& suffix_array,
                               uintmax_t separator_position)
    : global_maximum_iterator(lcp_array, burrows_wheeler_transform),
      suffix_array(suffix_array) {
    this->separator_position = separator_position;
    this->i = -1;
    this->j = -1;
  }

  bool has_next() {
    this->i = -1;
    this->j = -1;

    bool ok = false;
    while (global_maximum_iterator.has_next()) {
      auto global_maximum = global_maximum_iterator.next();
      if (global_maximum.second != global_maximum.first + 1) {
        continue;
      }
      uintmax_t ci = global_maximum.first;
      uintmax_t cj = global_maximum.second;

      uintmax_t min_ij = std::min(suffix_array.indexes[ci], suffix_array.indexes[cj]);
      uintmax_t max_ij = std::max(suffix_array.indexes[ci], suffix_array.indexes[cj]);

      if (min_ij < separator_position && max_ij > separator_position) {
        i = ci;
        j = cj;
        ok = true;
        break;;
      }
    }

    return ok;
  }

  std::pair<uintmax_t, uintmax_t> next() {
    assert(i != -1 && j != -1);
    std::pair<uintmax_t, uintmax_t> res = {i, j};
    i = -1;
    j = -1;
    return res;
  }
};

int main() {
  std::srand(170701);
  // Alphabet alphabet = Alphabet("acgt", '$', '#');
  // Text text = Text::from(alphabet, "ggtcagtc$");
  // Alphabet alphabet = Alphabet("acgt", '$', '#');
  // Text t1 = Text::from(alphabet, "acaaacatat$");
  // Text t2 = t1.invert();
  Alphabet alphabet = Alphabet("abcd", '$', '#');
  Text t1 = Text::from(alphabet, "abcdaaaa$");
  Text t2 = Text::from(alphabet, "bbbabcd$");
  Text text = t1.join(t2, alphabet);
  SuffixArray suffix_array = SuffixArray::build(text);
  BurrowsWheelerTransform burrows_wheeler_transform = BurrowsWheelerTransform::build(text, suffix_array);
  FMIndex fm_index = FMIndex::build(alphabet, burrows_wheeler_transform);
  LCPArray lcp_array = LCPArray::build(text, suffix_array);

  std::ofstream file ("README.md");

  file << "## Alphabet" << std::endl;
  alphabet.to_markdown(file << "$\\Sigma$ := ") << std::endl;
  file << std::endl;
  file << std::endl;

  file << "## TextS" << std::endl;
  t1.to_markdown(file << "$T_1$ := ", alphabet) << std::endl;
  file << std::endl;
  t2.to_markdown(file << "$T_2$ := ", alphabet) << std::endl;
  file << std::endl;

  file << "## Text" << std::endl;
  text.to_markdown(file << "T := ", alphabet) << std::endl;
  file << std::endl;

  file << "## Local Maximums" << std::endl;
  LocalMaximumIterator local_maximum_iterator = LocalMaximumIterator(lcp_array);
  while (local_maximum_iterator.has_next()) {
    auto ij = local_maximum_iterator.next();
    file << "### [" << ij.first+1 << "," << ij.second+1 << "]" << std::endl;
    uintmax_t lcp = lcp_array.lcps[ij.first + 1];
    std::string match = text.slice_suffix(alphabet, suffix_array.indexes[ij.first + 1]).substr(0, lcp);
    file << "lcp := " << lcp << std::endl;
    file << "match := `" << match << "`" << std::endl;
  }

  file << "## Global Maximums" << std::endl;
  GlobalMaximumIterator global_maximum_iterator = GlobalMaximumIterator(lcp_array, burrows_wheeler_transform);
  while (global_maximum_iterator.has_next()) {
    auto ij = global_maximum_iterator.next();
    file << "### [" << ij.first+1 << "," << ij.second+1 << "]" << std::endl;
    uintmax_t lcp = lcp_array.lcps[ij.first + 1];
    std::string match = text.slice_suffix(alphabet, suffix_array.indexes[ij.first + 1]).substr(0, lcp);
    file << "lcp := " << lcp << std::endl;
    file << "match := `" << match << "`" << std::endl;
  }

  file << "## Maximal Unique Matches" << std::endl;
  MaximalUniqueMatchesIterator maximal_unique_matches_iterator = MaximalUniqueMatchesIterator(lcp_array, burrows_wheeler_transform, suffix_array, t1.size());
  while (maximal_unique_matches_iterator.has_next()) {
    auto ij = maximal_unique_matches_iterator.next();
    file << "### [" << ij.first+1 << "," << ij.second+1 << "]" << std::endl;
    uintmax_t lcp = lcp_array.lcps[ij.first + 1];
    std::string match = text.slice_suffix(alphabet, suffix_array.indexes[ij.first + 1]).substr(0, lcp);
    file << "lcp := " << lcp << std::endl;
    file << "match := `" << match << "`" << std::endl;
  }
  
  file << "## Suffix Array, LCP and BWT" << std::endl;
  file << "| `#` | SA  | LCP |  B  | SUFFIX |" << std::endl;
  file << "| --- | --- | --- | --- | ------ |" << std::endl;
  for (uintmax_t idx = 0; idx < text.size(); ++idx) {
    file
      << "|" << idx+1
      << "|" << suffix_array.indexes[idx] + 1
      << "|" << (intmax_t)(lcp_array.lcps[idx])
      << "|"  << alphabet.symbols[burrows_wheeler_transform.symbols[idx]]
      << "|" << "`" << text.slice_suffix(alphabet, suffix_array.indexes[idx]) << "`"
      << "|" << std::endl;
  }
  file << std::endl;

  file << "## FM Index" << std::endl;
  fm_index.to_markdown(file, alphabet);
  file.close();
  return 0;
}
