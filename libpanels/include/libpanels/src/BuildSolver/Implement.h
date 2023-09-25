/* DO NOT INCLUDE THIS FILE DIRECTLY */

template<typename Solver>
Solver::BuildGeometry& BuildSolver<Solver>::begin_geometry(Geometry<BuildSolver<Solver>::S> geom)
{
	auto pt = new Solver::BuildGeometry(geom, *this, solver);
	return *pt;
}

template<typename Solver>
Solver::BuildParameters& BuildSolver<Solver>::begin_parameters()
{
	auto pt = new Solver::BuildParameters(*this, solver);
	return *pt;
}
