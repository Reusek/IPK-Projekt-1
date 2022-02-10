#ifndef __POOL_HPP__
#define __POOL_HPP__

#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <functional>
#include <tuple>
#include <condition_variable>

#include "logger.hpp"

using namespace logger;

namespace pool {
	/**
	 * @brief Thread pool
	 *
	 * @tparam T Task function parameters
	 */
	template<class... T>
	class Pool {
	public:
		// Task function type
		using t_task_fn = std::function<void(T...)>;

		// Task type
		using t_task = std::tuple<t_task_fn, std::tuple<T...>>;

		/**
		 * @brief Construct a new Pool object
		 * Start default number of workers.
		 */
		Pool() {
			uint32_t n = std::thread::hardware_concurrency();
			start_workers(n);
		}

		/**
		 * @brief Construct a new Pool object
		 * Start defined number of workers.
		 *
		 * @param workers Number of workers
		 */
		Pool(uint32_t workers) {
			start_workers(workers);
		}

		/**
		 * @brief Add new task to pool which will be executed when his time comes.
		 *
		 * @param new_task_fn Task function
		 */
		void add_task(t_task new_task) {
			{
				std::unique_lock<std::mutex> lock(queue_mutex);
				queue.push(new_task);
			}

			condition.notify_one();
		}

		/**
		 * @brief Terminate pool
		 */
		void shutdown() {
			{
				std::unique_lock<std::mutex> lock(queue_mutex);
				terminate = true;
			}

			condition.notify_all();

			for (std::thread &th : threads)
			{
				th.join();
			}

			threads.clear();
		}

	private:
		// Mutex for queue
		std::mutex queue_mutex;

		// Queue of tasks
		std::queue<t_task> queue;

		// Used for waiting if queue is empty
		std::condition_variable condition;

		// Threads witch run workers
		std::vector<std::thread> threads;

		// Set to true if pool needs to be terminated
		bool terminate = false;

		/**
		 * @brief Worker function
		 * Runs forever or if variable `terminate` is set to `true`.
		 * Executes `t_task_fn` function.
		 */
		void worker() {
			for (;;) {
				t_task task;

				{
					std::unique_lock<std::mutex> lock(queue_mutex);

					condition.wait(lock, [this]() {
						return !queue.empty() || terminate;
					});

					task = queue.front();
					queue.pop();
				}

				auto task_function = std::get<0>(task);
				auto task_arguments = std::get<1>(task);

				std::apply([&task_function](auto &&... args) {
					task_function(args...);
				}, task_arguments);
			}
		}

		/**
		 * @brief Start workers
		 *
		 * @param workers Number of workers
		 */
		void start_workers(uint32_t workers) {
			Log(INFO) << "Starting pool with " << workers << " workers.";
			for (uint32_t i = 0; i != workers; i++) {
				threads.push_back(std::thread(&pool::Pool<int>::worker, this));
			}
		}
	};
}

#endif
