#pragma once

#include <boost/asio.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <filesystem>

#include "../index_creator/t_output.h"
#include "Constants.h"
#include "../index_creator/Index.h"

#include "../network_encryption/RSACustom.h"
#include "../network_encryption/AESCustom.h"
#include "../network_encryption/SHA256.h"



class Client
{
public:
	Client(std::string ip, int port, std::vector<std::string> v_msg);
	~Client();

	// get
	std::string thread_id_to_str(); 
	
	// threads
	void join();

	// buffer converting
	static std::string buffToString(boost::asio::streambuf& sb); // get data from buffer to std::string 
	static void stringToBuffer(boost::asio::streambuf& sb, std::string const& data); // write data from std::string to buffer

private:
	// network vars
	boost::asio::io_service service; // main obj for boost::asio
	boost::asio::ip::tcp::socket* sock;
	boost::asio::ip::tcp::endpoint ep; // connecting to localhost, port 10105

	// encryption
	rsa::RSACustom rsaCustom;
	aes::AESCustom aesCustom;

	// threads
	std::thread run_thread;

	// message handling 
	std::vector<std::string> v_msg;

	// main methods
	void connect(); // connecting to server
	void swapKeys(); // RSA and AES keys swap with sign
	void run();	// run client in separate  thread

	// transfer data
	template <typename T>
	void archive_and_send(T& data);
	template<typename T>
	void read_data_once(T& data, size_t size, bool encryption = true); // get data block from client
	template<typename T>
	void read_client_data(T& data); // get serialized data from client

	// other methods
	void test_echo_run(std::string msg); // provide echo check
	bool check_wp(const std::string& token, const index::word_pos& wp, const std::string& path); // true if wp is correct 
};

inline std::string Client::buffToString(boost::asio::streambuf& sb)
{
	std::istream is(std::addressof(sb));
	std::string line;
	std::getline(is, line);
	return line;
}

inline void Client::stringToBuffer(boost::asio::streambuf& sb, std::string const& data)
{
	std::ostream strm(std::addressof(sb));
	strm << data;
}