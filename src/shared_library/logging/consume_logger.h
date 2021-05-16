#ifndef CONSUME_LOGGER_H
#define CONSUME_LOGGER_H

#include <string>
#include <memory>
#include <mutex>

#include "logger.h"

namespace projectfarm::shared
{
	class ConsumeLogger
	{
	public:
		ConsumeLogger() = default;
		virtual ~ConsumeLogger() = default;

		void LogMessage(std::string_view s) const
		{
			if (!this->_logger)
            {
                return;
            }

			this->_logger->LogMessage(s);
		}

		void SetLogger(const std::shared_ptr<Logger>& logger)
		{
			this->_logger = logger;
		}

	protected:
		std::shared_ptr<Logger> _logger;
	};
}

#endif
