#pragma once

#include <boost/asio.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/algorithm/string.hpp>

#include "Constants.h"
#include "../index_creator/t_output.h"
#include "Server.h"
#include "../index_creator/Index.h"

class Server;

class Client
{
public:
	Client(Server& server, boost::asio::ip::tcp::socket *sock, unsigned id);
	~Client();

	// task related
	void make_task(); // parallel method that recieves and make task from socket
	void split_request(std::string msg, std::vector<std::string> &req); // split one msg into req vector

	// get
	unsigned get_id();
	std::string get_ip();
	bool is_disconnected();

private:
	// network vars
	boost::asio::ip::tcp::socket* sock;
	char buff[BUFF_SIZE];
	bool disconnected = false; // if true, then disconnect the client

	// multithread vars
	std::thread th_task;

	// other
	unsigned id;
	Server &host_server;
	
	// transfer methods
	template<typename T>
	void read_data_once(T& data, size_t size); // get data block from client
	template<typename T>
	void read_client_data(T& data); // get serialized data from client
	template<typename T>
	void archive_and_send(T& data); // send serialized data

	bool is_word(const char ch); // true if letter or digit

};

inline bool Client::is_word(const char ch) { // true if letter or digit
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')) {
		return true; // is word
	}
	return false;
}
