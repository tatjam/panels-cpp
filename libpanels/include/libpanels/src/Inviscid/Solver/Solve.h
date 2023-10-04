/* DO NOT INCLUDE THIS FILE DIRECTLY */


template<typename S>
InviscidSolution<S> InviscidSolver<S>::solve(Vector2<S> freestream, std::string* out_sln)
{
	if(state == SOLVE_DIRECT)
	{
		throw std::runtime_error("Tried to use solve() on a InviscidSolver already solved with solve_direct()");
	}

	InviscidSolution<S> out;

	// Copy the geometries to the solution
	out.geoms = geoms;

	size_t size = get_total_panels();
	if(state == NOT_COMPUTED)
	{
		if(use_sparse)
		{
			// TODO
		}
		else
		{
			// This "consumes" the matrix, avoiding an uneeded copy in memory
			dense_solver.compute(mat);
			mat = {};

			// Solve it for alpha = 0 and alpha = 90 (velocity 1 for simplicity)
			VectorX<S> rhs0(size + 1);
			VectorX<S> rhs90(size + 1);
			size_t i = 0;
			for(const auto& vec : rhs_vectors.colwise())
			{
				rhs0(i) = vec(1);
				rhs90(i) = vec(0);
				i++;
			}

			// Kutta condition
			rhs0(size) = 0;
			rhs90(size) = 0;

			alpha0 = dense_solver.solve(rhs0);
			alpha90 = dense_solver.solve(rhs90);

			// Clear the dense solver, reducing memory usage
			dense_solver = {};
		}
		state = LINEAR_SOLVE;
	}

	// Linear combination of the two, depending on alpha
	S alpha = std::atan2(-freestream(1), freestream(0));

	ArrayX<S> sln = alpha0 * std::cos(alpha) + alpha90 * std::sin(alpha);

	// Because they were calculated with velocity 1, the qinf term disappears
	out.cps = 1.0 - pow(sln, 2);
	out.freestream = freestream;

	// TODO: This is not particularly efficient
	if(out_sln)
	{
		std::stringstream s;
		s << sln;
		(*out_sln) = s.str();
	}

	return out;
}

template<typename S>
InviscidSolution<S> InviscidSolver<S>::solve_direct(Vector2<S> freestream, std::string* out_rhs, std::string* out_sln)
{
	if(state == LINEAR_SOLVE)
	{
		throw std::runtime_error("Tried to use solve_direct() on a InviscidSolver already solved with solve()");
	}

	InviscidSolution<S> out;

	// Copy the geometries to the solution
	out.geoms = geoms;

	size_t size = get_total_panels();
	if(state == NOT_COMPUTED)
	{
		if(use_sparse)
		{
			// TODO
		}
		else
		{
			// This "consumes" the matrix, avoiding an uneeded copy in memory
			dense_solver.compute(mat);
			mat = {};
		}
		state = SOLVE_DIRECT;
	}

	// Generate the right hand side (orthogonal to freestream)
	Vector2<S> pfs(freestream(1), -freestream(0));
	VectorX<S> rhs(size + 1);
	size_t i = 0;
	for(const auto& vec : rhs_vectors.colwise())
	{
		rhs(i) = vec.dot(pfs);
		i++;
	}
	// For the Kutta condition
	rhs(size) = 0;


	// TODO: This is not particularly efficient
	if(out_rhs)
	{
		std::stringstream s;
		s << rhs;
		(*out_rhs) = s.str();
	}

	ArrayX<S> vortex_strengths = dense_solver.solve(rhs).array();

	S qinf2 = freestream.squaredNorm();
	// Surface vorticity is equal to the surface velocity
	out.cps = 1.0 - pow(vortex_strengths, 2) / qinf2;

	out.freestream = freestream;

	// TODO: This is not particularly efficient
	if(out_sln)
	{
		std::stringstream s;
		s << vortex_strengths;
		(*out_sln) = s.str();
	}

	return out;
}
