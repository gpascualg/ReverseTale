#include "Tools/nstring.h"

NString::NString()
{
	_tokenizer = nullptr;
	_buffer = new fmt::MemoryWriter();
	_refs = new uint8_t;
	*_refs = 1;
}

NString::NString(const char* string):
	NString()
{
	*_buffer << string;
}

NString::NString(const char* string, int len) :
	NString()
{
	while (length() < (std::size_t)len)
	{
		const char *ptr = string + length();
		if (*ptr == 0)
		{
			*_buffer << (char)'\0';
			++ptr;
		}

		*_buffer << ptr;
	}
}

NString::NString(std::string& string):
	NString(string.c_str())
{}

NString::NString(const NString& nstring)
{
	_tokenizer = nstring._tokenizer;
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

NString::Tokenizer& NString::tokens(uint8_t delimiter)
{
	if (_tokenizer == nullptr)
	{
		_tokenizer = new Tokenizer(this, delimiter);
	}

	return *_tokenizer;
}

NString::Tokenizer::Tokenizer(NString* string, uint8_t delimiter)
{
	// FIXME: Evil cast from const to non-const
	char* buffer = (char*)string->_buffer->c_str();
	char* current = buffer;

	while (*buffer)
	{
		if ((uint8_t)*buffer == delimiter)
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
