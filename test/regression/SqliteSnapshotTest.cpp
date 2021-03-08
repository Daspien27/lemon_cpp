#include "../TestConfig.h"
#include "../TestUtil.h"

#include <fstream>
#include <filesystem>
#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <cstdlib>

namespace lemon::testing::regression {
using namespace std::string_view_literals;
namespace fs = std::filesystem;

class SqliteSnapshotTest : public ::testing::Test {
public:

  inline static constexpr auto GeneratedParserSource = "sqlite_parse.c"sv;
  inline static constexpr auto GeneratedParserHeader = "sqlite_parse.h"sv;
  inline static constexpr auto GeneratedParserOutput = "sqlite_parse.out"sv;

  static void GenerateOutputs(const fs::path& executable_path, const fs::path& output_dir) {
    auto command_str = executable_path.string() + " -T\"" +
                       config::lempar_path().string() + "\" -d\"" + output_dir.string() + "\" -l " +
                       config::sqlite_grammar_path().string();
    auto success_status = std::system(command_str.c_str());
    ASSERT_EQ(success_status, EXIT_SUCCESS) << command_str << " did not execute successfully.";
  }

  static void compare_original_cpp(const fs::path& file) {
    auto original_file = config::lemon_original_output_dir() / file;
    auto cpp_file = config::lemon_cpp_output_dir() / file;
    ASSERT_TRUE(util::is_existing_file(original_file)) << "Original file does not exist.";
    ASSERT_TRUE(util::is_existing_file(cpp_file)) << "Cpp file does not exist.";

    std::ifstream original_file_s{original_file,  std::ifstream::binary | std::ifstream::ate};
    std::ifstream cpp_file_s(cpp_file,  std::ifstream::binary | std::ifstream::ate);

    ASSERT_TRUE(!original_file_s.fail()) << "Could not open original file.";
    ASSERT_TRUE(!cpp_file_s.fail()) << "Could not open cpp file.";

    ASSERT_EQ(original_file_s.tellg(), cpp_file_s.tellg())
        << "File sizes mismatch.";

    using isiter = std::istream_iterator<char>;
    original_file_s.seekg(0, std::ifstream::beg);
    cpp_file_s.seekg(0, std::ifstream::beg);
    ASSERT_TRUE(std::equal(isiter{original_file_s}, isiter{},
                                      isiter{cpp_file_s}, isiter{}));
  }
};

TEST_F(SqliteSnapshotTest, CompareGeneratedParser) {
  GenerateOutputs(config::lemon_original_executable(),
                  config::lemon_original_output_dir());
  GenerateOutputs(config::lemon_cpp_executable(),
                  config::lemon_cpp_output_dir());
  compare_original_cpp(GeneratedParserSource);
  compare_original_cpp(GeneratedParserHeader);
  compare_original_cpp(GeneratedParserOutput);
}
}