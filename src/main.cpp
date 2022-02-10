#include "pool.hpp"
#include "http.hpp"
#include "logger.hpp"
#include "server.hpp"

using namespace logger;

int main() {
	// pool::Pool<int> pool;
	// logger::info << "test";
	// logger::debug << "test";
	// Log(DEBUG) << "test";
	// Log(INFO) << "test";
	// Log(WARN) << "test";
	// Log(ERROR) << "test";

	// http::Method m;
	core::Server server;

	server.set_port(8888);
	server.start();
	return 0;
}
