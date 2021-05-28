#include "Client.h"

Client::Client(boost::asio::ip::tcp::socket* sock, unsigned id) :
	sock(sock),
	id(id)
{
	th_task = std::thread(&Client::make_task, this); // create separate thread to perform the task
}

Client::~Client() {
	th_task.detach();
	sock->close();
	delete sock;
}

void Client::make_task() { // parallel method that recieves and make task from socket
	
}

unsigned Client::get_id() {
	return id;
}

std::string Client::get_ip() {
	return sock->remote_endpoint().address().to_string();
}