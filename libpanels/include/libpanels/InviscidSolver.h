#pragma once
#include "Solver.h"
#include <optional>
#include <sstream>

// TODO: Debug remove
#include <iostream>

namespace libpanels
{
	namespace internal
	{
		template<typename S>
		class InviscidGeometry
		{
		public:
			Geometry<S> geom;
			// Set to infinity to always close the trailing edge
			S distance_for_closed_trailing_edge;
		};
	}

	/** Implements the same inviscid method as Xfoil on inviscid mode. See the paper for details.
	 */
	template<typename S>
	class InviscidSolver : public Solver<S>
	{
		friend class BuildSolver<InviscidSolver>;
	public:
		static constexpr S EPSILON_CLOSED_TRAILING_EDGE = 0.0001;
		static constexpr S FRACTION_FOR_SPARSE_SOLVER = 0.3;
	protected:
		#include "src/InviscidSolver/Builders.h"
	private:
		S disregard_smaller_than;
		S fraction_for_sparse;
		std::vector<internal::InviscidGeometry<S>> geoms;
		bool computed = false;
		bool use_sparse = false;
		PartialPivLU<Matrix<S, Dynamic, Dynamic>> dense_solver;
		Matrix<S, 2, Dynamic> rhs_vectors;

		// Note that this matrix gets cleared once computed = true
		// (It's transferred to the solver)
		MatrixX<S> mat;
		// Same as before if using sparse solver
		SparseMatrix<S> smat;

		size_t get_total_panels();
		// Also builds the right hand vectors (panel normals)
		void build_matrix();
		// Sets use_sparse and smat (if sparse solver is to be used)
		void maybe_make_sparse();

	public:

		InviscidSolver<S> build() const;
		/**
		 * After the first solve, subsequent solutions are considerably faster as the system matrix
		 * has been worked into a more usable form.
		 * @param freestream
		 * @return
		 */
		void solve(Vector2<S> freestream);

		/**
		 * Writes the matrix to a simple format (readable by Mathematica, among other software)
		 * WARNING: Only callable before the first solve! (Results in an exception otherwise)
		 * @return An string containing the matrix laid out, columns separated by spaces, rows by new lines.
		 */
		[[nodiscard]] std::string write_matrix() const;



	};



#include "src/InviscidSolver/Build.h"
#include "src/InviscidSolver/Solve.h"
#include "src/InviscidSolver/IO.h"
}