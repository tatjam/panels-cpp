#pragma once
#include "Solver.h"
#include "InviscidSolution.h"
#include <optional>
#include <sstream>

// TODO: Debug remove
#include <iostream>

namespace libpanels
{

	/** Implements the same inviscid method as Xfoil on inviscid mode. See the paper for details.
	 */
	template<typename S>
	class InviscidSolver : public Solver<S>
	{
		friend class BuildSolver<InviscidSolver>;
		friend class InviscidSolution<S>;
	public:
		static constexpr S EPSILON_CLOSED_TRAILING_EDGE = 0.0001;
		static constexpr S FRACTION_FOR_SPARSE_SOLVER = 0.3;

	protected:
		#include "libpanels/src/Inviscid/Solver/Builders.h"

	private:
		S disregard_smaller_than;
		S fraction_for_sparse;
		std::vector<internal::InviscidGeometry<S>> geoms;
		bool computed = false;
		bool use_sparse = false;
		FullPivLU<Matrix<S, Dynamic, Dynamic>> dense_solver;
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
		 * @param out_rhs Optional pointer to a string, into which the right hand side is written (as new line separated numbers)
		 * @param out_sln Optional pointer to a string, into which the solution is written (same as before)
		 * @return
		 */
		InviscidSolution<S> solve(Vector2<S> freestream, std::string* out_rhs = nullptr, std::string* out_sln = nullptr);

		/**
		 * Writes the matrix to a simple format (readable by Mathematica, among other software)
		 * WARNING: Only callable before the first solve! (Results in an exception otherwise)
		 * @return An string containing the matrix laid out, columns separated by spaces, rows by new lines.
		 */
		[[nodiscard]] std::string write_matrix() const;
	};



#include "libpanels/src/Inviscid/Solver/Build.h"
#include "libpanels/src/Inviscid/Solver/Solve.h"
#include "libpanels/src/Inviscid/Solver/IO.h"
}