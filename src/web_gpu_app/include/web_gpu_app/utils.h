#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#define TRACE_VAR(x) std::cout << #x << ": " << x << std::endl;

namespace web_gpu_app {

inline std::string ReadFileToString(const std::string& file_name) {
  std::ifstream file(file_name);
  if (!file) {
    std::cerr << "Cannot open file: " << file_name << std::endl;
    return "";
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

}  // namespace web_gpu_app
