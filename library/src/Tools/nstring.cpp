#include "Tools/nstring.h"


NString::NString(int size)
{
	_buffer = new char[size];
	_append = _buffer;
	_len = 0;
	_capacity = size;
	_tokenizer = nullptr;

	memset(_buffer, 0, size);
}

NString::NString(const char* string)
{
	_len = strlen(string);
	_buffer = new char[_len + 1];
	_append = _buffer + _len;
	_tokenizer = nullptr;

	*this << string;

	_refs = new uint8_t;
	*_refs = 1;
}

NString::NString(std::string& string):
	NString(string.c_str())
{}

NString::NString(const NString& nstring)
{
	_len = nstring._len;
	_buffer = nstring._buffer;
	_append = nstring._append;
	_refs = nstring._refs;
	_tokenizer = nstring._tokenizer;
	++(*_refs);
}

NString::~NString()
{
	if (--(*_refs) == 0)
	{
		delete[] _buffer;
		delete _refs;
	}
}

NString& NString::operator<<(const char* string)
{
	while (*string)
	{
		assert(_append < _buffer + _capacity && "Not enough space");

		*_append = *string;
		++_append;
		++_len;
		++string;
	}

	*_append = '\0';
	return *this;
}

NString& NString::operator<<(std::string& string)
{
	return *this << string.c_str();
}

NString& NString::operator<<(char chr)
{
	assert(_append < _buffer + _capacity && "Not enough space");

	*_append = chr;
	++_append;
	++_len;
	return *this;
}

NString::Tokenizer& NString::tokens()
{
	if (_tokenizer == nullptr)
	{
		_tokenizer = new Tokenizer(this);
	}

	return *_tokenizer;
}

NString::Tokenizer::Tokenizer(NString* string)
{
	// Do a copy
	char* buffer = string->_buffer;
	char* current = buffer;

	while (*buffer)
	{
		if (*buffer == ' ')
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
}
