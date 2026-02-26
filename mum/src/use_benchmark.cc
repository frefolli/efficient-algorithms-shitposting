#include <cassert>
#include <fstream>
#include <istream>
#include <string>
#include <vector>
#include <algorithm>
#include <library.hh>

struct Test {
  std::string text;
  std::vector<std::string> palindromes;
  uintmax_t length;

  inline static Test from(std::istream& in) {
    std::string arg;
    std::string text;
    std::vector<std::string> palindromes;

    in >> arg;
    assert(arg == "##");
    in >> text;
    assert(text.size() > 0);

    in >> arg;
    assert(arg.size() > 2 && arg.front() == '$' && arg.back() == '$');
    uintmax_t length = std::stoul(arg.substr(1, arg.size() - 2));
    assert(length > 0);

    for (uintmax_t idx = 0; idx < length; ++idx) {
      in >> arg;
      assert(arg == "-");
      in >> arg;
      assert(arg.size() > 0);

      assert(palindromes.size() == 0 || arg.size() == palindromes.back().size());
      palindromes.push_back(arg);
    }
    return Test {
      .text = text,
        .palindromes = palindromes,
        .length = palindromes.back().size()
    };
  }
}; 

struct Benchmark {
  std::string alphabet;
  std::vector<Test> tests;

  inline static Benchmark from(const std::string& filepath) {
    std::vector<Test> tests;
    std::string arg;
    std::ifstream file (filepath);
    std::string alphabet;
    uintmax_t benchmark_size;

    file >> arg;
    assert(arg == "#");
    file >> alphabet;
    assert(alphabet.size() > 0);

    file >> arg;
    assert(arg.size() > 2 && arg.front() == '$' && arg.back() == '$');
    benchmark_size = std::stoul(arg.substr(1, arg.size() - 2));
    assert(benchmark_size > 0);

    for (uintmax_t benchmark_entry = 0; benchmark_entry < benchmark_size; ++benchmark_entry) {
      tests.push_back(Test::from(file));
    }

    file.close();
    return Benchmark {
      .alphabet = alphabet,
      .tests = tests
    };
  }
};

int main() {
  Benchmark benchmark = Benchmark::from("./out.benchmark.md");

  std::string BOTH = "✅";
  std::string MUM_ONLY = "⚠️";
  std::string PAL_ONLY = "❌";
  std::string BOTH_IS_EMPTY_ALERT = "NIGGER ALERT";

  std::ofstream file ("./out.report.md");
  Alphabet alphabet = Alphabet(benchmark.alphabet, '$', '#');

  for (auto test : benchmark.tests) {
    Text t1 = Text::from(alphabet, test.text);
    Text t2 = t1.invert();
    Text text = t1.join(t2, alphabet);
    SuffixArray suffix_array = SuffixArray::build(text);
    BurrowsWheelerTransform burrows_wheeler_transform = BurrowsWheelerTransform::build(text, suffix_array);
    FMIndex fm_index = FMIndex::build(alphabet, burrows_wheeler_transform);
    LCPArray lcp_array = LCPArray::build(text, suffix_array);

    
    intmax_t separator_position = t1.size() - 1;
    auto found_palindromes =
      please_mummy_find_the_longest_palindromes_for_me(lcp_array,
                                                       suffix_array,
                                                       separator_position);

    std::vector<std::string> palindromes = {};
    for (auto start : found_palindromes.starts) {
      palindromes.push_back(text.slice_suffix(alphabet, start).substr(0, found_palindromes.length));
    }

    std::vector<std::string> both = {};
    std::vector<std::string> mum_only = {};
    std::vector<std::string> pal_only = {};

    for (auto palindrome : palindromes) {
      if (std::find(test.palindromes.begin(), test.palindromes.end(), palindrome) != test.palindromes.end()) {
        if (std::find(both.begin(), both.end(), palindrome) == both.end()) {
          both.push_back(palindrome);
        }
      } else {
        if (std::find(mum_only.begin(), mum_only.end(), palindrome) == mum_only.end()) {
          mum_only.push_back(palindrome);
        }
      }
    }

    for (auto palindrome : test.palindromes) {
      if (std::find(palindromes.begin(), palindromes.end(), palindrome) != palindromes.end()) {
        if (std::find(both.begin(), both.end(), palindrome) == both.end()) {
          both.push_back(palindrome);
        }
      } else {
        if (std::find(pal_only.begin(), pal_only.end(), palindrome) == pal_only.end()) {
          pal_only.push_back(palindrome);
        }
      }
    }

    file << "# " << test.text << std::endl;
    if (both.size() == 0) {
      file << BOTH_IS_EMPTY_ALERT << std::endl;
    }
    file << "| STATUS | STRING |" << std::endl;
    file << "| --- | --- |" << std::endl;
    for (auto string : mum_only) {
      file << "| " << MUM_ONLY << " | " << string << " |" << std::endl;
    }
    for (auto string : both) {
      file << "| " << BOTH << " | " << string << " |" << std::endl;
    }
    for (auto string : pal_only) {
      file << "| " << PAL_ONLY << " | " << string << " |" << std::endl;
    }
  }
  file.close();
}
