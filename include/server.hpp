#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <unordered_map>
#include <functional>

#include "buffer.hpp"
#include "socket.hpp"
#include "http.hpp"

namespace core {
	// TODO: Pass cached data to endpoint function.
	using endpoint_fn = std::function<http::Response(http::Request)>;

	class Server {
	public:
		Server();

		void set_port(uint16_t p);

		/**
		 * @brief Add single end point
		 *
		 * @param url
		 * @param fn Endpoint function
		 */
		void add_endpoint(std::string url, endpoint_fn fn);

		/**
		 * @brief Add multiple endpoints
		 *
		 * @param endpoints
		 */
		void add_endpoints(std::vector<std::tuple<std::string, endpoint_fn>> endpoints);

		void add_periodic_task(u_int32_t timeout);

		void start();
	private:
		uint16_t port;

		http::Http http_manager;

		std::unordered_map<std::string, endpoint_fn> endpoints;

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
