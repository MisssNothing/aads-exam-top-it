#include "meeting-processor.hpp"

int main(int argc, char* argv[]) {
  if (argc > 3) {
    std::cerr << "Error: Too many arguments" << "\n";
    return 0;
  }

  std::string inputFilename;
  std::string dataFilename;

  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg.length() >= 3 && arg.substr(0, 3) == "in:") {
      if (!inputFilename.empty()) {
        std::cerr << "Error: Input file specified twice" << "\n";
        return 1;
      }
      inputFilename = arg.substr(3);
    } else if (arg.length() >= 5 && arg.substr(0, 5) == "data:") {
      if (!dataFilename.empty()) {
        std::cerr << "Error: Data file specified twice" << "\n";
        return 1;
      }
      dataFilename = arg.substr(5);
    } else {
      std::cerr << "Error: Invalid argument: " << arg << "" << "\n";
      return 1;
    }
  }

  if (dataFilename.empty()) {
    std::cerr << "Error: Data file is required" << "\n";
    return 1;
  }

  petrenko::MeetingProcessor processor;

  if (!inputFilename.empty()) {
    std::ifstream inputFile;
    inputFile.open(inputFilename);
    if (!inputFile.is_open()) {
      std::cerr << "Error: Cannot open input file: " << inputFilename << "" << "\n";
      return 2;
    }
    processor.loadPersons(inputFile);
    inputFile.close();
  }

  std::ifstream dataFile;
  dataFile.open(dataFilename);
  if (!dataFile.is_open()) {
    std::cerr << "Error: Cannot open data file: " << dataFilename << "" << "\n";
    return 2;
  }
  processor.loadMeetings(dataFile);
  dataFile.close();

  std::string command;
  while (std::getline(std::cin, command)) {
    processor.processCommand(command);
  }

  return 0;
}
