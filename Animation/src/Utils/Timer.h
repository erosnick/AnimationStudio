// Timer.h

#pragma once

#include <chrono>
#include <iostream>

namespace Util
{
	class Timer
	{
	public:

		Timer()
		{
			start = std::chrono::high_resolution_clock::now();
		}

		inline void stop()
		{
			end = std::chrono::high_resolution_clock::now();

			std::chrono::duration<float> duration = end - start;

			float ms = duration.count() * 1000.0f;

			std::cout << "Timer took " << ms << "ms" << std::endl;
		}

		~Timer()
		{
			stop();
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> start;
		std::chrono::time_point<std::chrono::high_resolution_clock> end;
	};
}