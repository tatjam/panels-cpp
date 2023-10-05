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

		enum State
		{
			NOT_COMPUTED,
			SOLVE_DIRECT,
			LINEAR_SOLVE
		};
		State state;

		bool use_sparse = false;
		FullPivLU<Matrix<S, Dynamic, Dynamic>> dense_solver;
		Matrix<S, 2, Dynamic> rhs_vectors;


		// Note that this matrix gets cleared once solve or solve_direct is first called!
		MatrixX<S> mat;
		// Same as before if using sparse solver
		SparseMatrix<S> smat;

		// If using solve(), these store the solutions for alpha = 0 and alpha=90
		VectorX<S> alpha0, alpha90;

		Index get_total_panels();
		// Also builds the right hand vectors (panel normals)
		void build_matrix();
		// Sets use_sparse and smat (if sparse solver is to be used)
		void maybe_make_sparse();

	public:

		InviscidSolver<S> build() const;
		/**
		 * Due to the linearity of the method used, once the system is solved for Alpha = 0 and Alpha = 90
		 * further solutions are near instant. The first solve call will actually solve twice for these two angles of attack,
		 * and then for your requested freestream velocity. The solver afterwards will work with the precomputed
		 * solutions to give extremely fast solutions.
		 *
		 * If you are solving a single angle of attack, or only two, you may want to use solve_direct.
		 *
		 * Note that after solve or solve_direct is called, the other will result in a runtime error!
		 *
		 * @param freestream
		 * @param out_rhs Optional pointer to a string, into which the right hand side is written (as new line separated numbers)
		 * @param out_sln Optional pointer to a string, into which the solution is written (same as before)
		 * @return
		 */
		InviscidSolution<S> solve(Vector2<S> freestream, std::string* out_sln = nullptr);

		/**
		 * Doesn't exploit the linearity of the method. Instead, the matrix is computed on the first solve. This means
		 * that the first solution will take every so slightly less than solve, but further solutions will be slightly slower.
		 *
		 * TODO: Benchmark, maybe evaluating the two linear terms is actually slower than this!!
		 * @param freestream
		 * @param out_rhs
		 * @param out_sln
		 * @return
		 */
		InviscidSolution<S> solve_direct(Vector2<S> freestream, std::string* out_rhs = nullptr, std::string* out_sln = nullptr);

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