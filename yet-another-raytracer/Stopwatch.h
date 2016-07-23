#pragma once

class Stopwatch
{
public:
	virtual ~Stopwatch() {}

	virtual void Restart() = 0;
	virtual float Sample() const = 0;
};