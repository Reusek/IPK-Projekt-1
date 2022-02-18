#include "http.hpp"

using namespace logger;

size_t http::longest(std::vector<std::string> &a) {
	return std::max_element(
		a.begin(),
		a.end(),
		[](std::string a, std::string b) {
			return a.size() < b.size();
		}
	)->size();
}

http::Method::Method() {
	map = std::make_unique<std::unordered_map<std::string, http::MethodType>>(
		std::unordered_map<std::string, http::MethodType>({
			{"GET",     GET},
			{"HEAD",    HEAD},
			{"POST",    POST},
			{"PUT",     PUT},
			{"DELETE",  DELETE},
			{"TRACE",   TRACE},
			{"OPTIONS", OPTIONS},
			{"CONNECT", CONNECT},
			{"PATCH",   PATCH}
		})
	);
	max_len = load_map();
}

http::MethodType http::Method::from_string(std::string &method) {
	auto search = map->find(method);
	if (search != map->end()) {
		return search->second;
	} else {
		Log(ERROR) << "Http method \"" << method << "\" does not exist!";
		return METHOD_ERROR;
	}
}

std::string http::Method::to_string(http::MethodType m) {
	(void)(m);
	return "";
}


http::Version::Version() {
	// max_len = longest(string_type);
	map = std::make_unique<std::unordered_map<std::string, http::VersionType>>(
		std::unordered_map<std::string, http::VersionType>({
			{"HTTP/1.0", HTTP_1_0},
			{"HTTP/1.1", HTTP_1_1},
			{"HTTP/2.0", HTTP_2_0},
		})
	);
	max_len = load_map();
}

http::VersionType http::Version::from_string(std::string &version) {
	auto search = map->find(version);
	if (search != map->end()) {
		return search->second;
	} else {
		Log(ERROR) << "Http version \"" << version << "\" does not exist!";
		return VERSION_ERROR;
	}
}

std::string http::Version::to_string(http::VersionType m) {
	(void)(m);
	return "";
}

std::string http::Header::to_string(std::tuple<std::string, std::string> header) {
	return std::get<0>(header) + ": " + std::get<1>(header);
}

std::tuple<std::string, std::string> http::Header::from_string(std::string &header) {
	size_t pos = 0;
	std::string raw_header;
	std::tuple<std::string, std::string> h;

	if ((pos = header.find(": ")) != std::string::npos) {
		return std::make_tuple(header.substr(0, pos), header.substr(pos + 2, header.length() - 1));
	} else {
		return std::make_tuple(std::string(""), std::string(""));
	}
}


http::MethodType http::Http::parse_method(
	core::buf_t &rx_buf
) {
	char c = rx_buf.pop();
	std::string b;

	while (c != ' ' || b.length() >= method_manager.max_len) {
		b.push_back(c);
		c = rx_buf.pop();
	}

	return method_manager.from_string(b);
}

std::string http::Http::parse_url(
	core::buf_t &rx_buf
) {
	char c = rx_buf.pop();
	std::string b;

	while (c != ' ' || b.length() >= url_manager.max_len) {
		b.push_back(c);
		c = rx_buf.pop();
	}

	if (b.find_first_not_of(url_manager.allowed_chars) != std::string::npos) {
		return "";
	}

	return b;
}

http::VersionType http::Http::parse_version(
	core::buf_t &rx_buf
) {
	char c = rx_buf.pop();
	std::string b;

	while (
		c != '\n' &&
		c != ' ' &&
		b.length() <= url_manager.max_len
	) {
		if (c == '\r') {
			c = rx_buf.pop();
			continue;
		}
		b.push_back(c);
		c = rx_buf.pop();
	}

	return version_manager.from_string(b);
}

std::vector<std::tuple<std::string, std::string>>
http::Http::parse_headers(
	core::buf_t rx_buf
) {
	std::vector<std::tuple<std::string, std::string>> headers;

	char c = rx_buf.pop();

	while (true) {
		std::string s;
		while (
			c != '\n' &&
			s.length() <= header_manager.max_len
		) {
			// Skip CRLF
			if (c == '\r') {
				c = rx_buf.pop();
				continue;
			}

			s.push_back(c);
			c = rx_buf.pop();
		}

		if (rx_buf.peak() == '\r' || rx_buf.peak() == '\n') {
			return headers;
		}

		headers.push_back(header_manager.from_string(s));
	}

	return headers;
}

std::string http::Response::to_string() {

}

http::Response http::Response::ok(std::string body) {

}

http::Response http::Response::bad_request(std::string body) {
	http::Response resp;

	Response::init(resp);
	resp.body = body;
	Response::prep(resp);

	return resp;
}

/**
 * @brief Generate Date for http response
 *
 * Stolen from: https://stackoverflow.com/a/7548846
 */
std::string gen_time() {
	char buf[1000];
	time_t now = time(0);
	struct tm tm = *gmtime(&now);
	strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &tm);
	return std::string(buf);
}

void http::Response::init(http::Response &resp) {
	resp.headers.push_back({ std::string("Access-Control-Allow-Origin"), std::string("*") });
	resp.headers.push_back({ std::string("Content-Type"), std::string("text/html; charset=UTF-8") });
	resp.headers.push_back({ std::string("Date"), gen_time() });
}

void http::Response::init(http::Response &resp) {
	resp.headers.push_back({ std::string("Content-Length"), std::to_string(resp.body.length()) });
}
