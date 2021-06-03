#pragma once

#include <mutex>
#include <condition_variable>

class ThreadBarrier
{
	std::mutex _mutex;
	std::condition_variable _cv;
	std::size_t _count;
public:
	explicit ThreadBarrier(std::size_t count)
		: _count(count)
	{
	}

	void arrive_and_wait()
	{
		std::unique_lock lock{_mutex};
		if (--_count == 0)
		{
			_cv.notify_all();
		}
		else
		{
			_cv.wait(lock, [this] { return _count == 0; });
		}
	}

	void wait()
	{
		std::unique_lock lock{_mutex};
		_cv.wait(lock, [this] { return _count == 0; });
	}
};
