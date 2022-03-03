#ifndef __HTTP_HPP__
#define __HTTP_HPP__

#include <iostream>
#include <unistd.h>
#include <functional>
#include <tuple>
#include <algorithm>
#include <unordered_map>
#include <time.h>

// Related to sockets
#include <sys/socket.h>
#include <arpa/inet.h>

#include "logger.hpp"
#include "pool.hpp"
#include "buffer.hpp"
#include "socket.hpp"

namespace http {
	size_t longest(std::vector<std::string> &a);

	template <class T>
	class Parser {
	public:
		virtual std::string to_string(T &method) = 0;
		virtual T from_string(std::string &method) = 0;
	};

	template <class T>
	class MapParser {
	public:
		MapParser() = default;
	protected:
		std::unique_ptr<std::vector<std::string>> keys{};
		std::unique_ptr<std::unordered_map<std::string, T>> map_to{};
		std::unique_ptr<std::unordered_map<T, std::string>> map_from{};

		/**
		 * @brief Init HashMap.
		 *
		 * @return size_t Longest key
		 */
		size_t load_map() {
			keys = std::make_unique<std::vector<std::string>>(map_to->size());

			std::transform(
				map_to->begin(),
				map_to->end(),
				keys->begin(),
				[](auto pair) { return pair.first; });

			return longest(*keys);
		}
	};


	enum MethodType {
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

	class Method :
		Parser<MethodType>,
		MapParser<MethodType>
	{
	public:
		size_t max_len;

		Method();

		std::string to_string(MethodType &method) override;
		MethodType from_string(std::string &method) override;
	};


	class Url {
	public:
		/**
		 * @brief Serialize http version
		 *
		 * @param version
		 * @return std::string
		 */
		static std::string to_string(std::string &path);

		/**
		 * @brief Deserialize http version
		 *
		 * @param version
		 * @return Type
		 */
		std::string from_string(std::string &path);

		// To support IE11
		// https://stackoverflow.com/a/417184
		const size_t max_len = 2047;

		// TODO: Create this as HashMap conver O(n) to O(1)
		const char * allowed_chars =
			"abcdefghijklmnopqrstuvwxyz"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"0123456789"
			"/&?=-";
	};


	enum VersionType {
		HTTP_1_0,
		HTTP_1_1,
		HTTP_2_0,
		VERSION_ERROR
	};

	class Version :
		public Parser<VersionType>,
		public MapParser<VersionType>
	{
	public:
		Version();

		/**
		 * @brief Serialize http version
		 *
		 * @param version
		 * @return std::string
		 */
		std::string to_string(http::VersionType &version) override;

		/**
		 * @brief Deserialize http version
		 *
		 * @param version
		 * @return Type
		 */
		http::VersionType from_string(std::string &version) override;

		size_t max_len;
	};

	enum HeaderType {
		CORS
	};

	class Header :
		public Parser<std::tuple<std::string, std::string>>
	{
	public:
		/**
		 * @brief Serialize http version
		 *
		 * @param version
		 * @return std::string
		 */
		std::string to_string(std::tuple<std::string, std::string> &header) override;

		/**
		 * @brief Deserialize http version
		 *
		 * @param version
		 * @return Type
		 */
		std::tuple<std::string, std::string> from_string(std::string &header) override;

		size_t max_len = 1024;
	};


	enum StatusType {
		// 2xx,
		OK,

		// 4xx,
		BAD_REQUEST,
		FORBIDDEN,
		NOT_FOUND,

		// 5xx,
		INTERNAL_SERVER_ERROR,
		NOT_IMPLEMENTED,
		STATUS_ERROR
	};


	class Status :
		public Parser<StatusType>,
		public MapParser<StatusType>
	{
	public:
		Status();

		/**
		 * @brief Serialize http version
		 *
		 * @param version
		 * @return std::string
		 */
		std::string to_string(StatusType &type) override;

		/**
		 * @brief Deserialize http version
		 *
		 * @param version
		 * @return Type
		 */
		StatusType from_string(std::string &type) override;

		size_t max_len;
	};

	struct Request {
		VersionType version;
		MethodType method;
		std::string url;
		std::vector<std::tuple<std::string, std::string>> headers;
	};

	class Http {
	public:

		/**
		 * \addtogroup Parsers
		 * @{
		 */

		/**
		 * @brief HTTP Method parser.
		 *
		 * @param rx_buf Receive buffer.
		 * @return http::MethodType
		 */
		http::MethodType parse_method(core::buf_t rx_buf);

		/**
		 * @brief HTTP URL Parser
		 *
		 * @param rx_buf Receive buffer.
		 * @return std::string
		 */
		std::string parse_url(core::buf_t rx_buf);

		/**
		 * @brief HTTP Version parser.
		 *
		 * @param rx_buf Receive buffer.
		 * @return http::VersionType
		 */
		http::VersionType parse_version(core::buf_t rx_buf);

		/**
		 * @brief HTTP Headers parser.
		 * @note Parse all headers to vector.
		 *
		 * @param rx_buf Receive buffer.
		 * @return std::vector<std::tuple<std::string, std::string>>
		 */
		std::vector<std::tuple<std::string, std::string>> parse_headers(core::buf_t rx_buf);

		/**
		 * @}
		 */

		/**
		 * \addtogroup Managers
		 * @{
		 */

		/**
		 * @brief HTTP Method manager.
		 */
		http::Method method_manager;

		/**
		 * @brief HTTP URL manager.
		 */
		http::Url url_manager;

		/**
		 * @brief HTTP Version manager.
		 */
		http::Version version_manager;

		/**
		 * @brief HTTP Header manager.
		 */
		http::Header header_manager;

		/**
		 * @}
		 */
	};

	/**
	 * @brief Generate Date for http response
	 *
	 * Stolen from: https://stackoverflow.com/a/7548846
	 */
	static std::string gen_time() {
		char buf[1000];
		time_t now = time(0);
		struct tm tm = *gmtime(&now);
		strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &tm);
		return std::string(buf);
	}

	struct Response {
		VersionType version;
		MethodType method;
		std::string url;
		std::vector<std::tuple<std::string, std::string>> headers;
		std::string body;

		/**
		 * @brief Serialize Response.
		 *
		 * @param http_manager
		 * @return std::string
		 */
		std::string to_string(http::Http &http_manager);

		/**
		 * @brief Create basic 200 response with body
		 *
		 * @param body Response body
		 * @return Response
		 */
		static Response ok(std::string body);

		/**
		 * @brief Create basic bad request response
		 *
		 * @param body Response body
		 * @return Response
		 */
		static Response bad_request(std::string body);

		/**
		 * @brief Init response.
		 * @note Called before every request creation. To be sure
		 *       everithing is initialized.
		 *
		 * @param response
		 */
		static void init(Response &response);

		/**
		 * @brief Preper response to sending.
		 * @note Called at the end of response creation.
		 *       Adds needed headers.
		 *
		 * @param response
		 */
		static void prep(Response &response);
	};
}



#endif

