#include <string>

const char PATH_SEP =
#ifdef _WIN32
	'\\';
#else
	'/';
#endif

namespace fs
{
	class path
	{
	public:
		class placeholder
		{
		public:
			placeholder(std::string pl):
				_pl(pl)
			{}

			inline std::string string() { return _pl; }

		private:
			std::string _pl;
		};

		path(std::string ph):
			_path(ph)
		{}

		path(char* ph):
			_path(std::string(ph))
		{}

		placeholder remove_filename()
		{
			std::string ph = _path;
			int pos = ph.find_last_of(PATH_SEP);
			return placeholder(ph.substr(0, pos));
		}

	private:
	    	std::string _path;
	};
}
