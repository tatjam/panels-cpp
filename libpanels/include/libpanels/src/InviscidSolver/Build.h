/* DO NOT INCLUDE THIS FILE DIRECTLY */
namespace internal
{
	// Influence of panel j0->j1 in node n
	template<typename S>
	S inviscid_influence(Vector2<S> n, Vector2<S> j0, Vector2<S> j1)
	{
		return 1.0;
	}
}

template<typename S>
size_t InviscidSolver<S>::get_total_panels()
{
	size_t tot = 0;
	for(const auto& g : geoms)
	{
		// Each point gives a panel
		tot += g.geom.points.cols();
	}
	// Kutta condition / averaging equation
	tot += 1;
	return tot;
}

template<typename S>
void InviscidSolver<S>::build_matrix()
{
	size_t size = get_total_panels();
	mat = MatrixX<S>(size, size);
	rhs_vectors = Matrix<S, 2, Dynamic>(2, size);

	size_t gpi = 0, gpj = 0;

	// The coefficient matrix stores:
	// -> each node equation is represented by a row
	// -> each vortex strength is represented by a column
	// gpi, gpj are used to separate geometries in the matrix
	for(const auto& gi : geoms)
	{
		// We calculate influence at all nodes...
		for (size_t i = 0; i < gi.geom.points.cols(); i++)
		{
			for (const auto &gj: geoms)
			{
				// ... from all panels (including trailing edge, albeit it may be removed later!)
				for (size_t j = 0; j < gj.geom.points.cols(); j++)
				{
					Vector2<S> in = gi.geom.points.col(i);
					Vector2<S> j0 = gj.geom.points.col(j);
					Vector2<S> j1;
					// Careful! The trailing edge is assumed to close up
					if(j == gj.geom.points.cols() - 1)
						j1 = gj.geom.points.col(0);
					else
						j1 = gj.geom.points.col(j + 1);
					// We are writing the (gpj + j)th coefficient for the (gpi + i)th equation
					mat(gpi + i, gpj + j) = internal::inviscid_influence(in, j0, j1);
				}
				gpj += gj.geom.points.cols();
			}
			gpj = 0;
		}
		gpi += gi.geom.points.cols();
	}

	// If the trailing edge is closed, replace its equation with the averaged one

	// Finally, introduce the Kutta condition for each geometry
	gpi = 0;
	for(const auto& gi : geoms)
	{
		mat(gpi + gi.geom.points.cols(), 0) = 1;
		mat(gpi + gi.geom.points.cols(), gi.geom.points.cols()) = 1;
		gpi += gi.geom.points.cols();
	}


}


template<typename S>
void InviscidSolver<S>::maybe_make_sparse()
{
	// TODO
	use_sparse = false;
}

template<typename S>
InviscidSolver<S> InviscidSolver<S>::build() const
{
	// Make a copy of the parameters and geometry
	InviscidSolver<S> out = *this;

	// We now work to generate the matrix in out
	out.build_matrix();
	out.maybe_make_sparse();

	return out;
}
