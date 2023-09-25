/* DO NOT INCLUDE THIS FILE DIRECTLY */
#include "../BuilderBoilerplate.h"

class BuildGeometry
{
LIBPANELS_INTERNAL_BUILD_GEOMETRY(InviscidSolver)
public:
	BuildGeometry& enable_kutta_condition()
	{
		return *this;
	}
};

class BuildParameters
{
LIBPANELS_INTERNAL_BUILD_PARAMETERS(InviscidSolver)
public:
	BuildParameters& disregard_smaller_than(S val)
	{
		solver.disregard_smaller_than = val;
		return *this;
	}
};
