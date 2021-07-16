#ifndef NETWORKING_H
#define NETWORKING_H

namespace projectfarm::shared::networking
{
	class Networking final
	{
	public:
		Networking() = default;
		~Networking() = default;

		Networking(Networking&) = delete;
		Networking(Networking&&) = delete;

		bool Initialize();

		void Shutdown();
	};
}

#endif