#include "Server.h"

using namespace boost::asio;

Server::Server() :
	acceptor(service, ip::tcp::endpoint(boost::asio::ip::tcp::v4(), CON_PORT))
{
	cons::print("[LOAD] Index loading.", YELLOW);
	server_index.load_from_file("../index_creator/index.index");
	cons::print("[LOAD] Index loaded.", GREEN);

	server_start_time = std::chrono::high_resolution_clock::now(); // time nullify

	disconnect_th = std::thread(&Server::disconnect_clients, this);
}

Server::~Server() {
	// stop threads
	disconnect_th.detach();

	// clear dynamic clients
	std::unique_lock<std::mutex> ul(clients_m);
	for (auto i : clients) { 
		delete i;
	}
}

void Server::listen() {
	while (true) {
		// get connection
		ip::tcp::socket* sock = new ip::tcp::socket(service);
		acceptor.accept(*sock);

		// create new client
		std::unique_lock<std::mutex> ul(clients_m);
		clients.push_back(new Client(*this, sock, next_client_id++));
		Client* temp_client = clients.back(); // temp client to create log
		ul.unlock();

		// log
		cons::print("[NEW CONN] Connection established with IP "
			+ temp_client->get_ip() + "; Client id = " 
			+ std::to_string(temp_client->get_id()), GREEN);
		
	}
}

void Server::disconnect_clients() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(DISCONNECT_CYCLE)); // delete every N seconds

		std::unique_lock<std::mutex> ul(clients_m);

		// search correct clients
		std::list<Client*>::iterator i = clients.begin();
		while (i != clients.end()) {
			bool deleted = false;
				
			if ((*i)->is_disconnected()) { // delete client
				// log
				cons::print("[DISCONNECT] Client with id = " + std::to_string((*i)->get_id()) 
					+ " disconnected.", RED);

				// deleting
				deleted = true;
				delete (*i);
				i = clients.erase(i);
			}

			if (!deleted) i++;
		}
	}
}
