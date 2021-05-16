#ifndef CONSUME_CRYPTO_PROVIDER_H
#define CONSUME_CRYPTO_PROVIDER_H

#include <memory>

#include "crypto_provider.h"

namespace projectfarm::shared::crypto
{
	class ConsumeCryptoProvider
	{
	public:
        ConsumeCryptoProvider() = default;
		virtual ~ConsumeCryptoProvider() = default;

		void SetCryptoProvider(const std::shared_ptr<CryptoProvider>& cryptoProvider) noexcept
		{
			this->_cryptoProvider = cryptoProvider;
		}

	protected:
		std::shared_ptr<CryptoProvider> _cryptoProvider;
	};
}

#endif
