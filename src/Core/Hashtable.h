#pragma once

#include <Common.h>
#include <Lib/MurmurHash2.h>

#include <cstring>

template<typename TValue, bool Sorted = true>
class hashtable
{
public:
	struct entry
	{
		uint64_t m_hash;
		TValue m_value;
	};

private:
	entry* m_entries = nullptr;
	size_t m_length = 0;
	size_t m_allocSize = 0;

public:
	hashtable()
	{
	}

	~hashtable()
	{
		if (m_entries != nullptr) {
			free(m_entries);
		}
	}

	size_t len() const
	{
		return m_length;
	}

	TValue& add(const char* key, bool sort = true)
	{
		ensure_memory(m_length + 1);

		uint64_t hash = MurmurHash64A(key, strlen(key), 0);
		size_t newIndex = m_length;

		if (Sorted && sort && m_length > 0) {
			if (hash < m_entries[0].m_hash) {
				newIndex = 0;
			} else if (hash > m_entries[m_length - 1].m_hash) {
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
						if (hash > e.m_hash) {
							newIndex++;
						}
						break;
					} else if (hash > e.m_hash) {
						start = halfIndex;
					} else if (hash < e.m_hash) {
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
		ret->m_hash = hash;

		if (Sorted && sort) {
			if (newIndex > 0) {
				assert(hash > m_entries[newIndex - 1].m_hash);
			}
			if (newIndex < m_length - 1) {
				assert(hash < m_entries[newIndex + 1].m_hash);
			}
		}

		return ret->m_value;
	}

	void add(const char* key, const TValue& value, bool sort = true)
	{
		add(key, sort) = value;
	}

	bool get(const char* key, TValue& value) const
	{
		if (m_length == 0) {
			return false;
		}

		uint64_t hash = MurmurHash64A(key, strlen(key), 0);

		if (Sorted) {
			size_t start = 0;
			size_t end = m_length;

			while (true) {
				size_t halfLen = end - start;
				size_t halfIndex = start + halfLen / 2;
				assert(halfIndex < m_length);
				auto& e = m_entries[halfIndex];

				if (hash == e.m_hash) {
					value = e.m_value;
					return true;
				} else if (halfLen == 1) {
					return false;
				} else if (hash > e.m_hash) {
					start = halfIndex;
				} else if (hash < e.m_hash) {
					end = halfIndex;
				}
			}

		} else {
			for (size_t i = 0; i < m_length; i++) {
				auto& e = m_entries[i];
				if (e.m_hash == hash) {
					value = e.m_value;
					return true;
				}
			}
		}

		return false;
	}

	void sort()
	{
		qsort(m_entries, m_length, sizeof(entry), [](const void* pa, const void* pb) {
			auto a = (entry*)pa;
			auto b = (entry*)pb;
			if (a->m_hash < b->m_hash) {
				return -1;
			} else if (a->m_hash > b->m_hash) {
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
};
