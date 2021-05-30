#pragma once

#include <boost/asio.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "../index_creator/t_output.h"
#include "Constants.h"
#include "../index_creator/Index.h"

class Client
{
public:
	Client(std::string ip, int port, std::vector<std::string> v_msg);
	~Client();

	// get
	std::string thread_id_to_str(); 
	
	// threads
	void join();

private:
	// network vars
	boost::asio::io_service service; // main obj for boost::asio
	boost::asio::ip::tcp::socket* sock;
	boost::asio::ip::tcp::endpoint ep; // connecting to localhost, port 10105

	// threads
	std::thread run_thread;

	// message handling 
	std::vector<std::string> v_msg;

	// main methods
	void connect(); // connecting to server
	void run();	// run client in separate  thread

	// transfer data
	template <typename T>
	void archive_and_send(T& data);
	template<typename T>
	void read_data_once(T& data, size_t size); // get data block from client
	template<typename T>
	void read_client_data(T& data); // get serialized data from client

	// other methods
	void test_echo_run(std::string msg);
};

