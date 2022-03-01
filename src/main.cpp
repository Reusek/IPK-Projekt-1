#include "pool.hpp"
#include "http.hpp"
#include "logger.hpp"
#include "server.hpp"

using namespace logger;

http::Response test_e(http::Request req) {
	(void)(req);
	return http::Response::ok("Hello world!");
}

int main() {
	core::Server server;

	server.set_port(8888);
	server.add_endpoint("/test", test_e);
	server.start();
	return 0;
}
