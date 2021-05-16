#ifndef CONSUME_SERVER_H
#define CONSUME_SERVER_H

#include <memory>

namespace projectfarm::server
{
	class Server;
	
	class ConsumeServer
	{
	public:
		ConsumeServer() = default;
		virtual ~ConsumeServer() = default;

		[[nodiscard]] const std::shared_ptr<Server>& GetServer() const
		{
			return this->_server;
		}

		void SetServer(const std::shared_ptr<Server>& server)
		{
			this->_server = server;
		}

	private:
		std::shared_ptr<Server> _server;
	};
}

#endif