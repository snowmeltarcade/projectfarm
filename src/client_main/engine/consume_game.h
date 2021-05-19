#ifndef CONSUME_GAME_H
#define CONSUME_GAME_H

#include <memory>

namespace projectfarm::engine
{
	class Game;

	class ConsumeGame
	{
	public:
		ConsumeGame() = default;
		virtual ~ConsumeGame() = default;

        [[nodiscard]] const std::shared_ptr<Game>& GetGame() const
		{
			return _game;
		}

		void SetGame(const std::shared_ptr<Game>& game)
		{
			this->_game = game;
		}

	private:
		std::shared_ptr<Game> _game;
	};
}

#endif
