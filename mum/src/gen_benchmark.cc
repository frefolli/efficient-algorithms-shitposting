#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <random>
#include <vector>
#include <string>
#include <cstdint>
#include <iostream>

struct Report {
  std::vector<uint32_t> spreads_of_even_palindromes;
  std::vector<uint32_t> spreads_of_odd_palindromes;
  uint32_t spreadest_even;
  uint32_t spreadest_odd;

  static Report from(const std::string& text) {
    Report report;
    report.spreads_of_even_palindromes.resize(text.size());
    report.spreads_of_odd_palindromes.resize(text.size());
    report.spreadest_even = 0;
    report.spreadest_odd = 0;
    
    int32_t length = text.size();
    for (int32_t idx = 0; idx < length; ++idx) {
      int32_t even = 0;
      while (idx - even - 1 >= 0 && idx + even < length && text[idx - even - 1] == text[idx + even]) {
        even++;
      }
      
      int32_t odd = 0;
      while (idx - odd  - 1>= 0 && idx + odd + 1< length && text[idx - odd - 1] == text[idx + odd + 1]) {
        odd++;
      }

      report.spreads_of_even_palindromes[idx] = even;
      report.spreads_of_odd_palindromes[idx] = odd;
      report.spreadest_even = std::max(report.spreadest_even, report.spreads_of_even_palindromes[idx]);
      report.spreadest_odd = std::max(report.spreadest_odd, report.spreads_of_odd_palindromes[idx]);
    }

    return report;
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

std::vector<std::string> extract_longest_palindromes(const std::string& text, const Report& report) {
  std::vector<std::string> output = {};
  uint32_t length = text.size();
  uint32_t length_even = report.spreadest_even * 2;
  uint32_t length_odd = report.spreadest_odd * 2 + 1;
  if (length_even >= length_odd) {
    for (uint32_t idx = 0; idx < length; ++idx) {
      if (report.spreads_of_even_palindromes[idx] == report.spreadest_even) {
        uint32_t start = idx - report.spreadest_even;
        uint32_t length = report.spreadest_even * 2;
        output.push_back(text.substr(start, length));
      }
    }
  }

  if (length_odd >= length_even) {
    for (uint32_t idx = 0; idx < length; ++idx) {
      if (report.spreads_of_odd_palindromes[idx] == report.spreadest_odd) {
        uint32_t start = idx - report.spreadest_odd;
        uint32_t length = report.spreadest_odd * 2 + 1;
        output.push_back(text.substr(start, length));
      }
    }
  }
  return output;
}

std::string sample_string(const std::string& alphabet, uint32_t length) {
  std::random_device random_device;
  std::mt19937 generator(random_device());
  std::uniform_int_distribution<> char_distribution(0, alphabet.size() - 1);
  std::string result;
  result.resize(length);
  for (uint32_t idx = 0; idx < length; ++idx) {
    result[idx] = alphabet[char_distribution(generator) % alphabet.size()];
  }
  return result;
}

void print_help(const char* progname) {
  std::cout << "Usage: " << progname << " [-h] [-l <INT>] [-L <INT>] [-s <INT>] [-a <STR>]" << std::endl;
  std::cout << "Arguments" << std::endl;
  std::cout << "    -h, --help                                             Print this Help screen" << std::endl;
  std::cout << "    -l, --min-length <INT>                                 Set minimum text length" << std::endl;
  std::cout << "    -L, --max-length <INT>                                 Set maximum text length" << std::endl;
  std::cout << "    -s, --benchmark-size <INT>                             Set benchmark size" << std::endl;
  std::cout << "    -a, --alphabet <STR>                                   Set benchmark alphabet" << std::endl;
  exit(0);
}

int main(int argc, char** argv) {
  uint32_t min_length = 10;
  uint32_t max_length = 50;
  uint32_t benchmark_size = 100;
  std::string alphabet = "acgt";

  int argi = 1;
  const char* arg = NULL;
  while (argi < argc) {
    arg = argv[argi++];
    if (strcmp("--help", arg) == 0 || strcmp("-h", arg) == 0) {
      print_help(argv[0]);
    } else if (strcmp("--max-length", arg) == 0 || strcmp("-L", arg) == 0) {
      arg = argv[argi++];
      max_length = std::stoul(arg);
    } else if (strcmp("--min-length", arg) == 0 || strcmp("-l", arg) == 0) {
      arg = argv[argi++];
      min_length = std::stoul(arg);
    } else if (strcmp("--benchmark-size", arg) == 0 || strcmp("-s", arg) == 0) {
      arg = argv[argi++];
      benchmark_size = std::stoul(arg);
    } else if (strcmp("--alphabet", arg) == 0 || strcmp("-a", arg) == 0) {
      arg = argv[argi++];
      alphabet = arg;
    } else {
      std::cerr << "Unhandled argument '" << arg << "'" << std::endl;
      exit(1);
    }
  }

  if (min_length > max_length) {
    std::cerr << "Min length must be less than or equal to max length" << std::endl;
    exit(1);
  }

  std::random_device random_device;
  std::mt19937 generator(random_device());
  std::uniform_int_distribution<> length_distribution(min_length, max_length);
  
  std::ofstream file ("./out.benchmark.md");
  file << "# " << alphabet << std::endl;
  file << "$" << benchmark_size << "$" << std::endl;
  for (uint32_t benchmark_entry = 0; benchmark_entry < benchmark_size; ++benchmark_entry) {
    uint32_t length = length_distribution(generator);
    std::string text = sample_string(alphabet, length);
    Report report = Report::from(text);
    std::vector<std::string> palindromes = extract_longest_palindromes(text, report);

    file << "## " << text << std::endl;
    file << "$" << palindromes.size() << "$" << std::endl;
    for (auto string : palindromes) {
      file << "- " << string << std::endl;
    }
  }
  file.close();
}
