#include "pool.hpp"

// template<class... T>
/* pool::Pool<T...>::Pool() {
	uint32_t n = std::thread::hardware_concurrency();
	start_workers(n);
}

template<class... T>
pool::Pool<T...>::Pool(uint32_t n) {
	start_workers(n);
}

template<class... T>
void pool::Pool<T...>::worker() {
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

template<class... T>
void pool::Pool<T...>::shutdown() {
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

template<class... T>
void pool::Pool<T...>::start_workers(uint32_t workers) {
	std::cout << "Starting pool with " << workers << " workers." << std::endl;
	for (uint32_t i = 0; i != workers; i++) {
		threads.push_back(std::thread(&pool::Pool<int>::worker, this));
	}
}


template<class... T>
void pool::Pool<T...>::add_task(t_task new_task) {
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		queue.push(new_task);
	}

	condition.notify_one();
}


template pool::Pool<int>;
 */
