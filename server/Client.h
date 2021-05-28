#pragma once

#include <boost/asio.hpp>

class Client
{
public:
	Client(boost::asio::ip::tcp::socket *sock, unsigned id);
	~Client();

	void make_task(); // parallel method that recieves and make task from socket
	unsigned get_id();
	std::string get_ip();

private:
	// network vars
	boost::asio::ip::tcp::socket* sock;

	// multithread vars
	std::thread th_task;

	// other
	unsigned id;
};

