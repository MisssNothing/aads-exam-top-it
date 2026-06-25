#ifndef MEETING_PROCESSOR_HPP
#define MEETING_PROCESSOR_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "../common/processor.hpp"

namespace petrenko {

struct Meeting {
  size_t id1;
  size_t id2;
  size_t duration;
};

class MeetingProcessor {
private:
  std::vector<Person> persons;
  std::vector<Meeting> meetings;

  bool isDigit(char c) {
    return c >= '0' && c <= '9';
  }

  bool parseNumber(const std::string& str, size_t& num) {
    num = 0;
    for (size_t i = 0; i < str.length(); i++) {
      if (!isDigit(str[i])) {
        return false;
      }
      num = num * 10 + (str[i] - '0');
    }
    return true;
  }

  bool findPerson(size_t id, Person*& result) {
    for (size_t i = 0; i < persons.size(); i++) {
      if (persons[i].id == id) {
        result = &persons[i];
        return true;
      }
    }
    return false;
  }

  bool findPerson(size_t id) {
    Person* dummy;
    return findPerson(id, dummy);
  }

  void addPerson(size_t id, const std::string& info = "") {
    persons.push_back({id, info});
  }

  void skipWhitespace(const std::string& line, size_t& pos) {
    while (pos < line.length() && std::isspace(line[pos])) {
      pos++;
    }
  }

  void sortMeetings(std::vector<Meeting>& list, size_t id) {
    for (size_t i = 0; i < list.size(); i++) {
      for (size_t j = i + 1; j < list.size(); j++) {
        size_t other1 = (list[i].id1 == id) ? list[i].id2 : list[i].id1;
        size_t other2 = (list[j].id1 == id) ? list[j].id2 : list[j].id1;
        if (other1 > other2 || (other1 == other2 && list[i].duration > list[j].duration)) {
          Meeting temp = list[i];
          list[i] = list[j];
          list[j] = temp;
        }
      }
    }
  }

  bool findInVector(const std::vector<size_t>& vec, size_t value) {
    for (size_t i = 0; i < vec.size(); i++) {
      if (vec[i] == value) {
        return true;
      }
    }
    return false;
  }

public:
  MeetingProcessor() {}

  void loadPersons(std::istream& input) {
    std::string line;
    while (std::getline(input, line)) {
      if (line.empty()) {
        continue;
      }
      size_t start = 0;
      skipWhitespace(line, start);
      if (start >= line.length()) {
        continue;
      }
      size_t idEnd = start;
      while (idEnd < line.length() && !std::isspace(line[idEnd])) {
        idEnd++;
      }
      std::string idStr = line.substr(start, idEnd - start);
      size_t id;
      if (!parseNumber(idStr, id)) {
        continue;
      }
      if (findPerson(id)) {
        continue;
      }
      size_t descStart = idEnd;
      skipWhitespace(line, descStart);
      if (descStart >= line.length()) {
        continue;
      }
      std::string description = line.substr(descStart);
      while (!description.empty() && std::isspace(description.back())) {
        description.pop_back();
      }
      if (description.empty()) {
        continue;
      }
      addPerson(id, description);
    }
  }

  void loadMeetings(std::istream& input) {
    std::string line;
    while (std::getline(input, line)) {
      if (line.empty()) {
        continue;
      }
      size_t pos = 0;
      skipWhitespace(line, pos);
      if (pos >= line.length()) {
        continue;
      }
      size_t id1End = pos;
      while (id1End < line.length() && !std::isspace(line[id1End])) {
        id1End++;
      }
      if (id1End >= line.length()) {
        std::cerr << "Error: Invalid meeting data" << "\n";
        exit(3);
      }
      std::string id1Str = line.substr(pos, id1End - pos);
      size_t id1;
      if (!parseNumber(id1Str, id1)) {
        std::cerr << "Error: Invalid meeting data" << "\n";
        exit(3);
      }
      pos = id1End;
      skipWhitespace(line, pos);
      if (pos >= line.length()) {
        std::cerr << "Error: Invalid meeting data" << "\n";
        exit(3);
      }
      size_t id2End = pos;
      while (id2End < line.length() && !std::isspace(line[id2End])) {
        id2End++;
      }
      if (id2End >= line.length()) {
        std::cerr << "Error: Invalid meeting data" << "\n";
        exit(3);
      }
      std::string id2Str = line.substr(pos, id2End - pos);
      size_t id2;
      if (!parseNumber(id2Str, id2)) {
        std::cerr << "Error: Invalid meeting data" << "\n";
        exit(3);
      }
      pos = id2End;
      skipWhitespace(line, pos);
      if (pos >= line.length()) {
        std::cerr << "Error: Invalid meeting data" << "\n";
        exit(3);
      }
      std::string durStr = line.substr(pos);
      while (!durStr.empty() && std::isspace(durStr.back())) {
        durStr.pop_back();
      }
      size_t duration;
      if (!parseNumber(durStr, duration)) {
        std::cerr << "Error: Invalid meeting data" << "\n";
        exit(3);
      }
      if (id1 != id2) {
        if (!findPerson(id1)) {
          addPerson(id1);
        }
        if (!findPerson(id2)) {
          addPerson(id2);
        }
        meetings.push_back({id1, id2, duration});
      }
    }
  }

