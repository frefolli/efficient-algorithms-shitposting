#include <fstream>
#include <library.hh>
#include <string.h>

void print_help(const char* progname) {
  std::cout << "Usage: " << progname << " [-h] [-l <INT>] [-L <INT>] [-s <INT>] [-a <STR>]" << std::endl;
  std::cout << "Arguments" << std::endl;
  std::cout << "    -h, --help                                             Print this Help screen" << std::endl;
  std::cout << "    -t, --text <STR>                                       Set analysis text" << std::endl;
  std::cout << "    -a, --alphabet <STR>                                   Set analysis alphabet" << std::endl;
  exit(0);
}

int main(int argc, char** argv) {
  std::srand(170701);
  std::string raw_alphabet = "1234xab";
  std::string raw_text = "1234xaba4321$";

  int argi = 1;
  const char* arg = NULL;
  while (argi < argc) {
    arg = argv[argi++];
    if (strcmp("--help", arg) == 0 || strcmp("-h", arg) == 0) {
      print_help(argv[0]);
    } else if (strcmp("--text", arg) == 0 || strcmp("-t", arg) == 0) {
      arg = argv[argi++];
      raw_text = arg;
    } else if (strcmp("--alphabet", arg) == 0 || strcmp("-a", arg) == 0) {
      arg = argv[argi++];
      raw_alphabet = arg;
    } else {
      std::cerr << "Unhandled argument '" << arg << "'" << std::endl;
      exit(1);
    }
  }

  Alphabet alphabet = Alphabet(raw_alphabet, '$', '#');
  Text text = Text::from(alphabet, raw_text);
  SuffixArray suffix_array = SuffixArray::build(text);
  BurrowsWheelerTransform burrows_wheeler_transform = BurrowsWheelerTransform::build(text, suffix_array);
  FMIndex fm_index = FMIndex::build(alphabet, burrows_wheeler_transform);
  LCPArray lcp_array = LCPArray::build(text, suffix_array);

  std::ofstream file ("out.analysis.md");

  file << "## Alphabet" << std::endl;
  alphabet.to_markdown(file << "$\\Sigma$ := ") << std::endl;
  file << std::endl;
  file << std::endl;

  file << "## TextS" << std::endl;
  text.to_markdown(file << "$T_1$ := ", alphabet) << std::endl;
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
