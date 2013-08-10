#include "Parser.h"

const char eol_chars[] = { '\n', '\r' };
const char whitespace_chars[] = { ' ', '\t' };

Parser::Parser( std::istream * stream )
	: m_stream(stream)
{

}


Parser::~Parser(void)
{
}

bool Parser::MoveToNextCommand()
{
	bool parsing_comment = false;
	char character;
	do
	{
		m_stream->get(character);

		if (m_stream->eof())
			return false;

		if (parsing_comment)
		{
			if (IsEol(character))
				parsing_comment = false;
		}
		else
		{
			if (character == '#')
			{
				parsing_comment = true;
			}
			else
			{
				if (!IsEol(character) && !IsWhitespace(character))
					break;
			}
		}
		
	} while (true);

	std::string command;

	*m_stream >> command;

	m_current_command = character + command;

	return true;
}

bool Parser::IsEol( char character ) const
{
	for (size_t i = 0; i < sizeof(eol_chars) / sizeof(char); i++)
	{
		if (character == eol_chars[i])
			return true;
	}
	return false;
}

bool Parser::IsWhitespace( char character ) const
{
	for (size_t i = 0; i < sizeof(whitespace_chars) / sizeof(char); i++)
	{
		if (character == whitespace_chars[i])
			return true;
	}
	return false;
}
