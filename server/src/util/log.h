#ifndef __LOG_H__
#define __LOG_H__

#include <string>
#include <vector>
#include <iomanip>
#include <sstream>

class Log {
	public:
		typedef enum {
			LogMsgDebug=0,
			LogMsgWarning,
			LogMsgInfo,
			LogMsgError
		} LogMsgType;

		class VectorStr {
			public:
				VectorStr(const std::vector<char> &vec) : v(vec){}
				const char& operator[] (size_t n) const { return this->v[n]; }
				size_t size() const { return this->v.size(); }
				std::vector<char>::const_iterator begin() const { return this->v.begin(); }
				std::vector<char>::const_iterator end() const { return this->v.end(); }

			protected:
				const std::vector<char> &v;
		};

		Log(LogMsgType type = LogMsgDebug);
		~Log();

		Log &operator<<(const VectorStr &v){
			if(Log::logLevel <= this->level)
				for(auto it = v.begin(); it != v.end(); it++)
					this->ss << (*it);
			return *this;
		}

		Log &operator<<(const std::vector<char> &v){
			if(Log::logLevel <= this->level)
				for(auto it = v.begin(); it != v.end(); it++)
					this->ss << std::setfill('0') << std::setw(2) << std::hex << (unsigned int) ((unsigned char) (*it));
			return *this;
		}

		Log &operator<<(const std::vector<unsigned char> &v){
			if(Log::logLevel <= this->level)
				for(auto it = v.begin(); it != v.end(); it++)
					this->ss << std::setfill('0') << std::setw(2) << std::hex << (unsigned int) (*it);
			return *this;
		}

		template<typename T>
			Log &operator<<(const T &t){
				if(Log::logLevel <= this->level)
					this->ss << t;
				return *this;
			}

		static void setLogLevel(LogMsgType);

		/**
		 * "debug" -> LogMsgDebug
		 * "warning" -> LogMsgWarning
		 * "info" -> LogMsgInfo
		 * "error" -> LogMsgError
		 */
		static void setLogLevel(const std::string&);

		static void setOutput(std::ostream&);

	protected:
		std::ostringstream ss; ///< Buffer
		LogMsgType level; ///< Nivel de log del mensaje

		static LogMsgType logLevel; ///< Nivel de log actual
		static std::ostream* output; ///< Output
};

#endif
