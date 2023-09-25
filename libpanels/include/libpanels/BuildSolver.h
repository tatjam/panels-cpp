#pragma once
#include "Solver.h"

namespace libpanels
{

	/**
	 * We refer to building a Solver as adding to it the geometry and certain parameters.
	 * Changing geometry, or these certain parameters, is a heavy operation, which may require
	 * plenty of computing time.
	 * Once a Solver is built, any exposed parameters result in a smaller performance hit if changed
	 * At any point you may store the BuildSolver object to generate variations of the same solver in
	 * an efficient manner.
	 * (within certain tolerances, as specified in each solver)
	 * @tparam Solver A valid solver class, specialized for numeric type desired
	 */
	template<typename Solver>
	class BuildSolver
	{
		typedef decltype(Solver::scalar_type_id) S;
	protected:
		Solver solver;

	public:

		/**
		 * Specify a geometry to add to the solver, and begin specifying
		 * its solver specific configuration. Make sure to call finish_geometry() once done.
		 * @param geom
		 */
		Solver::BuildGeometry& begin_geometry(Geometry<S> geom);
		/**
		 * Begin a parameter definition block for the solver, specifying
		 * its specific configuration. Make sure to call finish_parameters() once done.
		 */
		Solver::BuildParameters& begin_parameters();
		/**
		 * Makes a copy of the solver being built and returns it
		 */
		Solver build(){ return solver.build(); }
	};

#include "src/BuildSolver/Implement.h"
}