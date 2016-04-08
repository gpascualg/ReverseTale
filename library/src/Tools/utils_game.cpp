#include "Tools/utils.h"
#include <cmath>


namespace Utils
{
	namespace Game
	{
		Session::Session()
		{
			reset();
		}

		void Session::reset()
		{
			_idHex = -1;
			_key = -1;
			_number = -1;
		}

		void Session::setID(uint32_t id)
		{
			_idHex = id;
			_number = Utils::encrypt_number(_idHex);
			_key = Utils::encrypt_key(_idHex);
		}

		void Session::setAlive(uint16_t alive)
		{
			_alive = alive - 1; // -1, it will be incremented on next call
		}

		uint16_t Session::alive()
		{
			uint16_t next = _alive + 1;
			++_alive;
			return next;
		}

		bool Session::check_alive(std::string& alive)
		{
			if (_alive + 1 == Utils::decimal_str2hex(alive))
			{
				++_alive;
				return true;
			}

			return false;
		}

		NString Session::next_alive()
		{
			++_alive;
			return NString() << _alive;
		}
	}
}
