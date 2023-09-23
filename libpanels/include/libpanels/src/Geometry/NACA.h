/* DO NOT INCLUDE THIS FILE DIRECTLY */
#pragma once
#include "../../Assert.h"

namespace internal
{
	// get<0> -> m
	// get<1> -> p
	// get<2> -> t
	template<typename S>
	using NACAElems = std::tuple<S, S, S>;

	// Returns (m, p, t)
	template<typename S>
	static NACAElems<S> get_naca_elems(const std::string& code)
	{
		if(code.length() != 4)
			throw std::runtime_error("NACA element was not a string of four characters");

		int m = code[0] - '0';
		int p = code[1] - '0';
		int tt1 = code[2] - '0';
		int tt2 = code[3] - '0';

		if(m < 0 || m > 9 || p < 0 || p > 9 || tt1 < 0 || tt1 > 9 || tt2 < 0 || tt2 > 9)
			throw std::runtime_error("NACA element was not a string of four numbers");

		return std::make_tuple(S(m) / S(100.0), S(p) / S(10.0), S(tt1 * 10 + tt2) / S(10.0));
	}

	// Takes tau from 0->1 and returns x from 0->1, appropiate for airfoil sampling
	template<typename S>
	static S cosine_sampling(S t)
	{
		return S(0.5) - cos(S(EIGEN_PI) * t);
	}

	// Returns thickness of NACA airfoil given parameters
	template<typename S>
	static S naca_thickness(S x, NACAElems<S> params)
	{
		libpanels_assert(x >= 0 && x <= 1);

		return S(1.0 / 0.2) * std::get<2>(params) * (
				S(0.2969) * Eigen::sqrt<S>(x) -
				S(0.126) * x -
				S(0.3516) * x * x +
				S(0.2843) * x * x * x -
				S(0.1015) * x * x * x * x);
	}

	template<typename S>
	static S naca_camber(S x, NACAElems<S> params)
	{
		libpanels_assert(x >= 0 && x <= 1);
		S m = std::get<0>(params);
		S p = std::get<1>(params);

		if(x <= std::get<1>(params))
		{
			return (m / (p * p)) * (S(2.0) * p * x - x * x);
		}
		else
		{
			S d = S(1.0) - p;
			return (m / (d * d)) * (S(1.0) - S(2.0) * p + S(2.0) * p * x - x * x);
		}
	}

	template<typename S>
	static S naca_dcamber(S x, NACAElems<S> params)
	{
		libpanels_assert(x >= 0 && x <= 1);
		S m = std::get<0>(params);
		S p = std::get<1>(params);

		if(x <= std::get<1>(params))
		{
			return (S(2.0) * m / (p * p)) * (p - x);
		}
		else
		{
			S d = S(1.0) - p;
			return (S(2.0) * m / (d * d)) * (p-x);
		}
	}
}

template<typename S>
Geometry<S> Geometry<S>::from_naca(int num_panels, S chord, const std::string& code)
{
	Geometry<S> out;
	auto naca_elems = internal::get_naca_elems<S>(code);

	for(int n = 0; n < num_panels - 1; n++)
	{
		S x = internal::cosine_sampling(n / S(num_panels - 1));

	}

	return out;
}