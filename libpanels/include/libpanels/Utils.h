#pragma once
#include <cstdint>

namespace libpanels
{
	// https://stackoverflow.com/questions/17719674/c11-fast-constexpr-integer-powers
	// Sadly, pow is not constexpr until C++26
	constexpr int64_t ipow(int64_t base, int exp, int64_t result = 1)
	{
		return exp < 1 ? result :  ipow(base * base, exp / 2, (exp % 2) ? result * base : result);
	}
}