  void anons() {
    std::vector<size_t> anonIds;
    for (size_t i = 0; i < persons.size(); i++) {
      if (persons[i].info.empty()) {
        anonIds.push_back(persons[i].id);
      }
    }
    std::sort(anonIds.begin(), anonIds.end());
    for (size_t i = 0; i < anonIds.size(); i++) {
      std::cout << anonIds[i] << "\n";
    }
    if (anonIds.empty()) {
      std::cout << "\n";
    }
  }

  void deanon(size_t anonId, size_t id) {
    Person* anonPerson = nullptr;
    Person* person = nullptr;
    if (!findPerson(anonId, anonPerson) || !findPerson(id, person)) {
      std::cout << "<INVALID COMMAND>" << "\n";
      return;
    }
    if (!anonPerson->info.empty() || person->info.empty()) {
      std::cout << "<INVALID COMMAND>" << "\n";
      return;
    }
    for (size_t i = 0; i < meetings.size(); i++) {
      if (meetings[i].id1 == anonId) {
        meetings[i].id1 = id;
      }
      if (meetings[i].id2 == anonId) {
        meetings[i].id2 = id;
      }
    }
    for (size_t i = 0; i < persons.size(); i++) {
      if (persons[i].id == anonId) {
        persons.erase(persons.begin() + i);
        break;
      }
    }
    for (size_t i = 0; i < meetings.size(); i++) {
      if (meetings[i].id1 == id && meetings[i].id2 == id) {
        meetings[i].id1 = 0;
        meetings[i].id2 = 0;
        meetings[i].duration = 0;
      }
    }
  }

  void redesc(size_t id, const std::string& description) {
    Person* person = nullptr;
    if (!findPerson(id, person)) {
      std::cout << "<INVALID COMMAND>" << "\n";
      return;
    }
    person->info = description;
  }

  void desc(size_t id) {
    Person* person = nullptr;
    if (!findPerson(id, person)) {
      std::cout << "<INVALID COMMAND>" << "\n";
      return;
    }
    if (person->info.empty()) {
      std::cout << "<ANON>" << "\n";
    } else {
      std::cout << person->info << "\n";
    }
  }

  void meets(size_t id) {
    if (!findPerson(id)) {
      std::cout << "<INVALID COMMAND>" << "\n";
      return;
    }
    std::vector<Meeting> result;
    for (size_t i = 0; i < meetings.size(); i++) {
      if ((meetings[i].id1 == id || meetings[i].id2 == id) &&
          !(meetings[i].id1 == 0 && meetings[i].id2 == 0)) {
        result.push_back(meetings[i]);
      }
    }
    sortMeetings(result, id);
    for (size_t i = 0; i < result.size(); i++) {
      size_t other = (result[i].id1 == id) ? result[i].id2 : result[i].id1;
      std::cout << other << " " << result[i].duration << "\n";
    }
    if (result.empty()) {
      std::cout << "\n";
    }
  }

