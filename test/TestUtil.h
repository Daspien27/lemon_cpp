#pragma once

#include <filesystem>

namespace lemon::testing::util{
namespace fs = std::filesystem;

static bool is_existing_file(const fs::path& file_name) {
  return fs::exists(file_name) && fs::is_regular_file(file_name);
}

static bool is_existing_directory(const fs::path& dir_name) {
  return fs::exists(dir_name) && fs::is_directory(dir_name);
}

static fs::path existing_file(const fs::path& file_name) {
  if(is_existing_file(file_name)) {
    return file_name;
  }
  throw std::runtime_error(file_name.string() + " not found or not file.");
}

static fs::path existing_directory(const fs::path& dir_name) {
  if(is_existing_directory(dir_name)) {
    return dir_name;
  }
  throw std::runtime_error(dir_name.string() + " not found or not directory.");
}
}