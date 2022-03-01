#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

// Related to sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "pool.hpp"
#include "buffer.hpp"

namespace core {
	using buf_t = buffer::Buffer<char>&;

	class Connection {
	public:
		Connection(int _new_socket);

		/**
		 * @brief Send string to client.
		 *
		 * @param message string
		 */
		void tx(buffer::Buffer<char>& buf);

		/**
		 * @brief Send string to client.
		 *
		 * @param message string
		 */
		void rx(buffer::Buffer<char>& buf);

		/**
		 * @brief Close connection.
		 */
		void quit();

		// TODO: Implement read method!
	private:
		int _socket;
	};

	class Socket {
	public:
		using handle_function_t = std::function<void(buf_t, buf_t)>;
		Socket(handle_function_t handle_function);
		// ~Socket() = default;

		void start(std::uint16_t p);
	private:
		int socket_desc, new_socket;
		struct sockaddr_in server, client;
		// pool::Pool<int> *pool;
		std::unique_ptr<pool::Pool<int>> pool;
		handle_function_t handle_fn;

		std::uint16_t port;

		void init();
		void gather_clients();
		void handler(int s);
	};
}

#endif
