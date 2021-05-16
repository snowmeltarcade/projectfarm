#ifndef CONSUME_RANDOM_ENGINE_H
#define CONSUME_RANDOM_ENGINE_H

#include <memory>

#include "random_engine.h"

namespace projectfarm::shared::math
{
	class ConsumeRandomEngine
	{
	public:
        ConsumeRandomEngine() = default;
		virtual ~ConsumeRandomEngine() = default;

		void SetRandomEngine(const std::shared_ptr<RandomEngine>& randomEngine)
		{
			this->_randomEngine = randomEngine;
		}

	protected:
		std::shared_ptr<RandomEngine> _randomEngine;
	};
}

#endif
