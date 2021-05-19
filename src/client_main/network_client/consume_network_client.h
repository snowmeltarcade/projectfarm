#ifndef CONSUME_NETWORK_CLIENT_H
#define CONSUME_NETWORK_CLIENT_H

#include <memory>

namespace projectfarm::network_client
{
	class NetworkClient;

	class ConsumeNetworkClient
	{
	public:
		ConsumeNetworkClient() = default;
		virtual ~ConsumeNetworkClient() = default;

        [[nodiscard]] const std::shared_ptr<NetworkClient>& GetNetworkClient() const
		{
			return _networkClient;
		}

		void SetNetworkClient(const std::shared_ptr<NetworkClient>& networkClient)
		{
			this->_networkClient = networkClient;
		}

	private:
		std::shared_ptr<NetworkClient> _networkClient;
	};
}

#endif
