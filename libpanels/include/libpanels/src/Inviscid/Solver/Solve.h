/* DO NOT INCLUDE THIS FILE DIRECTLY */



template<typename S>
InviscidSolution<S> InviscidSolver<S>::solve(Vector2<S> freestream, std::string* out_rhs, std::string* out_sln)
{
	InviscidSolution<S> out;

	// Copy the geometries to the solution
	out.geoms = geoms;

	size_t size = get_total_panels();
	if(!computed)
	{
		if(use_sparse)
		{
			// TODO
		}
		else
		{
			// This "consumes" the matrix, avoiding an uneeded copy in memory
			dense_solver.compute(mat);
			mat = MatrixX<S>();
		}
		computed = true;
	}

	// Generate the right hand side (orthogonal to freestream)
	Vector2<S> pfs(freestream(1), -freestream(0));
	VectorX<S> rhs(size);
	size_t i = 0;
	for(const auto& vec : rhs_vectors.colwise())
	{
		rhs(i) = vec.dot(pfs);
		i++;
	}


	// TODO: This is not particularly efficient
	if(out_rhs)
	{
		std::stringstream s;
		s << rhs;
		(*out_rhs) = s.str();
	}

	out.vortex_strengths = dense_solver.solve(rhs);
	out.freestream = freestream;

	// TODO: This is not particularly efficient
	if(out_sln)
	{
		std::stringstream s;
		s << out.vortex_strengths;
		(*out_sln) = s.str();
	}

	return out;
}
