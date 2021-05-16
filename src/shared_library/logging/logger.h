#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
#include <mutex>

namespace projectfarm::shared
{
	class Logger final
	{
	public:
	    Logger() = default;
	    ~Logger() = default;

		void LogMessage(std::string_view s) const noexcept;

	private:
		mutable std::mutex _logMessageMutex;
	};
}

#endif