#include "pch.h"
#include "CppUnitTest.h"

#include <filesystem>
#include <fstream>
#include <iterator>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace regression
{
	using namespace std::string_view_literals;
	namespace fs = std::filesystem;

	TEST_CLASS(sqlite_regression)
	{
		// current_path() ends up being in lemon_cpp/Release so .. to escape
		const fs::path original = "../out/sqlite/lemon_original/."sv;
		const fs::path cpp = "../out/sqlite/lemon_cpp/."sv;

	public:
		
		bool compare_original_cpp(const std::string_view& file)
		{

			auto original_file = fs::path{ original } / file;
			Assert::IsTrue(original_file.has_filename());

			auto cpp_file = fs::path{ cpp } / file;
			Assert::IsTrue(cpp_file.has_filename());

			Assert::IsTrue(fs::exists(original_file), L"Original file does not exist.");
			Assert::IsTrue(fs::exists(cpp_file), L"Cpp file does not exist.");

			std::ifstream original_file_s(original_file);
			std::ifstream cpp_file_s(cpp_file);

			Assert::IsFalse(original_file_s.fail(), L"Could not open original file.");
			Assert::IsFalse(cpp_file_s.fail(), L"Could not open cpp file.");

			using isiter = std::istream_iterator<char>;

			return std::equal(isiter{ original_file_s }, isiter{}, isiter{ cpp_file_s }, isiter{});
		}

		TEST_METHOD(sqlite_c)
		{
			constexpr auto file = "sqlite_parse.c"sv;
			
			Assert::IsTrue(compare_original_cpp(file), L"sqlite_parse.c failed regression test.");
		}

		TEST_METHOD(sqlite_h)
		{
			constexpr auto file = "sqlite_parse.h"sv;

			Assert::IsTrue(compare_original_cpp(file), L"sqlite_parse.h failed regression test.");
		}

		TEST_METHOD(sqlite_out)
		{
			constexpr auto file = "sqlite_parse.out"sv;

			Assert::IsTrue(compare_original_cpp(file), L"sqlite_parse.out failed regression test.");
		}
	};
}
