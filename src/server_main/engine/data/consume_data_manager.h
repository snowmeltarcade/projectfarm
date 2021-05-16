#ifndef CONSUME_DATA_MANAGER_H
#define CONSUME_DATA_MANAGER_H

#include <memory>

#include "data_manager.h"

namespace projectfarm::engine::data
{
	class ConsumeDataManager
	{
	public:
        ConsumeDataManager() = default;
		virtual ~ConsumeDataManager() = default;

		void SetDataManager(const std::shared_ptr<DataManager>& dataManager)
		{
			this->_dataManager = dataManager;
		}

	protected:
		std::shared_ptr<DataManager> _dataManager;
	};
}

#endif
