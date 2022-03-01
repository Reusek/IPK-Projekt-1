#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <iostream>

namespace logger {


	enum LevelType {
		DEBUG,
		INFO,
		WARN,
		ERROR
	};

	struct Confing
	{
		bool extended = true;
		LevelType level = WARN;
		const char* component = "root";
	};

	class Log {
	public:
		Log() {}
		Log(LevelType level) {
			msg_level = level;
			if (true) {
				operator << (getLabel(level));
			}
		}

		~Log() {
			if(opened) {
				std::cout << std::endl;
			}
			opened = false;
		}

		template<class T>
		Log &operator<<(const T &msg) {
			// if(msg_level >= LOGCFG.level) {
				std::cout << msg;
				opened = true;
			// }
			return *this;
		}
	private:
		bool opened = false;
		LevelType msg_level = DEBUG;

		inline std::string getLabel(LevelType type) {
			std::string label;
			switch(type) {
				case DEBUG:
					label += magenta + std::string("[") + reset;
					label += b_magenta + std::string("DEBUG") + reset;
					label += magenta + std::string("]") + reset;
					break;
				case INFO:
					// label = green + std::string(" INFO") + reset;
					label += green + std::string("[") + reset;
					label += b_green + std::string(" INFO") + reset;
					label += green + std::string("]") + reset;
					break;
				case WARN:
					// label = yellow + std::string(" WARN") + reset;
					label += yellow + std::string("[") + reset;
					label += b_yellow + std::string(" WARN") + reset;
					label += yellow + std::string("]") + reset;
					break;
				case ERROR:
					// label = red + std::string("ERROR") + reset;
					label += red + std::string("[") + reset;
					label += b_red + std::string("ERROR") + reset;
					label += red + std::string("]") + reset;
					break;
			}
			return label;
		}

		const char* black   = "\u001b[30m";
		const char* red     = "\u001b[31m";
		const char* green   = "\u001b[32m";
		const char* yellow  = "\u001b[33m";
		const char* blue    = "\u001b[34m";
		const char* magenta = "\u001b[35m";
		const char* cyan    = "\u001b[36m";
		const char* white   = "\u001b[37m";

		const char* b_black   = "\u001b[30;1m";
		const char* b_red     = "\u001b[31;1m";
		const char* b_green   = "\u001b[32;1m";
		const char* b_yellow  = "\u001b[33;1m";
		const char* b_blue    = "\u001b[34;1m";
		const char* b_magenta = "\u001b[35;1m";
		const char* b_cyan    = "\u001b[36;1m";
		const char* b_white   = "\u001b[37;1m";

		const char* reset   = "\u001b[0m";

	};

	/* #define debug logger::Log(logger::LevelType::DEBUG);
	#define info  logger::Log(logger::LevelType::INFO);
	#define warn  logger::Log(logger::LevelType::WARN);
	#define error logger::Log(logger::LevelType::ERROR); */
}

#endif

