#include "Tools/utils.h"
#include <cmath>


namespace Utils
{
	namespace Game
	{
		Session::Session(std::string id)
		{
			setID(id);
		}

		Session::Session()
		{
			reset();
		}

		void Session::reset()
		{
			_id = "";
			_idHex = -1;
			_key = -1;
			_number = -1;
		}

		void Session::setID(std::string id)
		{
			_id = id;
			_idHex = Utils::decimal_str2hex(id);
			_number = Utils::encrypt_number(_idHex);
			_key = Utils::encrypt_key(_idHex);
		}

		void Session::setAlive(uint32_t alive)
		{
			_alive = alive - 1; // -1, it will be incremented on next call
		}

		uint32_t Session::alive()
		{
			uint32_t next = _alive + 1;
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
	}
}
