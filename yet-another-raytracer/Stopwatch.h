#pragma once

class Stopwatch
{
public:
	virtual ~Stopwatch() = default;

	virtual void Restart() = 0;
	virtual float Sample() const = 0;
};