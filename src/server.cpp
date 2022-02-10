#include "server.hpp"
#include "logger.hpp"

using namespace logger;

void core::Server::set_port(uint16_t p) {
	port = p;
}

void core::Server::start() {
	core::Socket::handle_function_t f = [this](buf_t a, buf_t b) { handle(a, b); };
	core::Socket s(f);
	s.start();
}
core::Server::~Server() {
	// delete socket;
}

void core::Server::handle(
	core::Server::buf_t &rx_buf,
	core::Server::buf_t &tx_buf
) {
	Log(DEBUG) << "Start parsing method";
	Method::Type method = parse_method(rx_buf);
	if (method == http::Method::METHOD_ERROR) {
		Log(ERROR) << "Bad core method";
		return;
	}
	Log(DEBUG) << "Done parsing method " << method;

	Log(DEBUG) << "Start parsing url";
	std::string url = parse_url(rx_buf);
	if (url.length() < 1) {
		Log(ERROR) << "Bad url";
		return;
	}
	Log(DEBUG) << "Done parsing url " << url;

	Log(DEBUG) << "Start parsing version";
	core::Version::Type version = parse_version(rx_buf);
	if (version == core::Version::Type::VERSION_ERROR) {
		Log(ERROR) << "Bad version";
		return;
	}
	Log(DEBUG) << "Done parsing version " << version;

	(void)(tx_buf);
	(void)(method);
}
