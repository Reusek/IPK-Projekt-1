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
	core::buf_t &rx_buf,
	core::buf_t &tx_buf
) {
	Log(DEBUG) << "Start parsing method";
	http::Method::Type method = http_manager.parse_method(rx_buf);
	if (method == http::Method::METHOD_ERROR) {
		Log(ERROR) << "Bad core method";
		return;
	}
	Log(DEBUG) << "Done parsing method " << method;

	Log(DEBUG) << "Start parsing url";
	std::string url = http_manager.parse_url(rx_buf);
	if (url.length() < 1) {
		Log(ERROR) << "Bad url";
		return;
	}
	Log(DEBUG) << "Done parsing url " << url;

	Log(DEBUG) << "Start parsing version";
	http::Version::Type version = http_manager.parse_version(rx_buf);
	if (version == http::Version::Type::VERSION_ERROR) {
		Log(ERROR) << "Bad version";
		return;
	}
	Log(DEBUG) << "Done parsing version " << version;

	// TODO: Remove this after implementation.
	(void)(tx_buf);
	(void)(method);
}
