#include "../include/client/query_client.hh"

#include <fstream>
#include <vector>
#include <string>

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>

#include <gtest/gtest.h>

namespace bp = boost::process;

// Assume that all servers are already up and running
class Mp1Test : public ::testing::Test 
{
protected:
	// Entry function, invoke to validate the result on running grep_cmd
	void test_command(std::string grep_cmd)
	{
		start_client(grep_cmd);
		int count = 1;

    for (const auto& log_file : output_files) 
		{
			try 
			{
				std::string output_res = read_log_file(log_file);
				
				std::string expected_file_path = "./test/logs/vm" + std::to_string(count++) + ".log";
				std::string expected_res = async_execute_grep(expected_file_path, grep_cmd);

				// Compare output_res and expected_res
				bool is_equal = (output_res == expected_res);

				if (output_res.empty() && !expected_res.empty()) 
				{
					FAIL() << "Ouput is empty for " << log_file << " but grep found matches";
				} 
				
				if (!output_res.empty() && expected_res.empty()) 
				{
					FAIL() << "Grep result is empty for " << log_file << " but ouput is not empty";
				} 

				// Check whether the result is the same
				ASSERT_TRUE(is_equal) << "Content in " << log_file << " does not match the grep output";
			} 
			catch (const std::exception& e) 
			{
					FAIL() << "Unexpected exception for " << log_file << ": " << e.what();
			}
    }
	}

	// Initiate client's query
	void start_client(const std::string& grep_command) 
	{
		client::QueryClient::start_interact(grep_command);
	}

	// read in the content of a file
	std::string read_log_file(const std::string& file_path) 
	{
		std::ifstream file(file_path);
		
		if (!file.is_open()) {
				throw std::runtime_error("Failed to open file: " + file_path);
		}
		
		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		// Grep command will append a newline, so need a newline if does not exist
		if (!content.empty() && content.back() != '\n')
		{
			content.push_back('\n');
		}

		return content;
	}

	// Execute the grep command locally on file_path
	std::string async_execute_grep(const std::string& file_path, const std::string& grep_command) {
		bp::ipstream grep_output;
		std::string command = grep_command + " " + file_path;
		
		// spawn a process to execute the grep command
		bp::child c(command, bp::std_out > grep_output);

		std::string result;
		std::string line;
		
		// get the output
		while (std::getline(grep_output, line)) {
				result += line + "\n";
		}

		c.wait();

		return result;
	}

protected:
  // the output files that the client output
  const std::vector<std::string> output_files = 
  {
        "./output/fa24-cs425-1901.cs.illinois.edu.log",
        "./output/fa24-cs425-1902.cs.illinois.edu.log",
        "./output/fa24-cs425-1903.cs.illinois.edu.log",
        "./output/fa24-cs425-1904.cs.illinois.edu.log",
        "./output/fa24-cs425-1905.cs.illinois.edu.log",
        "./output/fa24-cs425-1906.cs.illinois.edu.log",
        "./output/fa24-cs425-1907.cs.illinois.edu.log",
        "./output/fa24-cs425-1908.cs.illinois.edu.log",
        "./output/fa24-cs425-1909.cs.illinois.edu.log",
        "./output/fa24-cs425-1910.cs.illinois.edu.log"
  };
};

TEST_F(Mp1Test, RequestErrorInfrequent) 
{
	// Status code 401 should be infrequent in http
	// -n: show line numbers
	test_command("grep -n 401");
}

TEST_F(Mp1Test, SuccessFrequent) 
{
	// Status code 200 should be common in http
  test_command("grep 200");
}

// TEST_F(Mp1Test, GetRequestFrequent) 
// {
// 	// Same as GET request
//   test_command("grep -i \"get\"");
// }

TEST_F(Mp1Test, RegexCommand) 
{
	// Search for successful get and put requests
  test_command("grep -E '[0-9]{5}'");
}

TEST_F(Mp1Test, ZeroMatch) 
{
	test_command("grep abcdefg");
}

int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}