#pragma once
#include "Solver.h"

namespace libpanels
{
	/**
	 */
	template<typename S>
	class InviscidSolver : public Solver<S>
	{
		friend class BuildSolver<InviscidSolver>;
	protected:
		#include "src/InviscidSolver/Builders.h"
	private:
		S disregard_smaller_than;

	public:

		InviscidSolver<S> build();

	};

}