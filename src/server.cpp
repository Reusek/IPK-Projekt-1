#include "server.hpp"
#include "logger.hpp"

using namespace logger;

core::Server::Server() {

}

void core::Server::set_port(uint16_t p) {
	port = p;
}

void core::Server::start() {
	core::Socket::handle_function_t f = [this](buf_t a, buf_t b) { handle(a, b); };
	core::Socket s(f);
	s.start();
}

void core::Server::handle(
	core::buf_t &rx_buf,
	core::buf_t &tx_buf
) {
	Log(DEBUG) << "Start parsing method";
	http::MethodType method = http_manager.parse_method(rx_buf);
	if (method == http::MethodType::METHOD_ERROR) {
		Log(ERROR) << "Bad core method";
		auto resp = http::Response::bad_request("Bad http method\n").to_string(http_manager);
		tx_buf.push_chunk(std::vector<char>(resp.begin(), resp.end()));
		return;
	}
	Log(DEBUG) << "Done parsing method " << method;

	Log(DEBUG) << "Start parsing url";
	std::string url = http_manager.parse_url(rx_buf);
	if (url.length() < 1) {
		Log(ERROR) << "Bad url";
		auto resp = http::Response::bad_request("Bad url\n").to_string(http_manager);
		tx_buf.push_chunk(std::vector<char>(resp.begin(), resp.end()));
		return;
	}
	Log(DEBUG) << "Done parsing url " << url;

	Log(DEBUG) << "Start parsing version";
	http::VersionType version = http_manager.parse_version(rx_buf);
	if (version == http::VersionType::VERSION_ERROR) {
		Log(ERROR) << "Bad version";
		auto resp = http::Response::bad_request("Bad http version\n").to_string(http_manager);
		tx_buf.push_chunk(std::vector<char>(resp.begin(), resp.end()));
		return;
	}
	Log(DEBUG) << "Done parsing version " << version;

	Log(DEBUG) << "Start parsing headers";
	auto headers = http_manager.parse_headers(rx_buf);
	Log(DEBUG) << "Done parsing headers";

	http::Request request = http::Request {
		.version = version,
		.method = method,
		.url = url,
		.headers = headers,
	};

	auto endpoint_search = endpoints.find(url);
	if (endpoint_search != endpoints.end()) {
		auto resp = endpoint_search->second(request).to_string(http_manager);
		tx_buf.push_chunk(std::vector<char>(resp.begin(), resp.end()));
	} else {
		Log(INFO) << "Endpoint with url: " << url << "does not exit.";
		auto resp = http::Response::bad_request("Unknown url\n").to_string(http_manager);
		tx_buf.push_chunk(std::vector<char>(resp.begin(), resp.end()));
	}

	/* auto endpoint = endpoints.find(url);
	if (endpoint == endpoints.end()) {
		Log(DEBUG) << "Unknown url";
	} */

	// auto response = endpoint->second(request);

	/* std::string response_string = response.to_string(http_manager);
	std::vector<char> response_vector(response_string.begin(), response_string.end());
	tx_buf.push_chunk(response_vector); */

	// TODO: Remove this after implementation.
	(void)(tx_buf);
	(void)(method);
}

void core::Server::add_endpoint(std::string url, endpoint_fn fn) {
	endpoints.insert({url, fn});
}

void core::Server::add_endpoints(std::vector<std::tuple<std::string, endpoint_fn>> eps) {
	for (auto e: eps) {
		endpoints.insert({std::get<0>(e), std::get<1>(e)});
	}
}
