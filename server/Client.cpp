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
	try {
		size_t size;
		read_data_once(size, sizeof(size_t));
		read_data_once(data, size);
	}
	catch (boost::system::system_error& err) { // error handling (lead to disconnect)
		disconnected = true; // disconnect client
		// error log
		//cons::print("[ERROR] Remote host closed connection.  Client id = " + std::to_string(get_id()), RED);
		//std::string err_text(boost::system::system_error(err).what());
		//cons::print("[ERROR] " + err_text + ";  Client id = " + std::to_string(get_id()), RED);
		return;
	}
}

void Client::split_request(std::string msg, std::vector<std::string>& req) {
	req.clear();

	// parse msg
	bool was_word = false;
	int start_pos = 0;
	std::string token;
	for (int end_pos = 0; end_pos < msg.length(); end_pos++) {
		if (is_word(msg[end_pos])) {
			if (!was_word) start_pos = end_pos;
			was_word = true;
		}
		else if (was_word) { // word found
			was_word = false;
			// create token
			token = msg.substr(start_pos, end_pos - start_pos);
			boost::to_lower(token);
			// add token to request
			req.push_back(token);
		}
	}
	if (was_word) {
		// create token
		token = msg.substr(start_pos, msg.size() - start_pos);
		boost::to_lower(token);
		// add token to request
		req.push_back(token);
	}
}

void Client::make_task() { // parallel method that recieves and make task from socket
	while (true) {
		// typedefs
		typedef std::vector<std::pair<std::string, std::list<index::word_pos> > > search_return_type;
		typedef std::pair<std::string, std::list<index::word_pos> > search_pair_type;

		std::string msg; // request on search
		try {
			read_client_data(msg);
		}
		catch (boost::system::system_error& err) {
			disconnected = true; // disconnect client
			// error log
			cons::print("[ERROR] Remote host closed connection.  Client id = " 
				+ std::to_string(get_id()), RED);
			return;
		}
		

		// log receiving
		cons::print("[MSG] Received msg \"" + msg + "\";  Client id = "
			+ std::to_string(get_id()), YELLOW);

		// split request into tokens
		std::vector<std::string> request;
		split_request(msg, request);

		// search in index
		search_return_type result;
		if (request.size() == 1) {
			auto single_result = host_server.server_index.search(request[0]); // search
			result.push_back(search_pair_type(request[0], single_result));
		}
		else if (request.size() > 1) {
			result = host_server.server_index.cross_search(request); // search
		}

		int res_size = result.size(); // size

		// generate map with result pathes
		std::map<int, std::string> path_map;
		for (int i = 0; i < res_size; i++) {
			for (auto& wp : result[i].second) {
				path_map[wp.doc_id] = host_server.server_index.paths[wp.doc_id];
			}
		}

		// send results
		try {
			archive_and_send(res_size); // send size
			for (int i = 0; i < result.size(); i++) { // send data
				archive_and_send(result[i].first);
				archive_and_send(result[i].second);
				archive_and_send(path_map);
			}
		}
		catch (boost::system::system_error& err) {
			disconnected = true; // disconnect client
			// error log
			cons::print("[ERROR] Remote host closed connection.  Client id = "
				+ std::to_string(get_id()), RED);
			return;
		}
		

	
	} // infinite loop
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