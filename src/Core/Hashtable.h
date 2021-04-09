#pragma once

#include <Common.h>
#include <Lib/MurmurHash2.h>
#include <Core/Sortdict.h>

#include <cstring>

template<typename TValue>
class hashtable
{
private:
	sortdict<uint64_t, TValue> m_dict;

public:
	TValue& add_unsorted(const char* key)
	{
		return m_dict.add_unsorted(hash(key));
	}

	TValue& add(const char* key)
	{
		return m_dict.add(hash(key));
	}

	void add(const char* key, const TValue& value, bool sort = true)
	{
		if (sort) {
			add(key) = value;
		} else {
			add_unsorted(key) = value;
		}
	}

	void sort()
	{
		m_dict.sort();
	}

	void ensure_memory(size_t count)
	{
		m_dict.ensure_memory(count);
	}

	size_t len() const
	{
		return m_dict.len();
	}

	bool get(const char* key, TValue& value) const
	{
		return m_dict.get(hash(key), value);
	}

protected:
	uint64_t hash(const char* key) const
	{
		return MurmurHash64A(key, strlen(key), 0);
	}
};
