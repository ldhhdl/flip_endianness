#include <cstdlib>
#include <elf.h>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

std::string read_file(const std::string &filename) {
  std::ifstream input_file{filename, std::ifstream::in | std::ifstream::binary};
  if (!input_file.is_open() || !input_file.good()) {
    std::cerr << "Error opening " << filename << std::endl;
    return std::string{};
  }
  std::string file_data{std::istreambuf_iterator<char>(input_file),
                        std::istreambuf_iterator<char>()};
  input_file.close();
  return file_data;
}

void flip_endian_value(Elf64_Ehdr *ehdr) {
  auto new_value{ELFDATANONE};
  switch (ehdr->e_ident[EI_DATA]) {
  case ELFDATANONE:
    std::cout << "Invalid data encoding" << std::endl;
    break;
  case ELFDATA2LSB:
    new_value = ELFDATA2MSB;
    break;
  case ELFDATA2MSB:
    new_value = ELFDATA2LSB;
    break;
  default:
    std::cout << "Unknown data encoding" << std::endl;
    break;
  }
  ehdr->e_ident[EI_DATA] = new_value;
}

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " INPUT_FILENAME OUTPUT_FILENAME"
              << std::endl;
    return EXIT_FAILURE;
  }

  // Check input file
  const std::string input_filename{argv[1]};
  if (!fs::is_regular_file(input_filename)) {
    std::cerr << input_filename << " does not exist" << std::endl;
    return EXIT_FAILURE;
  }

  // Check output file
  const std::string output_filename{argv[2]};
  if (fs::is_regular_file(output_filename)) {
    std::cerr << output_filename << " already exists" << std::endl;
    return EXIT_FAILURE;
  }

  auto file_data = read_file(input_filename);
  if (file_data.length() == 0) {
    std::cerr << "Error reading " << input_filename << std::endl;
    return EXIT_FAILURE;
  }

  // Flip endian value
  Elf64_Ehdr *ehdr = reinterpret_cast<Elf64_Ehdr *>(&file_data[0]);
  std::cout << "Original endian value: " << (char)ehdr->e_ident[EI_DATA]
            << std::endl;
  flip_endian_value(ehdr);
  std::cout << "New endian value: " << (char)ehdr->e_ident[EI_DATA]
            << std::endl;

  // Write to file
  std::ofstream output_file{output_filename,
                            std::ofstream::out | std::ofstream::binary};
  if (!output_file.is_open() || !output_file.good()) {
    std::cerr << "Error opening " << output_filename << std::endl;
    return EXIT_FAILURE;
  }
  output_file.write(file_data.c_str(), file_data.length());
  output_file.close();

  return 0;
}
