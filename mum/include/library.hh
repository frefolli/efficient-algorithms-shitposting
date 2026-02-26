#ifndef LIBRARY
#define LIBRARY
#include <cassert>
#include <cstdint>
#include <functional>
#include <ostream>
#include <stdint.h>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>

struct Alphabet {
  std::string symbols;

  Alphabet(const std::string& characters, char terminal, char separator);

  intmax_t terminal() const;

  intmax_t separator() const;

  std::string_view characters() const;

  intmax_t size() const;

  intmax_t sample() const;

  std::ostream& to_markdown(std::ostream& out) const;
};

struct Text {
  std::vector<intmax_t> symbols;

  static Text from(const Alphabet& alphabet, const std::string& raw_text);

  static Text sequence(const Alphabet& alphabet, intmax_t min_length, intmax_t max_length);

  intmax_t back() const;

  intmax_t size() const;

  std::ostream& to_markdown(std::ostream& out, const Alphabet& alphabet) const;

  std::string slice_suffix(const Alphabet& alphabet, intmax_t start_idx) const;

  Text invert() const;

  Text join(const Text& other, const Alphabet& alphabet) const;
};

struct SuffixArray {
  std::vector<intmax_t> indexes;

  static SuffixArray build(const Text& text);
};

struct BurrowsWheelerTransform {
  std::vector<intmax_t> symbols;

  static BurrowsWheelerTransform build(const Text& text, const SuffixArray& suffix_array);
};

struct FMIndex {
  std::vector<intmax_t> C;
  std::vector<std::vector<intmax_t>> Occ;

  static FMIndex build(const Alphabet& alphabet, const BurrowsWheelerTransform& burrows_wheeler_transform);

  std::ostream& to_markdown(std::ostream& out, const Alphabet& alphabet);
};

struct LCPArray {
  std::vector<intmax_t> lcps;

  static LCPArray build(const Text& text, const SuffixArray& suffix_array);
};

template <typename T>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec) {
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
  intmax_t q;
  intmax_t i, j;

  LocalMaximumIterator(const LCPArray& lcp_array);

  bool has_next();

  std::pair<intmax_t, intmax_t> next();
};

struct GlobalMaximumIterator {
  LocalMaximumIterator local_maximum_iterator;
  BurrowsWheelerTransform& burrows_wheeler_transform;
  intmax_t i, j;

  GlobalMaximumIterator(const LCPArray& lcp_array,
                        BurrowsWheelerTransform& burrows_wheeler_transform);

  bool has_next();

  std::pair<intmax_t, intmax_t> next();
};

struct MaximalUniqueMatchesIterator {
  GlobalMaximumIterator global_maximum_iterator;
  const SuffixArray& suffix_array;
  intmax_t separator_position;
  intmax_t i, j;

  MaximalUniqueMatchesIterator(const LCPArray& lcp_array,
                               BurrowsWheelerTransform& burrows_wheeler_transform,
                               const SuffixArray& suffix_array,
                               intmax_t separator_position);

  bool has_next();

  std::pair<intmax_t, intmax_t> next();
};

struct MaximalPalindromeMatchesIterator {
  LocalMaximumIterator local_maximum_iterator;
  const SuffixArray& suffix_array;
  const BurrowsWheelerTransform& burrows_wheeler_transform;
  intmax_t separator_position;
  intmax_t i, j;

  MaximalPalindromeMatchesIterator(const LCPArray& lcp_array,
                                   const BurrowsWheelerTransform& burrows_wheeler_transform,
                                   const SuffixArray& suffix_array,
                                   intmax_t separator_position);

  bool has_next();

  std::pair<intmax_t, intmax_t> next();
};

struct Interval {
  intmax_t length;
  intmax_t lb;
  intmax_t rb;
};

struct Palindromes {
  intmax_t length;
  std::vector<intmax_t> starts;
};

std::ostream& operator<<(std::ostream& out, const Interval& interval);

void evaluate_lcp_interval(const SuffixArray& suffix_array,
                           intmax_t separator_position,
                           Palindromes& palindromes,
                           const Interval& interval);

void bottom_up_traversal(const LCPArray& lcp_array, std::function<void (const Interval&)> process);

Palindromes please_mummy_find_the_longest_palindromes_for_me(const LCPArray& lcp_array,
                                                             const SuffixArray& suffix_array,
                                                             intmax_t separator_position);

struct Telemetry {
  intmax_t push_count;
  intmax_t pop_count;
  intmax_t interval_count;
  intmax_t interval_size;
  intmax_t comparison_count;
  intmax_t elapsed_time;
  std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> timer_start;
  std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> timer_end;

  void start();
  void end();
};

extern Telemetry TELEMETRY;
#endif//LIBRARY
