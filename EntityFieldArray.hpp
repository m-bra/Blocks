/*
 * EntityFieldArray.hpp
 *
 *  Created on: Sep 30, 2014
 *      Author: merlin
 */

#ifndef ENTITYFIELDARRAY_HPP_
#define ENTITYFIELDARRAY_HPP_

#include <vector>
#include <cassert>

template <typename Field>
class EntityFieldArray
{
private:
	std::vector<Field> array;
public:

	void resize(int newSize)
	{
		array.resize(newSize);
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
	bool iterate(F const &func)
	{
		for (int e = 0; e < array.size(); ++e)
			if (!func(e, operator [](e)))
				return false;
		return true;
	}

	template <typename F>
	bool iterate(F const &func) const
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

	void resize(int newSize)
	{
		array.resize(newSize);
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
	bool iterate(F const &func)
	{
		for (int e = 0; e < array.size(); ++e)
			if (!func(e, operator [](e)))
				return false;
		return true;
	}

	template <typename F>
	bool iterate(F const &func) const
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

#endif /* ENTITYFIELDARRAY_HPP_ */
