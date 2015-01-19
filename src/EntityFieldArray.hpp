#ifndef ENTITYFIELDARRAY_HPP_INCLUDED
#define ENTITYFIELDARRAY_HPP_INCLUDED

#ifndef PRECOMPILED_HPP_INCLUDED
#warning #include "precompiled.hpp" at the beginning of the TU!
#include "precompiled.hpp"
#endif

#include <vector>

namespace blocks
{

template <typename Field>
class EntityFieldArray
{
private:
	std::vector<Field> array;
public:

	void resize(int newSize, Field f = Field())
	{
		array.resize(newSize, f);
	}

	bool isValidEntity(int e) const
	{
		return e >= 0 && e < array.size();
	}

	Field &operator [](int e)
	{
		assert(isValidEntity(e));
		return array[e];
	}

	Field const &operator [](int e) const
	{
		assert(isValidEntity(e));
		return array[e];
	}

	template <typename F>
	void iterate(F const &func)
	{
		for (int e = 0; e < array.size(); ++e)
			func(e, operator [](e));
	}

	template <typename F>
	void iterate(F const &func) const
	{
		for (int e = 0; e < array.size(); ++e)
			func(e, operator [](e));
	}

	template <typename F>
	bool iterate_cond(F const &func)
	{
		for (int e = 0; e < array.size(); ++e)
			if (!func(e, operator [](e)))
				return false;
		return true;
	}

	template <typename F>
	bool iterate_cond(F const &func) const
	{
		for (int e = 0; e < array.size(); ++e)
			if (!func(e, operator [](e)))
				return false;
		return true;
	}

	void fill(Field f = Field())
	{
		for (int i = 0; i < array.size(); ++i)
			array[i] = f;
	}

	int getCount()
	{
		return array.size();
	}
};

template <>
class EntityFieldArray<bool>
{
private:
	std::vector<int> array;
public:

	void resize(int newSize, bool b = bool())
	{
		array.resize(newSize, b);
	}

	bool isValidEntity(int e) const
	{
		return e >= 0 && e < (int)array.size();
	}

	int &operator [](int e)
	{
		assert(isValidEntity(e));
		return array[e];
	}

	int const &operator[](int e) const
	{
		assert(isValidEntity(e));
		return array[e];
	}

	template <typename F>
	void iterate(F const &func)
	{
		for (int e = 0; e < array.size(); ++e)
			func(e, operator [](e));
	}

	template <typename F>
	void iterate(F const &func) const
	{
		for (int e = 0; e < array.size(); ++e)
			func(e, operator [](e));
	}

	template <typename F>
	bool iterate_cond(F const &func)
	{
		for (int e = 0; e < array.size(); ++e)
			if (!func(e, operator [](e)))
				return false;
		return true;
	}

	template <typename F>
	bool iterate_cond(F const &func) const
	{
		for (int e = 0; e < array.size(); ++e)
			if (!func(e, operator [](e)))
				return false;
		return true;
	}

	void fill(bool f = false)
	{
		for (int i = 0; i < (int)array.size(); ++i)
			array[i] = f;
	}

	int getCount()
	{
		return array.size();
	}
};

}

#endif /* ENTITYFIELDARRAY_HPP_ */
