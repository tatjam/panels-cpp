/* DO NOT INCLUDE THIS FILE DIRECTLY */

template<typename Solver>
typename Solver::BuildGeometry BuildSolver<Solver>::begin_geometry(Geometry<BuildSolver<Solver>::S> geom)
{
	return typename Solver::BuildGeometry(geom, *this, solver);
}

template<typename Solver>
typename Solver::BuildParameters BuildSolver<Solver>::begin_parameters()
{
	return typename Solver::BuildParameters(*this, solver);
}
