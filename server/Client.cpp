#include "Client.h"

using namespace CryptoPP;
using namespace boost::multiprecision;
using namespace boost::asio;
namespace chr = std::chrono;

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
void Client::read_data_once(T& data, size_t size, bool encryption) {
	// receive data
	streambuf encryptedBuff;
	sock->receive(encryptedBuff.prepare(size));
	encryptedBuff.commit(size);
	
	streambuf buff; // buffer with decrypted data

	// decrypt if encryption == true
	if (encryption) {
		std::string encryptedData = buffToString(encryptedBuff);
		std::string decryptedData = aesCustom.decrypt(encryptedData);
		stringToBuffer(buff, encryptedData);
	}
	else {
		// forward copy encrypted buff to buff
		std::size_t bytes_copied = buffer_copy(
			buff.prepare(encryptedBuff.size()), encryptedBuff.data());                
		buff.commit(bytes_copied);
	}

	// deserialize
	boost::archive::binary_iarchive iarchive(buff, flags);
	iarchive >> data;
}

template<typename T>
void Client::read_client_data(T& data) {
	try {
		size_t size;
		read_data_once(size, sizeof(size_t), false);
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

void Client::swapKeys() {
	std::string responce = "0";
	while (responce == "0") {
		rsaCustom.generateKeyPair();

		// send plain RSA pub
		std::string publicExpStr = rsa::RSACustom::toString(rsaCustom.getPublicKey().exp);
		std::string publicModulusStr = rsa::RSACustom::toString(rsaCustom.getPublicKey().modulus);
		archive_and_send(publicExpStr);
		archive_and_send(publicModulusStr);

		// receive encrypted RSA key + hash
		std::string remoteKeyExpStr;
		std::string remoteKeyModulusStr1;
		std::string remoteKeyModulusStr2;
		std::string remoteKeyHashStr;
		read_client_data(remoteKeyExpStr);
		read_client_data(remoteKeyModulusStr1);
		read_client_data(remoteKeyModulusStr2);
		read_client_data(remoteKeyHashStr);
		// decrypt
		cpp_int remoteKeyExp = rsa::RSACustom::toCppInt(rsaCustom.decrypt(rsa::RSACustom::toCppInt(remoteKeyExpStr)));
		cpp_int remoteKeyModulus = rsa::RSACustom::toCppInt(rsaCustom.decrypt(rsa::RSACustom::toCppInt(remoteKeyModulusStr1)) 
			+ rsaCustom.decrypt(rsa::RSACustom::toCppInt(remoteKeyModulusStr2)));
		rsa::Key remoteKey = { remoteKeyExp , remoteKeyModulus };
		// check hash
		if (!SHA256Custom::isEqual(SHA256Custom(rsa::RSACustom::toString(remoteKeyExp + remoteKeyModulus)).getHash(), remoteKeyHashStr)) {
			//cons::print("[ERROR] Remote public key hashes in not equal!", RED);
			archive_and_send(responce);
			continue;
		}
		rsaCustom.setRemotePublicKey(remoteKey);
		responce = "1";
		archive_and_send(responce);
		responce = "0";

		// send signed and encrypted AES key
		// prepare vars
		SecByteBlock aesKey = aesCustom.getKey();
		SecByteBlock aesIV = aesCustom.getIV();
		std::string aesKeyStr = aes::AESCustom::keyToString(aesKey);
		std::string aesIVStr = aes::AESCustom::keyToString(aesIV);
		// calc hash
		std::string aesKeyHash = SHA256Custom(aesKeyStr).getHash();
		std::string aesIVHash = SHA256Custom(aesIVStr).getHash();
		// sign and encrypt
		std::string aesKeySignedEncrypted = rsa::RSACustom::toString(rsaCustom.encrypt(rsaCustom.encryptSign(aesKeyStr)));
		std::string aesIVSignedEncrypted = rsa::RSACustom::toString(rsaCustom.encrypt(rsaCustom.encryptSign(aesIVStr)));
		// send
		archive_and_send(aesKeySignedEncrypted);
		archive_and_send(aesKeyHash);
		archive_and_send(aesIVSignedEncrypted);
		archive_and_send(aesIVHash);

		read_client_data(responce);
	}
}

void Client::make_task() { // parallel method that recieves and make task from socket
	swapKeys(); // swap RSA and AES keys

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
		
		if (disconnected) return;

		// log receiving
		cons::print("[MSG] Received msg \"" + msg + "\";  Client id = "
			+ std::to_string(get_id()), YELLOW);

		// split request into tokens
		std::vector<std::string> request;
		split_request(msg, request);

		// timing on search (start)
		auto start_time = chr::high_resolution_clock::now();
		auto time_from_server_start = chr::duration_cast<chr::milliseconds>(start_time - host_server.server_start_time);
		cons::print("[TIME] Search start time: " + std::to_string(time_from_server_start.count()) + "ms, request = \"" 
			+ msg + "\";\n\tClient id = " + std::to_string(get_id()), BLUE);

		// search in index
		search_return_type result;
		if (request.size() == 1) {
			auto single_result = host_server.server_index.search(request[0]); // search
			result.push_back(search_pair_type(request[0], single_result));
		}
		else if (request.size() > 1) {
			result = host_server.server_index.cross_search(request); // search
		}

		// timing on search (end)
		auto end_time = chr::high_resolution_clock::now();
		time_from_server_start = chr::duration_cast<chr::milliseconds>(end_time - host_server.server_start_time);
		cons::print("[TIME] Search end time: " + std::to_string(time_from_server_start.count()) + "ms, request = \""
			+ msg + "\";\n\tSearch took: " + std::to_string(chr::duration_cast<chr::milliseconds>(end_time-start_time).count())
			+ "ms\n\tClient id = " + std::to_string(get_id()), CYAN);

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
		
		cons::print("[MSG] Send responce to \"" + msg + "\";  Client id = "
			+ std::to_string(get_id()), GREEN);
	
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
	streambuf rawBuff;
	boost::archive::binary_oarchive archive(rawBuff, flags);
	archive << data;
	//cons::print("[ARCH] Data archived.  Client id = " + std::to_string(get_id()));

	// encrypt data
	std::string rawData = buffToString(rawBuff);
	std::string encryptedData = aesCustom.encrypt(rawData);
	streambuf buff; // buffer with encrypted data
	stringToBuffer(buff, encryptedData);
	//cons::print(encryptedData, RED);
	for (auto x : encryptedData) std::cout << (unsigned)x << ' ';
	std::cout << std::endl;

	// send size
	size_t buffSize = buff.size();
	sock->send(buffer(&buffSize, sizeof(size_t)));

	// send data
	size_t bytes = sock->send(buff.data());
	buff.consume(bytes);
	//cons::print("[MSG] Data send.  Client id = " + std::to_string(get_id()), YELLOW);
}

