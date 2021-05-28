#include "Server.h"

using namespace boost::asio;

Server::Server() :
	acceptor(service, ip::tcp::endpoint(boost::asio::ip::tcp::v4(), CON_PORT))
{

}

Server::~Server() {
	std::unique_lock<std::mutex> ul(clients_m);
	for (int i = 0; i < clients.size(); i++) { // delete dynamic clients
		delete clients[i];
	}
}

void Server::listen() {
	while (true) {
		// get connection
		ip::tcp::socket* sock = new ip::tcp::socket(service);
		acceptor.accept(*sock);

		// create new client
		std::unique_lock<std::mutex> ul(clients_m);
		clients.push_back(new Client(sock, next_client_id++));
		Client* temp_client = clients.back(); // temp client to create log
		ul.unlock();

		// log
		cons::print("[NEW CONN] Connection established with IP "
			+ temp_client->get_ip() + ", client id = " 
			+ std::to_string(temp_client->get_id()), GREEN);
		
	}
}
