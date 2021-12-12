#include "Client.h"

#define LOG_TOKEN_RESPONCE

//#define TEST_WP_CHECK

using namespace boost::asio;
namespace chr = std::chrono;

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
	//cons::print("[ARCH] Data archived.  thread_id = " + thread_id_to_str());

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
	
	swapKeys();
}

void Client::swapKeys()
{
	std::string responce = "0";
	while (responce == "0") {
		rsaCustom.generateKeyPair();

		// receive plain RSA pub
		std::string remoteExpStr;
		std::string remoteModulusStr;
		read_client_data(remoteExpStr);
		read_client_data(remoteModulusStr);
		rsa::Key remotePublicKey = { rsa::RSACustom::toCppInt(remoteExpStr), rsa::RSACustom::toCppInt(remoteModulusStr) };
		rsaCustom.setRemotePublicKey(remotePublicKey);

		// send encrypted RSA key + hash
		// encrypt exp
		boost::multiprecision::cpp_int pubKeyExp =
			rsaCustom.encrypt(rsa::RSACustom::toString(rsaCustom.getPublicKey().exp)); // encrypt key exp
		// split modulus into two strings
		std::string modulusStr = rsa::RSACustom::toString(rsaCustom.getPublicKey().modulus);
		size_t strDelimiter = modulusStr.size() / 2;
		std::string modulusFirst = std::string(modulusStr, 0, strDelimiter);
		std::string modulusSecond = std::string(modulusStr, strDelimiter, modulusStr.size() - strDelimiter);
		boost::multiprecision::cpp_int pubKeyModulus1 = rsaCustom.encrypt(modulusFirst); // encrypt key modulus first
		boost::multiprecision::cpp_int pubKeyModulus2 = rsaCustom.encrypt(modulusSecond); // encrypt key modulus second
		// strings to send
		std::string pubKeyExpStr = rsa::RSACustom::toString(pubKeyExp);
		std::string pubKeyModulusStr1 = rsa::RSACustom::toString(pubKeyModulus1);
		std::string pubKeyModulusStr2 = rsa::RSACustom::toString(pubKeyModulus2);
		std::string hashStr = SHA256Custom(rsa::RSACustom::toString(rsaCustom.getPublicKey().exp 
			+ rsaCustom.getPublicKey().modulus)).getHash();
		// send
		archive_and_send(pubKeyExpStr); // send key exp
		archive_and_send(pubKeyModulusStr1); // send key modulus 1
		archive_and_send(pubKeyModulusStr2); // send key modulus 2
		archive_and_send(hashStr); // send key hash
		// get responce
		read_client_data(responce);
		if (responce == "0") {
			continue;
		}
		responce = "1";

		// receive AES encrypted and signed + hash
		// prepare vars
		std::string aesKeySignedEncrypted, aesKeyHash;
		std::string aesIVSignedEncrypted, aesIVHash;
		// receive data
		read_client_data(aesKeySignedEncrypted);
		read_client_data(aesKeyHash);
		read_client_data(aesIVSignedEncrypted);
		read_client_data(aesIVHash);
		// unsign
		std::string aesKey = rsaCustom.decryptAndUnsign(rsa::RSACustom::toCppInt(aesKeySignedEncrypted));
		std::string aesIV = rsaCustom.decryptAndUnsign(rsa::RSACustom::toCppInt(aesIVSignedEncrypted));
		// check hash
		if (!SHA256Custom::isEqual(aesKeyHash, SHA256Custom(aesKey).getHash()) ||
			!SHA256Custom::isEqual(aesIVHash, SHA256Custom(aesIV).getHash())) {
			//cons::print("[ERROR] Hash of AES key is not equal to decrypted AES key!", RED);
			archive_and_send(responce);
			continue;
		}
		// set received key
		aesCustom.setKey(aes::AESCustom::stringToKey(aesKey));
		aesCustom.setIV(aes::AESCustom::stringToKey(aesIV));
		responce = "1";
		archive_and_send(responce);
	}
	
	
}

void Client::run() {
	cons::print("[NEW CLIENT] Client created on thread = " + thread_id_to_str(), GREEN);

	connect();
	
	for (auto msg : v_msg) {

		// timing on search (start)
		auto start_time = chr::high_resolution_clock::now();

		archive_and_send(msg); // send request

		// receive size of responce
		int responce_size;
		read_client_data(responce_size);

		bool msg_responce_is_correct = true;

		// receive data
		for (int i = 0; i < responce_size; i++) {
			// read token
			std::string token;
			read_client_data(token);
			// read positions for token
			std::list<index::word_pos> wp_list;
			read_client_data(wp_list);
			//read path map
			std::map<int, std::string> path_map;
			read_client_data(path_map);

			// generate output
			#ifdef LOG_TOKEN_RESPONCE
			std::string out_s;
			out_s += "[RESP]\tMsg = \"" + msg + "\"\n\tToken = \"" + token 
				+ "\"\n\tThread id = " + thread_id_to_str()
				+ "\n\tFor " + std::to_string(i) + "th of " + std::to_string(responce_size) 
				+ " tokens found " + std::to_string(wp_list.size()) + " results";
			for (auto& wp : wp_list) {
				out_s += "\n\t\t{ doc_id: " + std::to_string(wp.doc_id) + "; row: " 
					+ std::to_string(wp.row) + "; pos_in_row: " 
					+ std::to_string(wp.pos_in_row) + " }\t\""
					+ path_map[wp.doc_id] + "\"";
			}
			cons::print(out_s);
			#endif

			// check wp
			#ifdef TEST_WP_CHECK
			for (auto& wp : wp_list) {
				if (!check_wp(token, wp, path_map[wp.doc_id])) {
					msg_responce_is_correct = false;
				}
			}
			#endif
		}

		#ifdef TEST_WP_CHECK
		if (msg_responce_is_correct) {
			cons::print("[TEST] For msg = \"" + msg + "\" all responses are correct.  thread id = " 
				+ thread_id_to_str(), GREEN);
		}
		else {
			cons::print("[TEST] For msg = \"" + msg + "\" there are incorrect responces.  thread id = "
				+ thread_id_to_str(), RED);
		}
		#endif

		// timing on search (end)
		auto end_time = chr::high_resolution_clock::now();
		cons::print("[TIME] Request = \"" + msg + "\";\n\tSearch took: " 
			+ std::to_string(chr::duration_cast<chr::milliseconds>(end_time - start_time).count())
			+ "ms\n\tThread id = " + thread_id_to_str(), CYAN);

	}
}

bool Client::check_wp(const std::string& token, const index::word_pos& wp, const std::string& temp_path) {
	std::string path = "../index_creator/" + temp_path;

	// open file
	std::ifstream fin;
	if (!std::filesystem::exists(path)) return false;
	try {
		fin.open(path);
		if (!fin.is_open()) return false;
	}
	catch (const std::ifstream::failure& e) {
		cons::print("[ERROR] File \"" + path + "\" failed to open.  thread id = "
			+ thread_id_to_str(), RED);
		return false;
	}

	// get needed row
	std::string str;
	for (int i = 0; i <= wp.row ; i++) {
		std::getline(fin, str);
	}

	// str now contains correct string
	bool result = false;
	str = str.substr(wp.pos_in_row, token.length());
	boost::to_lower(str); // tokenization
	if (str == token) {
		result = true;
	} 

	fin.close();
	return result;
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