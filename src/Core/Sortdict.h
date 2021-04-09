#pragma once

#include <Common.h>

#include <cstring>

template<typename TKey, typename TValue>
class sortdict
{
public:
	struct entry
	{
		uint64_t m_key;
		TValue m_value;
	};

private:
	entry* m_entries = nullptr;
	size_t m_length = 0;
	size_t m_allocSize = 0;

public:
	sortdict()
	{
	}

	sortdict(const sortdict& other)
	{
		m_length = other.m_length;
		if (m_length > 0) {
			ensure_memory(m_length);
		}
		for (size_t i = 0; i < m_length; i++) {
			m_entries[i] = other.m_entries[i];
		}
	}

	~sortdict()
	{
		if (m_entries != nullptr) {
			for (size_t i = 0; i < m_length; i++) {
				m_entries[i].~entry();
			}
			free(m_entries);
		}
	}

	size_t len() const
	{
		return m_length;
	}

	TValue& add_unsorted(const TKey& key)
	{
		ensure_memory(m_length + 1);
		entry* ret = new (m_entries + m_length) entry;
		m_length++;
		ret->m_key = key;
		return ret->m_value;
	}

	TValue& add(const TKey& key)
	{
		ensure_memory(m_length + 1);

		size_t newIndex = m_length;

		if (m_length > 0) {
			if (key < m_entries[0].m_key) {
				newIndex = 0;
			} else if (key > m_entries[m_length - 1].m_key) {
				newIndex = m_length;
			} else {
				size_t start = 0;
				size_t end = m_length;

				while (true) {
					size_t halfLen = end - start;
					size_t halfIndex = start + halfLen / 2;
					assert(halfIndex < m_length);
					auto& e = m_entries[halfIndex];

					if (halfLen == 1) {
						newIndex = halfIndex;
						if (key > e.m_key) {
							newIndex++;
						}
						break;
					} else if (key > e.m_key) {
						start = halfIndex;
					} else if (key < e.m_key) {
						end = halfIndex;
					}
				}
			}

			if (newIndex < m_length) {
				memmove(m_entries + newIndex + 1, m_entries + newIndex, (m_length - newIndex) * sizeof(entry));
			}
		}

		entry* ret = new (m_entries + newIndex) entry;
		m_length++;
		ret->m_key = key;

		if (newIndex > 0) {
			assert(key > m_entries[newIndex - 1].m_key);
		}
		if (newIndex < m_length - 1) {
			assert(key < m_entries[newIndex + 1].m_key);
		}

		return ret->m_value;
	}

	void add(const TKey& key, const TValue& value, bool sort = true)
	{
		if (sort) {
			add(key) = value;
		} else {
			add_unsorted(key) = value;
		}
	}

	void remove(const TKey& key)
	{
		int index = indexof(key);
		if (index == -1) {
			return;
		}

		m_entries[index].~entry();

		if (index < m_length) {
			memmove(m_entries + index, m_entries + index + 1, (m_length - index) * sizeof(entry));
		}
		m_length--;
	}

	bool contains(const TKey& key) const
	{
		return indexof(key) != -1;
	}

	bool get(const TKey& key, TValue& value) const
	{
		int index = indexof(key);
		if (index == -1) {
			return false;
		}

		value = m_entries[index].m_value;
		return true;
	}

	void sort()
	{
		qsort(m_entries, m_length, sizeof(entry), [](const void* pa, const void* pb) {
			auto a = (entry*)pa;
			auto b = (entry*)pb;
			if (a->m_key < b->m_key) {
				return -1;
			} else if (a->m_key > b->m_key) {
				return 1;
			}
			return 0;
		});
	}

	void ensure_memory(size_t count)
	{
		if (m_allocSize >= count) {
			return;
		}

		size_t resize = m_allocSize + m_allocSize / 2;
		if (resize < SIZE_MAX && resize > count) {
			count = resize;
		}

		m_entries = (entry*)realloc(m_entries, count * sizeof(entry));
		m_allocSize = count;
	}

private:
	int indexof(const TKey& key) const
	{
		if (m_length == 0) {
			return -1;
		}

		size_t start = 0;
		size_t end = m_length;

		while (true) {
			size_t halfLen = end - start;
			size_t halfIndex = start + halfLen / 2;
			assert(halfIndex < m_length);
			auto& e = m_entries[halfIndex];

			if (key == e.m_key) {
				return halfIndex;
			} else if (halfLen == 1) {
				return -1;
			} else if (key > e.m_key) {
				start = halfIndex;
			} else if (key < e.m_key) {
				end = halfIndex;
			}
		}

		return -1;
	}
};
