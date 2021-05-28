#include "Server.h"

using namespace boost::asio;

Server::Server() :
	acceptor(service, ip::tcp::endpoint(boost::asio::ip::tcp::v4(), CON_PORT))
{

}

void Server::listen() {

}
