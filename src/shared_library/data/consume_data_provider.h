#ifndef CONSUME_DATA_PROVIDER_H
#define CONSUME_DATA_PROVIDER_H

#include <string>
#include <memory>
#include <mutex>

#include "data_provider.h"

namespace projectfarm::shared
{
	class ConsumeDataProvider
	{
	public:
        ConsumeDataProvider() = default;
		virtual ~ConsumeDataProvider() = default;

		void SetDataProvider(const std::shared_ptr<DataProvider>& dataProvider)
		{
			this->_dataProvider = dataProvider;
		}

	protected:
		std::shared_ptr<DataProvider> _dataProvider;
	};
}

#endif
