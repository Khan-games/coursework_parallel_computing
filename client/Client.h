#pragma once

#include <boost/asio.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "t_output.h"
#include "Constants.h"

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

	// work methods
	void connect(); // connecting to server
	void run();	// run client in separate  thread

	// transfer data
	template <typename T>
	void archive_and_send(T& data);

};

