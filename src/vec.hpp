#ifndef VECTOR_HPP_
#define VECTOR_HPP_

#ifndef GLM_CONVERSION
#define GLM_CONVERSION 1
#endif
#ifndef BULLET_CONVERSION
#define BULLET_CONVERSION 1
#endif

#include <iostream>
#include <ctgmath>
#include <cstdint>
#include <mutex>
#if GLM_CONVERSION
#include <glm/glm.hpp>
#endif
#if BULLET_CONVERSION
#include <LinearMath/btVector3.h>
#endif

typedef intptr_t intP;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;

typedef unsigned int uint;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

typedef uint8 uchar;

template <typename _T = float>
struct vec3;

typedef vec3<int> ivec3;
typedef vec3<uint> uvec3;
typedef vec3<float> fvec3;
typedef vec3<double> dvec3;

typedef ivec3 const ivec3_c;
typedef uvec3 const uvec3_c;
typedef fvec3 const fvec3_c;
typedef dvec3 const dvec3_c;

template <typename _T>
struct vec3
{
	using T = _T;
	using ref = vec3<T> &;
	using cref = vec3<T> const &;
	using ptr = vec3<T> *;
	using cptr = vec3<T> const *;

	static vec3<T> const X;
	static vec3<T> const Y;
	static vec3<T> const Z;
	static vec3<T> const XY;
	static vec3<T> const YZ;
	static vec3<T> const XZ;
	static vec3<T> const XYZ;
	static vec3<T> const ZERO;

	T x, y, z;

	constexpr vec3() = default;

	template <typename T1, typename T2, typename T3>
	constexpr vec3(T1 const &x, T2 const &y, T3 const &z) : x(x), y(y), z(z) {}

	template <typename T2>
	constexpr vec3(vec3<T2> const &v) : x(v.x), y(v.y), z(v.z) {}

	constexpr vec3(vec3 const &v) = default;

#if BULLET_CONVERSION
	vec3(btVector3 const &v) : x(v.x()), y(v.y()), z(v.z()) {}
	btVector3 bt() const {return btVector3(x, y, z);}
	operator btVector3() const {return bt();}
#endif

#if GLM_CONVERSION
	vec3(glm::fvec3 const &v) : x(v.x), y(v.y), z(v.z) {};
	vec3(glm::dvec3 const &v) : x(v.x), y(v.y), z(v.z) {};
	vec3(glm::uvec3 const &v) : x(v.x), y(v.y), z(v.z) {};
	vec3(glm::ivec3 const &v) : x(v.x), y(v.y), z(v.z) {};
	vec3(glm::bvec3 const &v) : x(v.x), y(v.y), z(v.z) {};
	glm::vec3 glm() const {return glm::vec3(x, y, z);}
	operator glm::vec3() const {return glm();}
#endif

	T &operator [](int i)
	{return i == 0 ?x :(i == 1 ?y :z);}

	constexpr T const &operator [](int i) const
	{return i == 0 ?x :(i == 1 ?y :z);}

	constexpr T length2() const {return x*x + y*y + z*z;}
	constexpr T length() const {return sqrt(length2());}

	// compatibility with atomic_vec3
	vec3<T> &get()
	{
		return *this;
	}

