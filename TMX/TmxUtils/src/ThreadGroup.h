/*
 * ThreadGroup.h
 *
 *  Created on: May 5, 2017
 *      Author: gmb
 */

#ifndef SRC_THREADGROUP_H_
#define SRC_THREADGROUP_H_


namespace tmx {
namespace utils {

template <class ThreadClass, typename GroupT = uint8_t, typename IdentifierT = uint8_t>
class ThreadGroup {
public:
	typedef GroupT group_type;
	typedef IdentifierT id_type;
	static constexpr size_t max_groups = ::pow(2, 8 * sizeof(group_type));
	static constexpr size_t max_ids = ::pow(2, 8 * sizeof(id_type));

	ThreadGroup() {
		// Initialize the queue assignments
		for (size_t i = 0; i < max_groups; i++) {
			for (size_t j = 0; j < max_ids; j++) {
				assignments[i][j].count = 0;
				assignments[i][j].threadId = -1;
			}
		}
	}

	size_t size() {
		return _threads.size();
	}

	void set_size(size_t size) {
		for (size_t i = 0; _threads.size() < size; i++) {
			_threads.emplace_back();
			_threads[i].start();
		}
	}

	void assign(group_type group, id_type id, const typename ThreadClass::incoming_item &item) {
		static std::atomic<uint32_t> next {0};

		if (_threads.size() == 0)
			return;

		// Need this here to ensure no pre-mature free up of the thread ID
		assignments[group][id].count++;

		int tId = assignments[group][id].threadId;
		if (tId < 0) {
			assignments[group][id].threadId = next;

			if (++next >= _threads.size())
				next = 0;
		}

		_threads[(assignments[group][id].threadId)].push(item);
	}

	void unassign(group_type group, id_type id) {
		if (!(--assignments[group][id].count))
			assignments[group][id].threadId = -1;
	}

	void stop() {
		size_t total = _threads.size();
		for (size_t i = 0; i < total; i++) {
			_threads[0].stop();
			_threads.pop_front();
		}
	}

	ThreadClass &operator[](size_t n) {
		return _threads[n];
	}

	ThreadClass &operator[](size_t n) const {
		return _threads[n];
	}
private:
	std::deque<ThreadClass> _threads;

	struct source_info {
		std::atomic<uint64_t> count;
		std::atomic<int> threadId;
	};

	source_info assignments[max_groups][max_ids];
};


}} // namespace tmx::utils

#endif /* SRC_THREADGROUP_H_ */