  void commons(size_t id1, size_t id2) {
    if (!findPerson(id1) || !findPerson(id2)) {
      std::cout << "<INVALID COMMAND>" << "\n";
      return;
    }
    std::vector<size_t> common;
    std::vector<size_t> meetings1;
    std::vector<size_t> meetings2;

    for (size_t i = 0; i < meetings.size(); i++) {
      if ((meetings[i].id1 == id1 || meetings[i].id2 == id1) &&
          !(meetings[i].id1 == 0 && meetings[i].id2 == 0)) {
        size_t other = (meetings[i].id1 == id1) ? meetings[i].id2 : meetings[i].id1;
        if (other != id1 && other != id2) {
          meetings1.push_back(other);
        }
      }
      if ((meetings[i].id1 == id2 || meetings[i].id2 == id2) &&
          !(meetings[i].id1 == 0 && meetings[i].id2 == 0)) {
        size_t other = (meetings[i].id1 == id2) ? meetings[i].id2 : meetings[i].id1;
        if (other != id1 && other != id2) {
          meetings2.push_back(other);
        }
      }
    }

    for (size_t i = 0; i < meetings1.size(); i++) {
      for (size_t j = 0; j < meetings2.size(); j++) {
        if (meetings1[i] == meetings2[j] && !findInVector(common, meetings1[i])) {
          common.push_back(meetings1[i]);
        }
      }
    }

    std::sort(common.begin(), common.end());
    for (size_t i = 0; i < common.size(); i++) {
      std::cout << common[i] << "\n";
    }
  }

  void less(size_t time, size_t id) {
    if (!findPerson(id)) {
      std::cout << "<INVALID COMMAND>" << "\n";
      return;
    }
    std::vector<Meeting> result;
    for (size_t i = 0; i < meetings.size(); i++) {
      if ((meetings[i].id1 == id || meetings[i].id2 == id) &&
          meetings[i].duration < time &&
          !(meetings[i].id1 == 0 && meetings[i].id2 == 0)) {
        result.push_back(meetings[i]);
      }
    }
    sortMeetings(result, id);
    for (size_t i = 0; i < result.size(); i++) {
      size_t other = (result[i].id1 == id) ? result[i].id2 : result[i].id1;
      std::cout << other << " " << result[i].duration << "\n";
    }
    if (common.empty()) {
      std::cout << "\n";
    }
  }

  void greater(size_t time, size_t id) {
    if (!findPerson(id)) {
      std::cout << "<INVALID COMMAND>" << "\n";
      return;
    }
    std::vector<Meeting> result;
    for (size_t i = 0; i < meetings.size(); i++) {
      if ((meetings[i].id1 == id || meetings[i].id2 == id) &&
          meetings[i].duration > time &&
          !(meetings[i].id1 == 0 && meetings[i].id2 == 0)) {
        result.push_back(meetings[i]);
      }
    }
    sortMeetings(result, id);
    for (size_t i = 0; i < result.size(); i++) {
      size_t other = (result[i].id1 == id) ? result[i].id2 : result[i].id1;
      std::cout << other << " " << result[i].duration << "\n";
    }
  }

  void outPersons(const std::string& filename) {
    std::ofstream outputFile;
    outputFile.open(filename);
    if (!outputFile.is_open()) {
      std::cerr << "Error: Cannot open output file: " << filename << "\n";
      exit(2);
    }
    for (size_t i = 0; i < persons.size(); i++) {
      if (!persons[i].info.empty()) {
        outputFile << persons[i].id << " " << persons[i].info << "\n";
      }
    }
    outputFile.close();
  }

