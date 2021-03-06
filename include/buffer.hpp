#ifndef __BUFFER_HPP__
#define __BUFFER_HPP__

#include <stdint.h>
#include <deque>
#include <vector>
#include <mutex>
#include <condition_variable>

#include "logger.hpp"

namespace buffer {
	template<class T>
	class Buffer {
	public:
		/**
		 * @brief Push only one item to deque
		 *
		 * @param item
		 */
		void push(T& item) {
			std::unique_lock<std::mutex> lock(deque_mutex);
			deque.insert(deque.end(), item);
			condition.notify_one();
		}

		/**
		 * @brief Pop one item from deque
		 *
		 * @return T item
		 */
		T pop() {
			T item;
			{
				std::unique_lock<std::mutex> lock(deque_mutex);

				condition.wait(lock, [this]() {
					return !deque.empty() || terminate;
				});

				item = deque.front();
				deque.pop_front();
			}
			return item;
		}

		std::vector<T> pop_chunk() {
			std::vector<T> chunk;
			{
				std::unique_lock<std::mutex> lock(deque_mutex);
				condition.wait(lock, [this]() {
					return !deque.empty() || terminate;
				});

				chunk.insert(chunk.end(), deque.begin(), deque.end());
				deque.erase(deque.begin(), deque.end());
			}
			return chunk;
		}

		/**
		 * @brief Push more items to deque
		 *
		 * @param chunk items
		 */
		void push_chunk(std::vector<T> chunk) {
			std::unique_lock<std::mutex> lock(deque_mutex);
			deque.insert(deque.end(), chunk.begin(), chunk.end());
			condition.notify_one();
		}

		size_t size() {
			return deque.size();
		}

		T peak() {
			T item;
			{
				std::unique_lock<std::mutex> lock(deque_mutex);

				condition.wait(lock, [this]() {
					return !deque.empty() || terminate;
				});

				item = deque.front();
			}
			return item;
		}
		/* void push_chunk(T* chunk, int len) {
			std::unique_lock<std::mutex> lock(deque_mutex);
			deque.insert(deque.end(), chunk, chunk + len);
		}; */
	private:
		// Mutex for deque
		std::mutex deque_mutex;

		// Used for waiting if deque is empty
		std::condition_variable condition;

		std::deque<T> deque;

		// Set to true if Buffer needs to be terminated
		bool terminate = false;
	};
}

#endif
