#include "database.h"

mongocxx::instance* Database::_mongoInstance = nullptr;
std::map<std::string, std::vector<Database*> > Database::_instances{};
Utils::random_selector<> Database::_selector{};

Database::Database(mongocxx::uri uri, std::string& database):
	_client(uri),
	_db(_client[database]),
	_queue(256), // TODO: Configurable queue size,
	_stop(false)
{
	gPool->postWork<void>([this]() { run(); });
}
