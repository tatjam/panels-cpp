#pragma once
#include "Geometry.h"

namespace libpanels
{

	/**
	 * Base class for all other solvers. Do not use directly.
	 * Concept of building and solving:
	 * 	- Building prepares the solver for solution by adding geometry and parameters, which
	 * 	  may not change on subsequent operations.
	 * 	- Solving generates a solution, and likely modifies the solver internal state in such
	 * 	  a way that subsequent solutions, using the same solver, give results faster, or
	 * 	  some other behaviour. Across multiple solves, you may vary certain parameters,
	 * 	  within certain limits, but the data fixed on build is not able to change anymore.
	 * See individual solvers for details.
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