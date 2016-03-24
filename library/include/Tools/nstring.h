#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <cppformat/posix.h>


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
	NString();
	explicit NString(const char* string);
	explicit NString(std::string& string);
	NString(const NString& nstring);
	~NString();

	// Constant attributes
	// TODO: Getters
	inline const char* get() { return _buffer->c_str(); }
	//inline const int length() { return _len; }
	//inline const int capacity() { return _capacity; }
	//inline NString clone(){ return NString(_buffer).copyTokensFrom(_tokenizer); }

	// Wrappers around numberic operators
	template <typename Type>
	inline NString& append(Type number)
	{
		return operator<< <Type> (number);
	}

	template <typename Type>
	inline NString& appendHex(Type number)
	{
		return operator<< <Type> (fmt::hex(number));
	}

	// Appending
	template <typename Type>
	NString& operator<<(Type val)
	{
		*_buffer << val;
		return *this;
	}

	Tokenizer& tokens();

private:
	NString& copyTokensFrom(Tokenizer* tokenizer);

private:
	Tokenizer* _tokenizer;
	fmt::MemoryWriter* _buffer;

	uint8_t* _refs;
};
