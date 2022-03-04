#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <unordered_map>
#include <functional>
#include <cstdint>
#include <chrono>

#include "buffer.hpp"
#include "socket.hpp"
#include "http.hpp"

namespace core {
	template <class T>
	class Server {
	public:
		// TODO: Pass cached data to endpoint function.
		typedef std::function<http::Response(http::Request, T &)> endpoint_fn;
		typedef std::function<void(T &)> task_fn;

		// Server();

		void set_port(uint16_t p) {
			port = p;
		}

		/**
		 * @brief Add single end point
		 *
		 * @param url
		 * @param fn Endpoint function
		 */
		void add_endpoint(std::string url, endpoint_fn fn) {
			endpoints.insert({url, fn});
		}

		/**
		 * @brief Add multiple endpoints
		 *
		 * @param endpoints
		 */
		void add_endpoints(std::vector<std::tuple<std::string, endpoint_fn>> eps) {
			for (auto e: eps) {
				endpoints.insert({std::get<0>(e), std::get<1>(e)});
			}
		}

		void add_periodic_task(const std::uint32_t timeout, task_fn task) {

			task_fns.push_back([this, timeout, task]() {
				for (;;) {
					task(data);
					std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
				}
			});

			/* tasks.push_back(std::thread([this, &timeout, &task]() {
				for (;;) {
					task(data);
					std::this_thread::sleep_for(std::chrono::);
				}
			})); */
		}

		void start() {
			for (auto task: task_fns) {
				task_threads.push_back(std::thread(task));
			}
			core::Socket::handle_function_t f = [this](buf_t a, buf_t b) { handle(a, b); };
			core::Socket s(f);
			s.start(port);
		}
	private:
		T data;

		uint16_t port;

		std::vector<std::function<void()>> task_fns;
		std::vector<std::thread> task_threads;

		http::Http http_manager;

		std::unordered_map<std::string, endpoint_fn> endpoints;

		/**
		 * @brief Handle client connection
		 *
		 * @param rx_buf Receaving buffer
		 * @param tx_buf Transmitting buffer
		 */
		void handle(
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
				version,
				method,
				url,
				headers,
			};

			Log(INFO) << "Request to: " << url;

			auto endpoint_search = endpoints.find(url);
			if (endpoint_search != endpoints.end()) {
				auto resp = endpoint_search->second(request, data).to_string(http_manager);
				tx_buf.push_chunk(std::vector<char>(resp.begin(), resp.end()));
			} else {
				Log(INFO) << "Endpoint with url: " << url << "does not exit.";
				auto resp = http::Response::bad_request("Unknown url\n").to_string(http_manager);
				tx_buf.push_chunk(std::vector<char>(resp.begin(), resp.end()));
			}

			// End of buffer stream
			char c = '\0';
			tx_buf.push(c);


			// TODO: Remove this after implementation.
			// (void)(tx_buf);
			// (void)(method);
		}




	};
}

#endif
