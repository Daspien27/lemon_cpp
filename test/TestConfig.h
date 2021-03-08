#pragma once

#include "TestUtil.h"

namespace lemon::testing::config{
static auto lempar_path() {
  return util::existing_file(LEMPAR_PATH);
}

static auto sqlite_grammar_path() {
  return util::existing_file(SQLITE_GRAMMAR_PATH);
}

static auto lemon_original_executable() {
  return util::existing_file(LEMON_ORIGINAL_EXECUTABLE_PATH);
}

static auto lemon_cpp_executable() {
  return util::existing_file(LEMON_CPP_EXECUTABLE_PATH);
}

static auto lemon_original_output_dir() {
  return util::existing_directory(LEMON_ORIGINAL_OUT_DIR);
}

static auto lemon_cpp_output_dir() {
  return util::existing_directory(LEMON_CPP_OUT_DIR);
}
}
