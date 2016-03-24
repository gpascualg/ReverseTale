#pragma once

#include <atomic>
#include <map>
#include <vector>
#include <chrono>

#include <Tools/random_selector.h>

#include <threadpool.h>
#include <threadpool11/threadpool11.hpp>
#include <threadpool11/utility.hpp>
#include <threadpool11/worker.hpp>

#include <boost/lockfree/queue.hpp>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>


using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;


class Database
{
	template <typename T>
	using Work = std::function<T(mongocxx::database)>;
	using InternalWork = std::function<void(void)>;

public:
	Database(mongocxx::uri uri, std::string& database);

	static Database* initialize(mongocxx::uri uri, std::string&& database)
	{
		if (_mongoInstance == nullptr)
		{
			_mongoInstance = new mongocxx::instance;
		}
		
		auto db = new Database(std::move(uri), database);
		auto vec = _instances.find(database);
		if (vec == _instances.end())
		{
			_instances.emplace(database, std::vector<Database*>{});
		}

		_instances[database].push_back(db);
		return db;
	}

	static void deinitialize()
	{
		for (auto pair : _instances)
		{
			for (auto db : pair.second)
			{
				delete db;
			}
		}

		delete _mongoInstance;
	}

	template <typename T>
	std::future<T> query(Work<T> callable)
	{
		std::promise<T> promise;
		auto future = promise.get_future();

		/* TODO: how to avoid copy of callable into this lambda and the ones below? In a decent way... */
		/* evil move hack */
		auto move_callable = threadpool11::make_move_on_copy(std::move(callable));
		/* evil move hack */
		auto move_promise = threadpool11::make_move_on_copy(std::move(promise));
		
		auto workFunc = new InternalWork([move_callable, move_promise, this]() mutable {
			move_promise.value().set_value((move_callable.value())(_db));
		});

		bool result = _queue.push(workFunc);
		assert(result && "Database queue is full, consider using more threads or increasing queue size");

		std::unique_lock<std::mutex> work_signal_lock(_work_signal_mutex);
		_work_signal.notify_one();

		return future;
	}

	static Database* get(std::string&& database)
	{
		assert(_instances.find(database) != _instances.end() && "Database not found");
		return _selector(_instances[database]);
	}

	inline void stop() { _stop = true; }

private:
	void run()
	{
		while (!_stop)
		{
			while (!_queue.empty())
			{
				InternalWork* work;
				if (_queue.pop(work))
				{
					(*work)();
					delete work;
				}
			}

			std::unique_lock<std::mutex> work_signal_lock(_work_signal_mutex);
			_work_signal.wait(work_signal_lock);
		}
	}

private:
	static mongocxx::instance* _mongoInstance;
	static std::map<std::string, std::vector<Database*> > _instances;
	static Utils::random_selector<> _selector;

	mongocxx::client _client;
	mongocxx::database _db;
	boost::lockfree::queue<InternalWork*> _queue;
	std::atomic<bool> _stop;

	mutable std::mutex _work_signal_mutex;
	std::condition_variable _work_signal;
};

#define gDB(x) Database::get(x)
