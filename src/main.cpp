#include "pool.hpp"
#include "http.hpp"
#include "logger.hpp"
#include "server.hpp"

using namespace logger;

int main() {
	core::Server server;

	server.set_port(8888);
	server.start();
	return 0;
}
