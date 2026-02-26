#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <ostream>
#include <random>
#include <vector>
#include <string>
#include <cstdint>
#include <iostream>
#include <library.hh>

std::string sample_string(std::mt19937& generator, const Alphabet& alphabet, intmax_t length) {
  std::uniform_int_distribution<> char_distribution(0, alphabet.characters().size() - 1);
  std::string result;
  result.resize(length);
  for (intmax_t idx = 0; idx < length; ++idx) {
    result[idx] = alphabet.symbols[char_distribution(generator) + 2];
  }
  return result;
}

void print_help(const char* progname) {
  std::cout << "Usage: " << progname << " [-h] [-Tl/-TL <INT>] [-Al/-AL <INT>] [-b <INT>] [-s <INT>]" << std::endl;
  std::cout << "Arguments" << std::endl;
  std::cout << "    -h, --help                                             Print this Help screen" << std::endl;
  std::cout << "    -Tl, --min-text-length <INT>                           Set minimum text length" << std::endl;
  std::cout << "    -TL, --max-text-length <INT>                           Set maximum text length" << std::endl;
  std::cout << "    -Al, --min-alphabet-length <INT>                       Set minimum alphabet length" << std::endl;
  std::cout << "    -AL, --min-alphabet-length <INT>                       Set maximum alphabet length" << std::endl;
  std::cout << "    -b, --batch-size <INT>                                 Set batch size" << std::endl;
  std::cout << "    -s, --seed <INT>                                       Set seed" << std::endl;
  exit(0);
}

int main(int argc, char** argv) {
  intmax_t min_text_length = 1;
  intmax_t max_text_length = 100;
  intmax_t min_alphabet_length = 1;
  intmax_t max_alphabet_length = 20;
  intmax_t batch_size = 100;
  intmax_t seed = 42;
  std::string alphabet = "abcdefghijklmnopqrstuvwxyz0123456789";

  int argi = 1;
  const char* arg = NULL;
  while (argi < argc) {
    arg = argv[argi++];
    if (strcmp("--help", arg) == 0 || strcmp("-h", arg) == 0) {
      print_help(argv[0]);
    } else if (strcmp("--max-text-length", arg) == 0 || strcmp("-TL", arg) == 0) {
      arg = argv[argi++];
      max_text_length = std::stoll(arg);
    } else if (strcmp("--min-text-length", arg) == 0 || strcmp("-Tl", arg) == 0) {
      arg = argv[argi++];
      min_text_length = std::stoll(arg);
    } else if (strcmp("--max-alphabet-length", arg) == 0 || strcmp("-AL", arg) == 0) {
      arg = argv[argi++];
      max_alphabet_length = std::stoll(arg);
    } else if (strcmp("--min-alphabet-length", arg) == 0 || strcmp("-Al", arg) == 0) {
      arg = argv[argi++];
      min_alphabet_length = std::stoll(arg);
    } else if (strcmp("--batch-size", arg) == 0 || strcmp("-b", arg) == 0) {
      arg = argv[argi++];
      batch_size = std::stoll(arg);
    } else if (strcmp("--seed", arg) == 0 || strcmp("-s", arg) == 0) {
      arg = argv[argi++];
      seed = std::stoll(arg);
    } else {
      std::cerr << "Unhandled argument '" << arg << "'" << std::endl;
      exit(1);
    }
  }

  if (min_alphabet_length > max_alphabet_length) {
    std::cerr << "Min alphabet length must be less than or equal to max alphabet length" << std::endl;
    exit(1);
  }

  if (min_text_length > max_text_length) {
    std::cerr << "Min text length must be less than or equal to max text length" << std::endl;
    exit(1);
  }

  if (min_text_length <= 0) {
    std::cerr << "Min text length must be greater than zero" << std::endl;
    exit(1);
  }

  if (min_alphabet_length <= 0) {
    std::cerr << "Min alphabet length must be greater than zero" << std::endl;
    exit(1);
  }

  if (max_alphabet_length > alphabet.size()) {
    std::cerr << "Max alphabet length must be most the size of the alphabet, which is Sigma = \"" << alphabet << "\", with |Sigma| = " << alphabet.size() << std::endl;
    exit(1);
  }

  std::mt19937 generator(seed);
  std::ofstream file ("./out.scale.csv");
  file << "alphabet,alphabet_length,text,text_length,palindrome_length,palindrome_number,push_count,pop_count,interval_count,interval_size,comparison_count,elapsed_time" << std::endl;
  for (intmax_t alphabet_length = min_alphabet_length; alphabet_length <= max_alphabet_length; ++alphabet_length) {
    Alphabet sigma = Alphabet(alphabet.substr(0, alphabet_length), '$', '#');
    for (intmax_t text_length = min_text_length; text_length <= max_text_length; ++text_length) {
      for (intmax_t batch_entry = 0; batch_entry < batch_size; ++batch_entry) {
        std::string string = sample_string(generator, sigma, text_length);
        file << alphabet.substr(0, alphabet_length) << "," << alphabet_length << "," << string << "," << text_length << std::flush;
        Text t1 = Text::from(sigma, string);
        Text t2 = t1.invert();
        Text text = t1.join(t2, sigma);
        SuffixArray suffix_array = SuffixArray::build(text);
        LCPArray lcp_array = LCPArray::build(text, suffix_array);

        TELEMETRY.start();
        intmax_t separator_position = t1.size() - 1;
        auto found_palindromes = please_mummy_find_the_longest_palindromes_for_me(lcp_array, suffix_array, separator_position);
        TELEMETRY.end();

        file
          << "," << found_palindromes.length
          << "," << found_palindromes.starts.size()
          << "," << TELEMETRY.push_count
          << "," << TELEMETRY.pop_count
          << "," << TELEMETRY.interval_count
          << "," << TELEMETRY.interval_size
          << "," << TELEMETRY.comparison_count
          << "," << TELEMETRY.elapsed_time
          << std::endl;
      }
    }
  }
  file.close();
}
