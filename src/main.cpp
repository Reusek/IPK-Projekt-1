#include "pool.hpp"
#include "http.hpp"
#include "logger.hpp"
#include "server.hpp"

#include <fstream>
#include <numeric>

using namespace logger;

struct Data {
	std::string test = "";
	std::string cpu_name = "";
	std::string hostname = "";

	uint32_t cpu_usage = 0;

	std::vector<uint32_t> cpu_last = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	uint32_t cpu_sum_last = 0;
	uint32_t cpu_idle_last = 0;
};


void update(Data &data) {
	std::string line;
	std::ifstream fstream;

	fstream.open("/proc/stat");
	getline(fstream, line);
	fstream.close();

	// std::cout << line << std::endl;

	std::size_t pos = 0;
	std::vector<std::uint32_t> cpu_stats;

	if ((pos = line.find(" ")) != std::string::npos) {
		line = line.substr(pos + 2, line.length() - 1);
		// std::cout << line << std::endl;
		while ((pos = line.find(" ")) != std::string::npos) {
			std::string sub_str = line.substr(0, pos);
			// std::cout << sub_str << std::endl;
			cpu_stats.push_back(std::stoul(sub_str));
			line = line.substr(pos + 1, line.length() - 1);
		}
	}

	std::uint32_t cpu_sum = 0;
	for (auto i : cpu_stats) {
		cpu_sum += i;
	}

	std::uint32_t cpu_delta = cpu_sum - data.cpu_sum_last;
	std::uint32_t cpu_idle = cpu_stats[3] - data.cpu_last[3];
	std::uint32_t cpu_used = cpu_delta - cpu_idle;

	data.cpu_usage = 100 * ((float)cpu_used / (float)cpu_delta);

	data.cpu_sum_last = cpu_sum;
	data.cpu_last = cpu_stats;
}

/**
 * @brief Update CPU name
 *
 * @param data Server cached data
 */
void update_cpu_name(Data &data) {
	std::string line;
	std::ifstream fstream;
	fstream.open("/proc/cpuinfo");

	for (;;) {
		getline(fstream, line);
		if (line.rfind("model name", 0) != std::string::npos) {
			std::size_t pos = 0;
			if ((pos = line.find(": ")) != std::string::npos) {
				data.cpu_name = line.substr(pos + 2, line.length() - 1);
			}
			break;
		}
	}

	fstream.close();
}


void update_hostname(Data &data) {
	std::string line;
	std::ifstream fstream;
	fstream.open("/etc/hostname");
	getline(fstream, line);
	data.hostname = line;
	fstream.close();
}

http::Response test_e(http::Request req, Data &data) {
	(void)(req);
	return http::Response::ok("Hello world! " + data.test);
}

http::Response route_cpu_name(http::Request req, Data &data) {
	(void)(req);
	return http::Response::ok(data.cpu_name);
}

http::Response route_hostname(http::Request req, Data &data) {
	(void)(req);
	return http::Response::ok(data.hostname);
}

http::Response route_cpu_load(http::Request req, Data &data) {
	(void)(req);
	return http::Response::ok(std::to_string(data.cpu_usage) + "%");
}


int main(int argc, char **argv) {
	uint32_t port = 0;
	if (argc >= 2) {
		port = std::stoul(argv[1]);
		// return 1;
	} else {
		Log(ERROR) << "Too few arguments!";
		return 1;
	}

	core::Server<Data> server;

	server.set_port(port);

	// Routes
	server.add_endpoint("/test", test_e);
	server.add_endpoint("/cpu-name", route_cpu_name);
	server.add_endpoint("/hostname", route_hostname);
	server.add_endpoint("/load", route_cpu_load);

	// Tasks
	server.add_periodic_task(1000, update);
	server.add_periodic_task(1000, update_cpu_name);
	server.add_periodic_task(5000, update_hostname);
	server.start();
	(void)(argv);
	return 0;
}
