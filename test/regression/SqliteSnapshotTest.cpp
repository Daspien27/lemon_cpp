#include <fstream>
#include <filesystem>
#include <gtest/gtest.h>
#include <string_view>

namespace lemon::testing::regression {
using namespace std::string_view_literals;
namespace fs = std::filesystem;

class SqliteSnapshotTest : public ::testing::Test {
public:
  inline static const fs::path ActualDataPath = "./lemon_original_out/"sv;
  inline static const fs::path ExpectedDataPath = "./lemon_original_out/"sv;
  static constexpr auto GeneratedParserSource = "sqlite_parse.c"sv;
  static constexpr auto GeneratedParserHeader = "sqlite_parse.h"sv;
  static constexpr auto GeneratedParserOutput = "sqlite_parse.out"sv;

  static void compare_original_cpp(std::string_view file) {
    auto original_file = ExpectedDataPath / file;
    ASSERT_TRUE(original_file.has_filename());

    auto cpp_file = ActualDataPath / file;
    ASSERT_TRUE(cpp_file.has_filename());

    ASSERT_TRUE(fs::exists(original_file)) << "Original file does not exist.";
    ASSERT_TRUE(fs::exists(cpp_file)) << "Cpp file does not exist.";

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
  compare_original_cpp(GeneratedParserSource);
  compare_original_cpp(GeneratedParserHeader);
  compare_original_cpp(GeneratedParserOutput);
}
}