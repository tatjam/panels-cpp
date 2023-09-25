#pragma once
#include "Geometry.h"

namespace libpanels
{

	/**
	 * Base class for all other solvers. Do not use directly.
	 */
	template<typename S> requires std::is_floating_point_v<S>
	class Solver
	{
	private:

	public:
		// Used as a template trick, possibly a better way to do this
		static S scalar_type_id;
	};
}