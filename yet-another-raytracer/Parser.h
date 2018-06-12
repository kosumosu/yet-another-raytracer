#pragma once

#include <iostream>
#include <string>

class Parser
{
public:
	Parser(std::istream * stream);

	// Moves to the next command. If end of file is reached, returns false.
	bool MoveToNextCommand();
	
	const std::string & current_command() const { return _currentCommand; }

	std::size_t getCurrentLineNumber() const { return _currentLineNumber; }
private:
	std::istream * _stream;
	std::string _currentCommand;
	std::size_t _currentLineNumber;

	bool IsEol(char character) const;
	bool IsWhitespace(char character) const;
};

