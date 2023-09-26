/* DO NOT INCLUDE THIS FILE DIRECTLY */
namespace internal
{
	// Influence of panel j0->j1 in node n
	// We return a value to be added to the j0 vortex strength, and another for the j1 vortex strength
	template<typename S>
	std::pair<S, S> inviscid_influence(Vector2<S> n, Vector2<S> j0, Vector2<S> j1, bool in_panel)
	{
		// Procedure "translated" straight from Xfoil paper
		if(in_panel)
		{
			// Singularity
			return std::make_pair(0, 0);
		}
		else
		{
			// Obtain relative vectors, and their angles
			Vector2<S> r1vec = n - j0;
			Vector2<S> r2vec = n - j1;
			// NOTE: theta is respect to the vertical! That's why arguments are flipped
			S theta1 = std::atan2(r1vec(0), r1vec(1));
			S theta2 = std::atan2(r2vec(0), r2vec(1));

			Vector2<S> j01 = j1 - j0;
			// The perpendicular distance is simple distance of point to line
			S yhat = std::abs((j1(0) - j0(0)) * (j0(1) - n(1)) - (j0(0) - n(0)) * (j1(1) - j0(1))) / (j1 - j0).norm();
			// The parallel distance from j1 and j0 to n is easily obtained, as it's the projection
			// of (ji - n) over (j0->j1)
			// The minus is due to the sign convention in the mathematics developed in the paper
			S xhat1 = -j01.dot(r1vec);
			S xhat2 = -j01.dot(r2vec);
			S xhat12 = xhat1 * xhat1;
			S xhat22 = xhat2 * xhat2;

			S r12 = r1vec.squaredNorm();
			S r22 = r2vec.squaredNorm();
			S r1 = std::sqrt(r12);
			S r2 = std::sqrt(r22);
			S logr1 = std::log(r1);
			S logr2 = std::log(r2);

			S psip = xhat1 * logr1 - xhat2 * logr2 + xhat2 - xhat1 + yhat * (theta1 - theta2);
			S psin = (xhat1 + xhat2) * psip + r22 * logr2 - r12 * logr1 + S(0.5) * yhat * (xhat12 - xhat22);
			psin /= (xhat1 - xhat2);

			return std::make_pair(psip - psin, psip + psin);
		}
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

// TODO: Check that the gpi, gpj stuff is correct
template<typename S>
void InviscidSolver<S>::build_matrix()
{
	size_t size = get_total_panels();
	mat = MatrixX<S>::Zero(size, size);
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
					bool in_panel = gpi == gpj && // must be in same geometry
							(i == j || i == j + 1 || i + 1 == j || // normal panels
							(i==0 && j == gj.geom.points.cols() - 1) || // trailing edge panels
							(i == gj.geom.points.cols() - 1 && j == 0)); // ..
					// Careful! The trailing edge is assumed to close up
					if(j == gj.geom.points.cols() - 1)
					{
						j1 = gj.geom.points.col(0);
						auto infl = internal::inviscid_influence(in, j0, j1, in_panel);
						mat(gpi + i, gpj + j) += infl.first;
						mat(gpi + i, gpj + j + 1) += infl.second;
					}
					else
					{
						j1 = gj.geom.points.col(j + 1);
						auto infl = internal::inviscid_influence(in, j0, j1, in_panel);
						mat(gpi + i, gpj + j) += infl.first;
						mat(gpi + i, gpj + 0) += infl.second;
					}
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

	// Build the rhs_vectors (node positions simply, laid out in the same way as gpi)
	rhs_vectors = Matrix<S, 2, Dynamic>(2, size);
	size_t i = 0;
	for(const auto& gi : geoms)
	{
		for(const auto& vec : gi.geom.points.colwise())
		{
			rhs_vectors(0, i) = vec(0);
			rhs_vectors(1, i) = vec(1);
			i++;
		}
	}
	rhs_vectors(0, size - 1) = 0;
	rhs_vectors(1, size - 1) = 0;

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
