#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <iostream>
#include <unistd.h>
#include <functional>
#include <tuple>
#include <algorithm>
#include <unordered_map>

// Related to sockets
#include <sys/socket.h>
#include <arpa/inet.h>

#include "logger.hpp"
#include "pool.hpp"
#include "buffer.hpp"
#include "socket.hpp"

#include "logger.hpp"

namespace http {

	size_t longest(std::vector<std::string> a);

	class Method {
	public:
		enum Type {
			GET,
			HEAD,
			POST,
			PUT,
			DELETE,
			TRACE,
			OPTIONS,
			CONNECT,
			PATCH,
			METHOD_ERROR
		};

		size_t max_len;

		Method();

		std::string to_string(Type method);
		Type from_string(std::string &method);

	private:
		const std::vector<std::string> string_type = {
			"GET",
			"HEAD",
			"POST",
			"PUT",
			"DELETE",
			"TRACE",
			"OPTIONS",
			"CONNECT",
			"PATCH"
		};
		const std::unordered_map<std::string, Type> map_type = {
			{"GET",     GET},
			{"HEAD",    HEAD},
			{"POST",    POST},
			{"PUT",     PUT},
			{"DELETE",  DELETE},
			{"TRACE",   TRACE},
			{"OPTIONS", OPTIONS},
			{"CONNECT", CONNECT},
			{"PATCH",   PATCH}
		};
	};

	class Url {
	public:
		Url() = default;
		~Url() = default;

		/**
		 * @brief Serialize http version
		 *
		 * @param version
		 * @return std::string
		 */
		std::string to_string(std::string &path);

		/**
		 * @brief Deserialize http version
		 *
		 * @param version
		 * @return Type
		 */
		std::string from_string(std::string &path);

		// To support IE11
		// https://stackoverflow.com/questions/417142/what-is-the-maximum-length-of-a-url-in-different-browsers?answertab=active#tab-top
		const size_t max_len = 2047;

		const char * allowed_chars =
			"abcdefghijklmnopqrstuvwxyz"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"0123456789"
			"/&?=";
	};

	class Version {
	public:
		Version();

		enum Type {
			HTTP_1_0,
			HTTP_1_1,
			HTTP_2_0,
			VERSION_ERROR
		};

		/**
		 * @brief Serialize http version
		 *
		 * @param version
		 * @return std::string
		 */
		std::string to_string(Type version);

		/**
		 * @brief Deserialize http version
		 *
		 * @param version
		 * @return Type
		 */
		Type from_string(std::string &version);

		size_t max_len;
	private:
		const std::vector<std::string> string_type = {
			"HTTP/1.0",
			"HTTP/1.1",
			"HTTP/2.0",
		};

		const std::unordered_map<std::string, Type> map_type = {
			{"HTTP/1.0", HTTP_1_0},
			{"HTTP/1.1", HTTP_1_1},
			{"HTTP/2.0", HTTP_2_0},
		};
	};

	class Request {

	};

	class Response {

	};

	class Http {
	public:
		Method::Type parse_method(core::buf_t rx_buf);
		std::string parse_url(core::buf_t rx_buf);
		Version::Type parse_version(core::buf_t rx_buf);
	};
}



#endif

