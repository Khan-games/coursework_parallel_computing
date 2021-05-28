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
		std::string msg; // message to receive
		try {
			read_client_data(msg);
		}
		catch (boost::system::system_error& err) { // error handling (lead to disconnect)
			disconnected = true; // disconnect client
			// error log
			cons::print("[ERROR] Remote host closed connection.  Client id = " + std::to_string(get_id()), RED);
			//std::string err_text(boost::system::system_error(err).what());
			//cons::print("[ERROR] " + err_text + ";  Client id = " + std::to_string(get_id()), RED);
			return;
		}

		// log receiving
		cons::print("[MSG] Received msg \"" + msg + "\";  Client id = "
			+ std::to_string(get_id()), YELLOW);

		// test echo msg
		archive_and_send(msg);
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

template <typename T>
void Client::archive_and_send(T& data) {
	// archive data
	streambuf buff;
	boost::archive::binary_oarchive archive(buff, flags);
	archive << data;
	//cons::print("[ARCH] Data archived.  Client id = " + std::to_string(get_id()));

	// send size
	size_t buff_size = buff.size();
	sock->send(buffer(&buff_size, sizeof(size_t)));

	// send data
	size_t bytes = sock->send(buff.data());
	buff.consume(bytes);
	//cons::print("[MSG] Data send.  Client id = " + std::to_string(get_id()), YELLOW);
}