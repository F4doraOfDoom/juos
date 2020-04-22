#ifndef _LIBCXX_COMPARE_H
#define _LIBCXX_COMPARE_H

template <class T>
struct Less
{
	bool operator()(const T& x, const T& y) const
	{
		return x < y;
	}

	typedef bool result_type;
};

#endif // _LIBCXX_COMPARE_H