  void processCommand(const std::string& command) {
    if (command == "anons") {
      anons();
      return;
    }
    if (command.rfind("deanon ", 0) == 0) {
      std::string args = command.substr(7);
      size_t pos = 0;
      skipWhitespace(args, pos);
      if (pos >= args.length()) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      size_t id1End = pos;
      while (id1End < args.length() && !std::isspace(args[id1End])) {
        id1End++;
      }
      if (id1End >= args.length()) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      std::string id1Str = args.substr(pos, id1End - pos);
      size_t id1;
      if (!parseNumber(id1Str, id1)) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      pos = id1End;
      skipWhitespace(args, pos);
      if (pos >= args.length()) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      std::string id2Str = args.substr(pos);
      size_t id2;
      if (!parseNumber(id2Str, id2)) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      deanon(id1, id2);
      return;
    }
    if (command.rfind("redesc ", 0) == 0) {
      std::string args = command.substr(7);
      size_t pos = 0;
      skipWhitespace(args, pos);
      if (pos >= args.length()) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      size_t idEnd = pos;
      while (idEnd < args.length() && !std::isspace(args[idEnd])) {
        idEnd++;
      }
      if (idEnd >= args.length()) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      std::string idStr = args.substr(pos, idEnd - pos);
      size_t id;
      if (!parseNumber(idStr, id)) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      pos = idEnd;
      skipWhitespace(args, pos);
      if (pos >= args.length() || args[pos] != '"') {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      pos++;
      size_t quoteEnd = pos;
      while (quoteEnd < args.length() && args[quoteEnd] != '"') {
        quoteEnd++;
      }
      if (quoteEnd >= args.length()) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      std::string description = args.substr(pos, quoteEnd - pos);
      redesc(id, description);
      return;
    }
    if (command.rfind("desc ", 0) == 0) {
      std::string args = command.substr(5);
      size_t pos = 0;
      skipWhitespace(args, pos);
      if (pos >= args.length()) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      std::string idStr = args.substr(pos);
      size_t id;
      if (!parseNumber(idStr, id)) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      desc(id);
      return;
    }
    if (command.rfind("meets ", 0) == 0) {
      std::string args = command.substr(6);
      size_t pos = 0;
      skipWhitespace(args, pos);
      if (pos >= args.length()) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      std::string idStr = args.substr(pos);
      size_t id;
      if (!parseNumber(idStr, id)) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      meets(id);
      return;
    }
    if (command.rfind("commons ", 0) == 0) {
      std::string args = command.substr(8);
      size_t pos = 0;
      skipWhitespace(args, pos);
      if (pos >= args.length()) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      size_t id1End = pos;
      while (id1End < args.length() && !std::isspace(args[id1End])) {
        id1End++;
      }
      if (id1End >= args.length()) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      std::string id1Str = args.substr(pos, id1End - pos);
      size_t id1;
      if (!parseNumber(id1Str, id1)) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      pos = id1End;
      skipWhitespace(args, pos);
      if (pos >= args.length()) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      std::string id2Str = args.substr(pos);
      size_t id2;
      if (!parseNumber(id2Str, id2)) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      commons(id1, id2);
      return;
    }
    if (command.rfind("less ", 0) == 0) {
      std::string args = command.substr(5);
      size_t pos = 0;
      skipWhitespace(args, pos);
      if (pos >= args.length()) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      size_t timeEnd = pos;
      while (timeEnd < args.length() && !std::isspace(args[timeEnd])) {
        timeEnd++;
      }
      if (timeEnd >= args.length()) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      std::string timeStr = args.substr(pos, timeEnd - pos);
      size_t time;
      if (!parseNumber(timeStr, time)) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      pos = timeEnd;
      skipWhitespace(args, pos);
      if (pos >= args.length()) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      std::string idStr = args.substr(pos);
      size_t id;
      if (!parseNumber(idStr, id)) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      less(time, id);
      return;
    }
    if (command.rfind("greater ", 0) == 0) {
      std::string args = command.substr(8);
      size_t pos = 0;
      skipWhitespace(args, pos);
      if (pos >= args.length()) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      size_t timeEnd = pos;
      while (timeEnd < args.length() && !std::isspace(args[timeEnd])) {
        timeEnd++;
      }
      if (timeEnd >= args.length()) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      std::string timeStr = args.substr(pos, timeEnd - pos);
      size_t time;
      if (!parseNumber(timeStr, time)) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      pos = timeEnd;
      skipWhitespace(args, pos);
      if (pos >= args.length()) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      std::string idStr = args.substr(pos);
      size_t id;
      if (!parseNumber(idStr, id)) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      greater(time, id);
      return;
    }
    if (command.rfind("out-persons ", 0) == 0) {
      std::string filename = command.substr(12);
      size_t pos = 0;
      skipWhitespace(filename, pos);
      if (pos >= filename.length()) {
        std::cout << "<INVALID COMMAND>" << "\n";
        return;
      }
      filename = filename.substr(pos);
      while (!filename.empty() && std::isspace(filename.back())) {
        filename.pop_back();
      }
      outPersons(filename);
      return;
    }
    std::cout << "<INVALID COMMAND>" << "\n";
  }
};

}
#endif
