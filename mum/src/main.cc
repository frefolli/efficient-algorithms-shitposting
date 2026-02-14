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

  std::string slice_suffix(const Alphabet& alphabet, uintmax_t start_idx) {
    std::string result = "";
    for (uintmax_t idx = start_idx; idx < symbols.size(); ++idx) {
      result += alphabet.symbols[symbols[idx]];
    }
    return result;
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

int main() {
  std::srand(170701);
  // Alphabet alphabet = Alphabet("acgt", '$', '#');
  // Text text = Text::from(alphabet, "ggtcagtc$");
  Alphabet alphabet = Alphabet("acgt", '$', '#');
  Text text = Text::from(alphabet, "acaaacatat$");
  SuffixArray suffix_array = SuffixArray::build(text);
  BurrowsWheelerTransform burrows_wheeler_transform = BurrowsWheelerTransform::build(text, suffix_array);
  FMIndex fm_index = FMIndex::build(alphabet, burrows_wheeler_transform);
  LCPArray lcp_array = LCPArray::build(text, suffix_array);

  std::ofstream file ("README.md");

  file << "## Alphabet" << std::endl;
  alphabet.to_markdown(file << "$\\Sigma$ := ") << std::endl;
  file << std::endl;
  file << std::endl;

  file << "## Text" << std::endl;
  text.to_markdown(file << "T := ", alphabet) << std::endl;
  file << std::endl;
  
  file << "## Suffix Array, LCP and BWT" << std::endl;
  file << "| SA  | LCP |  B  | SUFFIX |" << std::endl;
  file << "| --- | --- | --- | ------ |" << std::endl;
  for (uintmax_t idx = 0; idx < text.size(); ++idx) {
    file
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
