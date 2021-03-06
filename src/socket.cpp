#include "socket.hpp"
#include <future>

core::Socket::Socket(handle_function_t fn) :
	pool(std::make_unique<pool::Pool<int>>(1)),
	handle_fn(fn)
{
	handle_fn = fn;
}


void core::Socket::init() {
	// TODO: Rework this to modern way!
	//       https://beej.us/guide/bgnet/html/#getaddrinfoprepare-to-launch

	socket_desc = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_desc == -1) {
		Log(INFO) << "Could not create socket";
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
		Log(ERROR) << "Could not bind port " << port;
		throw std::runtime_error("Could not bind port");
		return;
	}

	Log(INFO) << "Server is listening on port: " << port;

	listen(socket_desc , 3);
}

void core::Socket::gather_clients() {
	socklen_t c = sizeof(struct sockaddr_in);
	int new_socket;
	while ((new_socket = accept(socket_desc, (struct sockaddr *)&client, &c))) {
		pool::Pool<int>::t_task_fn f_handler = [this](int a) {handler(a);};
		pool::Pool<int>::t_task t{f_handler, std::tuple<int>{new_socket}};
		pool->add_task(t);
	}
}

void core::Socket::handler(int s) {
	core::Connection connection(s);

	buffer::Buffer<char> rx_buffer;
	buffer::Buffer<char> tx_buffer;

	// TODO: Create timeout!

	std::thread rx_thread([&connection, &rx_buffer]() {
		Log(DEBUG) << "Start rx thread";
		connection.rx(rx_buffer);
		Log(DEBUG) << "Stop rx thread";
	});

	std::thread tx_thread([&rx_thread, &connection, &tx_buffer]() {
		Log(DEBUG) << "Start tx thread";
		connection.tx(tx_buffer);
		Log(DEBUG) << "Stop tx thread";
	});

	std::thread h([this, &rx_buffer, &tx_buffer]() {
		Log(DEBUG) << "Start handler thread";
		handle_fn(rx_buffer, tx_buffer);
		Log(DEBUG) << "Stop handler thread";
	});

	h.join();
	tx_thread.join();
	connection.quit();
	rx_thread.join();

	Log(DEBUG) << "Connection finished!";

}

void core::Socket::start(std::uint16_t p) {
	port = p;
	init();
	gather_clients();
}


core::Connection::Connection(int _new_socket) {
	_socket = _new_socket;
}

void core::Connection::tx(buffer::Buffer<char>& buf) {
	for (;;) {
		auto c = buf.pop_chunk();
		if (c.back() == '\0') {
			send(_socket, c.data(), c.size() - 1, 0);
			break;
		} else {
			send(_socket, c.data(), c.size(), 0);
		}
	}
	// write(_socket, msg.c_str(), msg.length());
	(void)(buf);
}

void core::Connection::rx(buffer::Buffer<char>& buf) {
	std::string rcv_msg;
	char buffer[500];

	int read_size;

	while ( (read_size = recv(_socket, buffer, 500, 0)) > 0) {
		std::vector<char> chunk(std::begin(buffer), std::begin(buffer) + read_size);
		std::string s(chunk.begin(), chunk.end());
		// std::cout << s;
		buf.push_chunk(chunk);
	}

	{
		char endofstream = '\0';
		buf.push(endofstream);
	}
}

void core::Connection::quit() {
	// Further sends and receives are disallowed
	shutdown(_socket, 0);
}
