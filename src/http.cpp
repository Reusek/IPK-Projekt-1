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
	map_to = std::make_unique<std::unordered_map<std::string, http::MethodType>>(
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

	map_from = std::make_unique<std::unordered_map<http::MethodType, std::string>>(
		std::unordered_map<http::MethodType, std::string>({
			{GET,     "GET"},
			{HEAD,    "HEAD"},
			{POST,    "POST"},
			{PUT,     "PUT"},
			{DELETE,  "DELETE"},
			{TRACE,   "TRACE"},
			{OPTIONS, "OPTIONS"},
			{CONNECT, "CONNECT"},
			{PATCH,   "PATCH"}
		})
	);
	max_len = load_map();
}

http::MethodType http::Method::from_string(std::string &method) {
	auto search = map_to->find(method);
	if (search != map_to->end()) {
		return search->second;
	} else {
		Log(ERROR) << "Http method \"" << method << "\" does not exist!";
		return METHOD_ERROR;
	}
}

std::string http::Method::to_string(http::MethodType &m) {
	auto search = map_from->find(m);
	if (search != map_from->end()) {
		return search->second;
	} else {
		Log(ERROR) << "Http method \"" << m << "\" does not exist!";
		return "";
	}
}


http::Version::Version() {
	// max_len = longest(string_type);
	map_to = std::make_unique<std::unordered_map<std::string, http::VersionType>>(
		std::unordered_map<std::string, http::VersionType>({
			{"HTTP/1.0", HTTP_1_0},
			{"HTTP/1.1", HTTP_1_1},
			{"HTTP/2.0", HTTP_2_0},
		})
	);

	map_from = std::make_unique<std::unordered_map<http::VersionType, std::string>>(
		std::unordered_map<http::VersionType, std::string>({
			{HTTP_1_0, "HTTP/1.0"},
			{HTTP_1_1, "HTTP/1.1"},
			{HTTP_2_0, "HTTP/2.0"},
		})
	);

	max_len = load_map();
}

http::VersionType http::Version::from_string(std::string &version) {
	auto search = map_to->find(version);
	if (search != map_to->end()) {
		return search->second;
	} else {
		Log(ERROR) << " [" << __func__ << "] " <<  "Http version \"" << version << "\" does not exist!";
		return VERSION_ERROR;
	}
}

std::string http::Version::to_string(http::VersionType &m) {
	auto search = map_from->find(m);
	if (search != map_from->end()) {
		return search->second;
	} else {
		Log(ERROR) << " [" << __func__ << "] " << "Http version \"" << m << "\" does not exist!";
		return "";
	}
}

std::string http::Header::to_string(std::tuple<std::string, std::string> &header) {
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

http::Status::Status() {
	map_to = std::make_unique<std::unordered_map<std::string, http::StatusType>>(
		std::unordered_map<std::string, http::StatusType>({
			{"200 Ok",                    OK},
			{"400 Bad Request",           BAD_REQUEST},
			{"403 Forbidden",             FORBIDDEN},
			{"404 Not Found",             NOT_FOUND},
			{"500 Internal Server Error", INTERNAL_SERVER_ERROR},
			{"501 Not Implemented",       NOT_IMPLEMENTED}
		})
	);

	map_from = std::make_unique<std::unordered_map<http::StatusType, std::string>>(
		std::unordered_map<http::StatusType, std::string>({
			{OK,                    "200 Ok"},
			{BAD_REQUEST,           "400 Bad Request"},
			{FORBIDDEN,             "403 Forbidden"},
			{NOT_FOUND,             "404 Not Found"},
			{INTERNAL_SERVER_ERROR, "500 Internal Server Error"},
			{NOT_IMPLEMENTED,       "501 Not Implemented"}
		})
	);

	max_len = load_map();
}

http::StatusType http::Status::from_string(std::string &type) {
	auto search = map_to->find(type);
	if (search != map_to->end()) {
		return search->second;
	} else {
		Log(ERROR) << "Http status \"" << type << "\" does not exist!";
		return STATUS_ERROR;
	}
}

std::string http::Status::to_string(http::StatusType &m) {
	auto search = map_from->find(m);
	if (search != map_from->end()) {
		return search->second;
	} else {
		Log(ERROR) << "Http status \"" << m << "\" does not exist!";
		return "";
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


	while (true) {
		std::string s;
		char c = rx_buf.pop();
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
			rx_buf.pop();
			return headers;
		}

		headers.push_back(header_manager.from_string(s));
	}

	return headers;
}

std::string http::Response::to_string(http::Http &http_manager) {
	std::string buffer;

	buffer += http_manager.version_manager.to_string(version) + " ";
	// TODO: Response code
	buffer += "200 OK";
	buffer += "\r\n";

	for (auto header : headers) {
		buffer += http_manager.header_manager.to_string(header);
		buffer += "\r\n";
	}

	buffer += "\r\n";

	buffer += body;

	return buffer;
}

http::Response http::Response::ok(std::string body) {
	http::Response resp;

	Response::init(resp);
	resp.body = body;
	Response::prep(resp);

	return resp;
}

http::Response http::Response::bad_request(std::string body) {
	http::Response resp;

	Response::init(resp);
	resp.body = body;
	Response::prep(resp);

	return resp;
}