	template <typename T2>
	void set(vec3<T2> const &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	template <typename T2>
	constexpr bool operator ==(vec3<T2> const &v) const
	{return x == v.x && y == v.y && z == v.z;}

	template <typename T2>
	constexpr bool operator !=(vec3<T2> const &v) const
	{return x != v.x || y != v.y || z != v.z;}

	vec3<T> &normalize()
	{
		T l = length();
		x/= l;
		y/= l;
		z/= l;
		return *this;
	}

	constexpr vec3<T> normalized() const
	{
		return vec3<T>(x, y, z).normalize();
	}

	constexpr vec3<T> operator -() const
	{
		return vec3<T>(-x, -y, -z);
	}

	template <typename T2>
	vec3<T> &add(vec3<T2> const &v)
	{
		x+= v.x;
		y+= v.y;
		z+= v.z;
		return *this;
	}

	template <typename T2>
	vec3<T> &add(T2 const &ax, T2 const &ay, T2 const &az)
	{
		x+= ax;
		y+= ay;
		z+= az;
		return *this;
	}

	template <typename T2>
	vec3<T> &operator +=(vec3<T2> const &v)
	{
		return add(v);
	}

	template <typename T2>
	constexpr vec3<decltype(T() + T2())> operator +(vec3<T2> const &v) const
	{
		return vec3<decltype(T() + T2())>(x + v.x, y + v.y, z + v.z);
	}

	template <typename T2>
	vec3<T> &add(T2 const &s)
	{
		x+= s;
		y+= s;
		z+= s;
		return *this;
	}

	template <typename T2>
	vec3<T> &operator +=(T2 const &s)
	{
		return add(s);
	}

	template <typename T2>
	constexpr vec3<decltype(T() + T2())> operator +(T2 const &s) const
	{
		return vec3<decltype(T() + T2())>(x + s, y + s, z + s);
	}

	template <typename T2>
	vec3<T> &sub(vec3<T2> const &v)
	{
		x-= v.x;
		y-= v.y;
		z-= v.z;
		return *this;
	}

	template <typename T2>
	vec3<T> &sub(T2 const &ax, T2 const &ay, T2 const &az)
	{
		x-= ax;
		y-= ay;
		z-= az;
		return *this;
	}

	template <typename T2>
	vec3<T> &operator -=(vec3<T2> const &v)
	{
		return sub(v);
	}

	template <typename T2>
	constexpr vec3<decltype(T() - T2())> operator -(vec3<T2> const &v) const
	{
		return vec3<decltype(T() - T2())>(x - v.x, y - v.y, z - v.z);
	}

	template <typename T2>
	vec3<T> &sub(T2 const &s)
	{
		x-= s;
		y-= s;
		z-= s;
		return *this;
	}

	template <typename T2>
	vec3<T> &operator -=(T2 const &s)
	{
		return sub(s);
	}

	template <typename T2>
	constexpr vec3<decltype(T() - T2())> operator -(T2 const &s) const
	{
		return vec3<decltype(T() - T2())>(x - s, y - s, z - s);
	}


	template <typename T2>
	vec3<T> &mul(vec3<T2> const &v)
	{
		x*= v.x;
		y*= v.y;
		z*= v.z;
		return *this;
	}

	template <typename T2>
	vec3<T> &mul(T2 const &ax, T2 const &ay, T2 const &az)
	{
		x*= ax;
		y*= ay;
		z*= az;
		return *this;
	}

	template <typename T2>
	vec3<T> &operator *=(vec3<T2> const &v)
	{
		return mul(v);
	}

	template <typename T2>
	constexpr vec3<decltype(T() * T2())> operator *(vec3<T2> const &v) const
	{
		return vec3<decltype(T() * T2())>(x * v.x, y * v.y, z * v.z);
	}

	template <typename T2>
	vec3<T> &mul(T2 const &s)
	{
		x*= s;
		y*= s;
		z*= s;
		return *this;
	}

	template <typename T2>
	vec3<T> &operator *=(T2 const &s)
	{
		return mul(s);
	}

	template <typename T2>
	constexpr vec3<decltype(T() * T2())> operator *(T2 const &s) const
	{
		return vec3<decltype(T() * T2())>(x * s, y * s, z * s);
	}


	template <typename T2>
	vec3<T> &div(vec3<T2> const &v)
	{
		x/= v.x;
		y/= v.y;
		z/= v.z;
		return *this;
	}

	template <typename T2>
	vec3<T> &div(T2 const &ax, T2 const &ay, T2 const &az)
	{
		x/= ax;
		y/= ay;
		z/= az;
		return *this;
	}

	template <typename T2>
	vec3<T> &operator /=(vec3<T2> const &v)
	{
		return div(v);
	}

	template <typename T2>
	constexpr vec3<decltype(T() / T2())> operator /(vec3<T2> const &v) const
	{
		return vec3<decltype(T() / T2())>(x / v.x, y / v.y, z / v.z);
	}


	template <typename T2>
	vec3<T> &div(T2 const &s)
	{
		x/= s;
		y/= s;
		z/= s;
		return *this;
	}

	template <typename T2>
		vec3<T> &operator /=(T2 const &s)
	{
		return div(s);
	}

	template <typename T2>
	constexpr vec3<decltype(T() / T2())> operator /(T2 const &s) const
	{
		return vec3<decltype(T() / T2())>(x / s, y / s, z / s);
	}

	template <typename T2>
	vec3<T> &mod(vec3<T2> const &v)
	{
		x = fmod(x, v.x);
		y = fmod(y, v.y);
		z = fmod(z, v.z);
		return *this;
	}

	template <typename T2>
	vec3<T> &mod(T2 const &ax, T2 const &ay, T2 const &az)
	{
		x = fmod(x, ax);
		y = fmod(y, ay);
		z = fmod(z, az);
		return *this;
	}

	template <typename T2>
	vec3<T> &operator %=(vec3<T2> const &v)
	{
		return mod(v);
	}

	template <typename T2>
	constexpr vec3<decltype(fmod(T(), T2()))> operator %(vec3<T2> const &v) const
	{
		return vec3<decltype(fmod(T(), T2()))>(fmod(x, v.x), fmod(y, v.y), fmod(z, v.z));
	}

	template <typename T2>
	vec3<T> &mod(T2 const &s)
	{
		x = fmod(x, s);
		y = fmod(y, s);
		z = fmod(z, s);
		return *this;
	}

	template <typename T2>
	vec3<T> &operator %=(T2 const &s)
	{
		return mod(s);
	}

	template <typename T2>
	constexpr vec3<decltype(fmod(T(), T2()))> operator %(T2 const &s) const
	{
		return vec3<decltype(fmod(T(), T2()))>(fmod(x, s), fmod(y, s), fmod(z, s));
	}
};










template <typename T>
vec3<T> const vec3<T>::X = vec3<T>(1, 0, 0);
template <typename T>
vec3<T> const vec3<T>::Y = vec3<T>(0, 1, 0);
template <typename T>
vec3<T> const vec3<T>::Z = vec3<T>(0, 0, 1);
template <typename T>
vec3<T> const vec3<T>::XY = vec3<T>(1, 1, 0);
template <typename T>
vec3<T> const vec3<T>::YZ = vec3<T>(0, 1, 1);
template <typename T>
vec3<T> const vec3<T>::XZ = vec3<T>(1, 0, 1);
template <typename T>
vec3<T> const vec3<T>::ZERO = vec3<T>(0, 0, 0);
template <typename T>
vec3<T> const vec3<T>::XYZ = vec3<T>(1, 1, 1);

template <typename _T>
struct atomic_vec3
{
private:
	std::mutex lock;
	vec3<_T> vec;
public:
	using T = _T;

	atomic_vec3() = default;

	template <typename T2>
	atomic_vec3(vec3<T2> const &v)
	{
		vec = v;
	}

	vec3<T> get()
	{
		vec3<T> v;
		lock.lock();
		v = vec;
		lock.unlock();
		return v;
	}

	template <typename T2>
	void set(vec3<T2> const &v)
	{
		lock.lock();
		vec = v;
		lock.unlock();
	}

	template <typename T2>
	void operator =(vec3<T2> const &v)
	{
		set(v);
	}
};

template <typename T>
std::ostream &operator << (std::ostream &os, vec3<T> const &v)
{
	return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

#endif /* VECTOR_HPP_ */
