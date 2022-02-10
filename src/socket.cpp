#include "socket.hpp"

core::Socket::Socket(handle_function_t fn) {
	pool = new pool::Pool<int>();
	handle_fn = fn;
}

core::Socket::~Socket() {
	delete pool;
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
	server.sin_port = htons(8888);

	if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
		// std::cout << "Could not bind port 8888" << std::endl;
		Log(ERROR) << "Could not bind port 8888";
		return;
	}

	Log(INFO) << "Server is listening on port: 8888";

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

	std::thread h([this, &rx_buffer, &tx_buffer]() {
		Log(INFO) << "Start handler thread";
		handle_fn(rx_buffer, tx_buffer);
	});

	std::thread rx_thread([&connection, &rx_buffer]() {
		Log(INFO) << "Start rx thread";
		connection.rx(rx_buffer);
	});

	std::thread tx_thread([&connection, &tx_buffer]() {
		Log(INFO) << "Start tx thread";
		connection.tx(tx_buffer);
	});


	h.join();
	rx_thread.join();
	tx_thread.join();

	connection.quit();
}

void core::Socket::start() {
	init();
	gather_clients();
}


core::Connection::Connection(int _new_socket) {
	_socket = _new_socket;
}

void core::Connection::tx(buffer::Buffer<char>& buf) {
	// write(_socket, msg.c_str(), msg.length());
	(void)(buf);
}

void core::Connection::rx(buffer::Buffer<char>& buf) {
	std::string rcv_msg;
	char buffer[500];

	int read_size;
	while ( (read_size = recv(_socket, buffer, 500, 0)) > 0) {
		/// rcv_msg.append(buffer, read_size);
		Log(DEBUG) << "Receaved size: " << read_size;
		std::vector<char> chunk(std::begin(buffer), std::begin(buffer) + read_size);
		std::cout << buffer << std::endl;
		buf.push_chunk(chunk);
	}

	{
		char endofstream = 0;
		buf.push(endofstream);
	}
}

void core::Connection::quit() {
	// Further sends and receives are disallowed
	shutdown(_socket, 2);
}
