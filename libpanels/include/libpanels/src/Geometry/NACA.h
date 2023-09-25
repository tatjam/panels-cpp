/* DO NOT INCLUDE THIS FILE DIRECTLY */
#include "../../Assert.h"

namespace internal
{
	template<typename S>
	struct NACAElems { S m,p,t; };

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

		return NACAElems{S(m) / S(100.0), S(p) / S(10.0), S(tt1 * 10 + tt2) / S(10.0)};
	}

	// Takes tau from 0->1 and returns x from 0->1, appropiate for airfoil sampling
	template<typename S>
	static S cosine_sampling(S t)
	{
		return S(0.5) * (S(1.0 ) - std::cos(S(EIGEN_PI) * t));
	}

	// Returns thickness of NACA airfoil given parameters
	template<typename S>
	static S naca_thickness(S x, NACAElems<S> params)
	{
		libpanels_assert(x >= 0 && x <= 1);

		return S(1.0 / 0.2) * params.t * (
				S(0.2969) * std::sqrt(x) -
				S(0.126) * x -
				S(0.3516) * x * x +
				S(0.2843) * x * x * x -
				S(0.1015) * x * x * x * x);
	}

	template<typename S>
	static S naca_camber(S x, NACAElems<S> params)
	{
		libpanels_assert(x >= 0 && x <= 1);
		S m = params.m;
		S p = params.p;

		if(x <= p)
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
		S m = params.m;
		S p = params.p;

		if(x <= p)
		{
			return (S(2.0) * m / (p * p)) * (p - x);
		}
		else
		{
			S d = S(1.0) - p;
			return (S(2.0) * m / (d * d)) * (p-x);
		}
	}

	template<typename S>
	static Vector2<S> naca_sample(S x, NACAElems<S> params, bool upper)
	{
		S t = naca_thickness(x, params);
		S c = naca_camber(x, params);
		S dc = naca_dcamber(x, params);

		// Note that 	sin(atan(x)) = x / sqrt(1+x^2)
		// 				cos(atan(x)) = 1 / sqrt(1+x^2)
		S denom = std::sqrt(S(1.0) + dc * dc);
		S sint = dc / denom;
		S cost = S(1.0) / denom;

		if(upper)
			return Vector2<S>(x - t * sint, c + t * cost);
		else
			return Vector2<S>(x + t * sint, c - t * cost);
	}
}

template<typename S>
Geometry<S> Geometry<S>::from_naca(int num_panels, S chord, const std::string& code)
{
	Geometry<S> out;
	auto naca_elems = internal::get_naca_elems<S>(code);
	int top_elem = (int)ceil(S(num_panels) / S(2.0));
	int bottom_elem = (int)floor(S(num_panels) / S(2.0));

	// Allocate the points
	out.points = PointList(2, num_panels);

	// We start at the top trailing edge, and travel to the
	// bottom trailing edge, through the leading edge
	for(int n = 0; n < top_elem; n++)
	{
		S x = 1.0 - internal::cosine_sampling(S(n) / S(top_elem));
		Vector2<S> sample = internal::naca_sample(x, naca_elems, true);
		out.points.col(n) = sample;
	}
	for(int n = 0; n < bottom_elem; n++)
	{
		S x = internal::cosine_sampling(S(n) / S(bottom_elem));
		Vector2<S> sample = internal::naca_sample(x, naca_elems, false);
		out.points.col(n + top_elem) = sample;
	}

	return out;
}