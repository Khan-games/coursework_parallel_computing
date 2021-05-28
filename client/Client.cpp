#include "Client.h"

using namespace boost::asio;

Client::Client(std::string ip, int port, std::vector<std::string> v_msg) :
	ep(boost::asio::ip::address::from_string(ip), port),
	v_msg(v_msg)
{
	sock = new ip::tcp::socket(service);
	run_thread = std::thread(&Client::run, this);
}

Client::~Client() {
	if (run_thread.joinable()) run_thread.join();
	delete sock;
}

template <typename T>
void Client::archive_and_send(T& data) {
	// archive data
	streambuf buff;
	boost::archive::binary_oarchive archive(buff, flags);
	archive << data;
	cons::print("[ARCH] Data archived.  thread_id = " + thread_id_to_str());

	// send size
	size_t buff_size = buff.size();
	sock->send(buffer(&buff_size, sizeof(size_t)));

	// send data
	size_t bytes = sock->send(buff.data());
	buff.consume(bytes);
	cons::print("[MSG] Data send.  thread_id = " + thread_id_to_str(), YELLOW);
}

std::string Client::thread_id_to_str() { // convert current thread's id to string
	std::stringstream ss;
	std::string s;
	ss << std::this_thread::get_id();
	ss >> s;
	return s;
}

void Client::connect() {
	cons::print("[NEW CONN] Connecting to server " + ep.address().to_string() 
		+ ":" + std::to_string(ep.port()) + ";  thread_id = " + thread_id_to_str());
	boost::system::error_code ec;
	sock->connect(ep, ec); // connecting

	// check connection error
	if (ec) {
		cons::print("[ERROR] Failed to connect to server!  thread_id = " + thread_id_to_str(), RED);
		return;
	}
	cons::print("[NEW CONN] Connection established with server " + sock->remote_endpoint().address().to_string()
		+ ":" + std::to_string(sock->remote_endpoint().port()) + ";  thread_id = " + thread_id_to_str(), GREEN);
}

void Client::run() {
	cons::print("[NEW CLIENT] Client created on thread = " + thread_id_to_str(), GREEN);

	connect();

	for (auto msg : v_msg) {
		test_echo_run(msg);
	}
}

void Client::test_echo_run(std::string msg) {
	archive_and_send(msg);
	std::string echo_msg;
	read_client_data(echo_msg);
	if (msg == echo_msg) {
		cons::print("[ECHO] Received echo msg == sended msg.  thread_id = " + thread_id_to_str(), GREEN);
	}
	else {
		cons::print("[ECHO] Received echo msg != sended msg.  thread_id = " + thread_id_to_str(), RED);
	}
}

void Client::join() {
	run_thread.join();
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