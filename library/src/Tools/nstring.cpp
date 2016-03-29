#include "Tools/nstring.h"

NString::NString()
{
	_buffer = new fmt::MemoryWriter();
	_refs = new uint8_t;
	*_refs = 1;
}

NString::NString(const char* string):
	NString()
{
	*_buffer << string;
}

NString::NString(std::string& string):
	NString(string.c_str())
{}

NString::NString(const NString& nstring)
{
	_buffer = nstring._buffer;
	_refs = nstring._refs;
	*_refs = *_refs + 1;
}

NString& NString::operator= (const NString& nstring)
{
	_buffer = nstring._buffer;
	_refs = nstring._refs;
	*_refs = *_refs + 1;

	return *this;
}

NString::~NString()
{
	*_refs = *_refs - 1;
	if (*_refs == 0)
	{
		delete _refs;
		delete _buffer;
	}
}

NString::Tokenizer& NString::tokens(char delimiter)
{
	if (_tokenizer == nullptr)
	{
		_tokenizer = new Tokenizer(this, delimiter);
	}

	return *_tokenizer;
}

NString::Tokenizer::Tokenizer(NString* string, char delimiter)
{
	// FIXME: Evil cast from const to non-const
	char* buffer = (char*)string->_buffer->c_str();
	char* current = buffer;

	while (*buffer)
	{
		if (*buffer == delimiter)
		{
			*buffer = '\0';
			_tokens.push_back(current);
			current = buffer + 1;
		}
		++buffer;
	}

	if (current != buffer)
	{
		_tokens.push_back(current);
	}
}

NString& NString::copyTokensFrom(Tokenizer* tokenizer)
{
	if (_tokenizer == nullptr)
	{
		_tokenizer = new Tokenizer();
	}
	_tokenizer->_tokens = tokenizer->_tokens;

	return *this;
}
