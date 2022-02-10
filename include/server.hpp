#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include "buffer.hpp"
#include "socket.hpp"
#include "http.hpp"

namespace core {
	class Server {
	public:
		Server() = default;
		~Server();


		void set_port(uint16_t p);

		void start();
	private:
		uint16_t port;

		http::Http http_manager;

		/**
		 * @brief Handle client connection
		 *
		 * @param rx_buf Receaving buffer
		 * @param tx_buf Transmitting buffer
		 */
		void handle(
			core::buf_t rx_buf,
			core::buf_t tx_buf
		);
	};
}

#endif
