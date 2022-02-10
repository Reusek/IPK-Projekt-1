#include "http.hpp"

using namespace logger;

size_t http::longest(std::vector<std::string> a) {
	return std::max_element(
		a.begin(),
		a.end(),
		[](std::string a, std::string b) {
			return a.size() < b.size();
		}
	)->size();
}



http::Method::Method() {
	max_len = longest(string_type);
}

http::Method::Type http::Method::from_string(std::string &method) {
	auto search = map_type.find(method);
	if (search != map_type.end()) {
		return search->second;
	} else {
		Log(ERROR) << "Http method \"" << method << "\" does not exist!";
		return METHOD_ERROR;
	}
}


http::Version::Version() {
	max_len = longest(string_type);
}

http::Version::Type http::Version::from_string(std::string &version) {
	auto search = map_type.find(version);
	if (search != map_type.end()) {
		return search->second;
	} else {
		Log(ERROR) << "Http version \"" << version << "\" does not exist!";
		return VERSION_ERROR;
	}
}


http::Method::Type http::Http::parse_method(
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

http::Version::Type http::Http::parse_version(
	core::buf_t &rx_buf
) {
	char c = rx_buf.pop();
	std::string b;

	while (
		c != '\n' ||
		c != ' ' ||
		b.length() >= url_manager.max_len
	) {
		b.push_back(c);
		c = rx_buf.pop();
	}

	return version_manager.from_string(b);
}
