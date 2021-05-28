#include "Client.h"

using namespace boost::asio;

Client::Client(Server& server, boost::asio::ip::tcp::socket* sock, unsigned id) :
	host_server(server),
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

//void Client::make_task() { // parallel method that recieves and make task from socket
//	while (true) {
//		int bytes;
//		try {
//			bytes = read(*sock, buffer(buff), std::bind(&Client::read_complete, this, buff, std::placeholders::_1,
//				std::placeholders::_2)); // read msg
//		}
//		catch (boost::system::system_error& err) { // error handling (lead to disconnect)
//			disconnected = true; // disconnect client
//			// error log
//			std::string err_text(boost::system::system_error(err).what());
//			cons::print("[ERROR] " + err_text + ";  Client id = " + std::to_string(get_id()), RED);
//			return;
//		}
//
//		// echo msg
//		std::string msg(buff, bytes);
//		if (msg.length() > 1) {
//			cons::print("[MSG] Received msg \"" + msg.substr(0, msg.length() - 1) + "\";  Client id = " 
//				+ std::to_string(get_id()), YELLOW);
//		}
//		else {
//			cons::print("[MSG] Received msg is empty;  Client id = " + std::to_string(get_id()), RED);
//		}
//		sock->write_some(buffer(msg)); // echo to client
//	}
//}

template<typename T>
void Client::read_data_once(T& data, size_t size) {
	streambuf buff;
	sock->receive(buff.prepare(size));
	buff.commit(size);
	boost::archive::binary_iarchive iarchive(buff, flags);
	iarchive >> data;
}

template<typename T>
void Client::read_client_data(T& data) {
	size_t size;
	read_data_once(size, sizeof(size_t));
	read_data_once(data, size);
}

void Client::make_task() { // parallel method that recieves and make task from socket
	while (true) {
		int bytes;
		std::string msg;
		try {
			read_client_data(msg);
			cons::print(msg, CYAN);
		}
		catch (boost::system::system_error& err) { // error handling (lead to disconnect)
			disconnected = true; // disconnect client
			// error log
			cons::print("[ERROR] Remote host closed connection.  Client id = " + std::to_string(get_id()), RED);
			//std::string err_text(boost::system::system_error(err).what());
			//cons::print("[ERROR] " + err_text + ";  Client id = " + std::to_string(get_id()), RED);
			return;
		}

		//// echo msg
		//std::string msg(buff, bytes);
		//if (msg.length() > 1) {
		//	cons::print("[MSG] Received msg \"" + msg.substr(0, msg.length() - 1) + "\";  Client id = " 
		//		+ std::to_string(get_id()), YELLOW);
		//}
		//else {
		//	cons::print("[MSG] Received msg is empty;  Client id = " + std::to_string(get_id()), RED);
		//}
		//sock->write_some(buffer(msg)); // echo to client
	}
}

unsigned Client::get_id() {
	return id;
}

std::string Client::get_ip() {
	return sock->remote_endpoint().address().to_string();
}

bool Client::is_disconnected() {
	return disconnected;
}

size_t Client::read_complete(char* buff, const boost::system::error_code& err,
	size_t bytes) {	// check if reading is done (return 0 if done)

	if (err) return 0;

	bool found = std::find(buff, buff + bytes, '\n') < (buff + bytes);
	return (!found) * BYTES_PER_READ;
}