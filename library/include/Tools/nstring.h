#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/include/std_pair.hpp>

// Generators
using boost::spirit::karma::generate;
using boost::spirit::karma::int_generator;
using boost::spirit::karma::uint_generator;


template<typename T>
struct is_literal { enum{value = false}; };

template<>
struct is_literal<char> { enum{value = true}; };

class NString
{
	friend class Tokenizer;

private:
	class Tokenizer
	{
		friend class NString;

	public:
		const int length() { return _tokens.size(); }
		const char* token(int idx) { return idx < length() ? _tokens[idx] : nullptr; }
		const char* operator[](int idx) { return token(idx); }
		
		template <typename T>
		T from_int(int idx)
		{
			const char* buffer = token(idx);
			T value = 0;
			if (buffer)
			{
				while (*buffer >= '0' && *buffer <= '9')
				{
					value = value * 10 + (*buffer - '0');
					++buffer;
				}
			}

			return value;
		}

		template <typename T>
		T from_hex(int idx)
		{
			const char* buffer = token(idx);
			T value = 0;
			if (buffer)
			{
				while (*buffer)
				{
					if (*buffer >= '0' && *buffer <= '9')
					{
						value = value << 4 | (*buffer - '0');
					}
					else if (*buffer >= 'A' && *buffer <= 'F')
					{
						value = value << 4 | (*buffer - 'A');
					}
					else
					{
						break;
					}

					++buffer;
				}
			}

			return value;
		}

	private:
		Tokenizer(NString* string);
		Tokenizer() {}

	private:
		std::vector<char*> _tokens;
	};

public:
	explicit NString(int size);
	explicit NString(const char* string);
	explicit NString(std::string& string);
	NString(const NString& nstring);
	~NString();

	// Constant attributes
	inline const char* get() { return _buffer; }
	inline const int length() { return _len; }
	inline const int capacity() { return _capacity; }

	inline NString clone(){ return NString(_buffer).copyTokensFrom(_tokenizer); }

	// Wrappers around numberic operators
	template <typename Type, unsigned char Base = 10>
	inline NString& append(Type number)
	{
		return operator<< <Type, Base> (number);
	}

	template <typename Type>
	inline NString& appendHex(Type number)
	{
		return operator<< <Type, 16> (number);
	}

	// String appending
	NString& operator<<(const char* string);
	NString& operator<<(std::string& string);
	NString& operator<<(char chr);

	// Number appending
	template <typename Type, unsigned char Base = 10>
	typename std::enable_if<std::is_unsigned<Type>::value, NString&>::type
	operator<<(Type number)
	{
		generate(_append, uint_generator<Type, Base>(), number);
		while (*_append)
		{
			assert(_append < _buffer + _capacity && "Not enough space");
			++_append;
			++_len;
		}

		return *this;
	}

	template <typename Type, unsigned char Base = 10>
	typename std::enable_if<std::is_signed<Type>::value && !is_literal<Type>::value, NString&>::type
	operator<<(Type number)
	{
		generate(_append, int_generator<Type, Base>(), number);
		while (*_append)
		{
			assert(_append < _buffer + _capacity && "Not enough space");
			++_append;
			++_len;
		}

		return *this;
	}

	Tokenizer& tokens();

private:
	NString& copyTokensFrom(Tokenizer* tokenizer);

private:
	char* _buffer;
	uint8_t* _refs;
	int _len;
	int _capacity;
	char* _append;

	Tokenizer* _tokenizer;
};
