/* DO NOT INCLUDE THIS FILE DIRECTLY */
#include "libpanels/src/BuilderBoilerplate.h"

class BuildGeometry
{
LIBPANELS_INTERNAL_BUILD_GEOMETRY(InviscidSolver)
	internal::InviscidGeometry<S> g;
	void on_finish_geometry(const Geometry<S>& geom)
	{
		g.geom = geom;
		solver.geoms.push_back(g);
	}

public:
	/**
	 * Due to floating point imprecision, the only way for the trailing edge of a profile
	 * to be closed is for the two points (upper and lower surface) to be exactly the same.
	 * By default, a reasonably small value is used (EPSILON_CLOSED_TRAILING_EDGE),
	 * but you can set it manually here.
	 *
	 * If the trailing edge is considered closed, the extra trailing edge panel equation is ignored,
	 * and instead an average equation is introduced.
	 * @param val Minimum distance to consider the trailing edge closed.
	 * @return
	 */
	BuildGeometry& set_distance_to_close_trailing_edge(S val)
	{
		g.distance_for_closed_trailing_edge = val;
		return *this;
	}


};

class BuildParameters
{
LIBPANELS_INTERNAL_BUILD_PARAMETERS(InviscidSolver)
public:
	/**
	 * By default, a dense matrix solver is used. Typically, airfoil problems
	 * have many tiny values in the solution matrix. This function allows specifying
	 * which values (in absolute value) are discarded. Using this function automatically
	 * uses the sparse solver if enough elements are discarded (also configurable).
	 * @param val Thresold value
	 * @return
	 */
	BuildParameters& disregard_smaller_than(S val)
	{
		solver.disregard_smaller_than = val;
		return *this;
	}

	/**
	 * See disregard_smaller_than. This specifies the fraction of coefficients (out of the total)
	 * of the solution matrix that must be 0 for the sparse solver to be used.
	 * The default value is FRACTION_FOR_SPARSE_SOLVER
	 */
	BuildParameters& set_fraction_for_sparse_solver(S val)
	{
		solver.fraction_for_sparse = val;
		return *this;
	}
};
