#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <iostream>
#include <fstream>
#include <string>

namespace petrenko {

struct Person {
  size_t id;
  std::string info;
};

struct Node {
  Person data;
  Node* next;
};

class DataProcessor {
private:
  Node* head;
  Node* tail;
  size_t validCount;
  size_t ignoredCount;

  bool isDuplicate(size_t id) {
    Node* current = head;
    while (current) {
      if (current->data.id == id) {
        return true;
      }
      current = current->next;
    }
    return false;
  }

  void addRecord(const Person& person) {
    Node* newNode = new Node{person, nullptr};
    if (!head) {
      head = newNode;
      tail = newNode;
    } else {
      tail->next = newNode;
      tail = newNode;
    }
    validCount++;
  }

  bool parseId(const std::string& str, size_t& id) {
    id = 0;
    for (size_t i = 0; i < str.length(); i++) {
      if (!std::isdigit(str[i])) {
        return false;
      }
      id = id * 10 + (str[i] - '0');
    }
    return true;
  }

  void skipWhitespace(const std::string& line, size_t& pos) {
    while (pos < line.length() && std::isspace(line[pos])) {
      pos++;
    }
  }

public:
  DataProcessor() : head(nullptr), tail(nullptr), validCount(0), ignoredCount(0) {}

  ~DataProcessor() {
    Node* current = head;
    while (current) {
      Node* next = current->next;
      delete current;
      current = next;
    }
  }

  void processLine(const std::string& line) {
    if (line.empty()) {
      return;
    }
    size_t start = 0;
    skipWhitespace(line, start);
    if (start >= line.length()) {
      return;
    }
    size_t idEnd = start;
    while (idEnd < line.length() && !std::isspace(line[idEnd])) {
      idEnd++;
    }
    std::string idStr = line.substr(start, idEnd - start);
    size_t id;
    if (!parseId(idStr, id)) {
      ignoredCount++;
      return;
    }
    if (isDuplicate(id)) {
      ignoredCount++;
      return;
    }
    size_t descStart = idEnd;
    skipWhitespace(line, descStart);
    if (descStart >= line.length()) {
      ignoredCount++;
      return;
    }
    std::string description = line.substr(descStart);
    while (!description.empty() && std::isspace(description.back())) {
      description.pop_back();
    }
    if (description.empty()) {
      ignoredCount++;
      return;
    }
    addRecord({id, description});
  }

  void processInput(std::istream& input) {
    std::string line;
    while (std::getline(input, line)) {
      processLine(line);
    }
  }

  void outputResults(std::ostream& output) {
    Node* current = head;
    while (current) {
      output << current->data.id << " " << current->data.info << "\n";
      current = current->next;
    }
  }

  size_t getValidCount() const {
    return validCount;
  }

  size_t getIgnoredCount() const {
    return ignoredCount;
  }
};

}

#endif
