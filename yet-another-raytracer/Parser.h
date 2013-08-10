#pragma once

#include <iostream>
#include <string>

class Parser
{
public:
	Parser(std::istream * stream);
	~Parser(void);

	// Moves to the next command. If end of file is reached, returns false.
	bool MoveToNextCommand();
	
	const std::string & current_command() const { return m_current_command; }


private:
	std::istream * m_stream;
	std::string m_current_command;

	bool IsEol(char character) const;
	bool IsWhitespace(char character) const;
};

