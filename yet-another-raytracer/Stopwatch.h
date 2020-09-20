#pragma once

class Stopwatch
{
public:
	virtual ~Stopwatch() = default;

	virtual void Restart() = 0;
	[[nodiscard]] virtual float Sample() const = 0;
};