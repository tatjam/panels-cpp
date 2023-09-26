/* DO NOT INCLUDE THIS FILE DIRECTLY */



template<typename S>
void InviscidSolver<S>::solve(Vector2<S> freestream)
{
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
	for(auto vec : rhs_vectors.colwise())
		rhs(i) = vec.dot(pfs); i++;

	dense_solver.solve(rhs);

}
