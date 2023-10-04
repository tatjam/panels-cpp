/* DO NOT INCLUDE THIS FILE DIRECTLY */
namespace internal
{
	// Influence of panel j0->j1 in node i
	// We return a value to be added to the j0 vortex strength, and another for the j1 vortex strength
	template<typename S, bool trailing = false>
	std::pair<S, S> inviscid_influence(Vector2<S> ivec, Vector2<S> j0vec, Vector2<S> j1vec, size_t i, size_t j0, size_t j1)
	{
		Vector2<S> j01vec = j1vec - j0vec;
		S j01inorm = 1.0 / j01vec.norm();

		// Obtain relative vectors
		Vector2<S> r1vec = (ivec - j0vec);
		Vector2<S> r2vec = (ivec - j1vec);

		// Projection of (j1 -> n) over the perpendicular of (j0->j1)
		Vector2<S> perpj01(j01vec(1), -j01vec(0));
		S yhat = perpj01.dot(r1vec) * j01inorm;
		// Projection of (ji -> n) over (j0->j1)
		S xhat1 = j01vec.dot(r1vec) * j01inorm;
		S xhat2 = j01vec.dot(r2vec) * j01inorm;
		S xhat12 = xhat1 * xhat1;
		S xhat22 = xhat2 * xhat2;

		// This was obtained from XFoil's source code, as the paper's not fully clear
		// on exactly how theta is defined
		S theta1 = std::atan2(xhat1, yhat);
		S theta2 = std::atan2(xhat2, yhat);

		S r12 = r1vec.squaredNorm();
		S r22 = r2vec.squaredNorm();
		S r1 = std::sqrt(r12);
		S r2 = std::sqrt(r22);
		S logr1 = std::log(r1);
		S logr2 = std::log(r2);

		// Correct singularities
		if(i == j0)
		{
			logr1 = 0.0;
			theta1 = 0.0;
		}

		if(i == j1)
		{
			logr2 = 0.0;
			theta2 = 0.0;
		}

		S psip = S(0.5) * xhat1 * logr1 - S(0.5) * xhat2 * logr2 + xhat2 - xhat1 + yhat * (theta1 - theta2);
		// This equation is different than on the paper
		S psin = (xhat1 + xhat2) * psip + S(0.5) * (r22 * logr2 - r12 * logr1 + xhat1 * xhat1 - xhat2 * xhat2);
		psin /= (xhat1 - xhat2);
		psip *= EIGEN_PI * S(0.25);
		psin *= EIGEN_PI * S(0.25);

		if (trailing)
		{
			return std::make_pair(psip, 0.0);
		}
		else
		{
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
	return tot;
}

// TODO: Check that the gpi, gpj stuff is correct
template<typename S>
void InviscidSolver<S>::build_matrix()
{
	size_t size = get_total_panels();
	// The extra row is the kutta condition, the extra column the stremfunction
	mat = MatrixX<S>::Zero(size + 1, size + 1);
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
				// ... from all panels (not including trailing edge!)
				for (size_t j = 0; j < gj.geom.points.cols() - 1; j++)
				{
					Vector2<S> in = gi.geom.points.col(i);
					Vector2<S> j0 = gj.geom.points.col(j);
					Vector2<S> j1;
					j1 = gj.geom.points.col(j + 1);
					auto infl = internal::inviscid_influence(in, j0, j1, i, j, j + 1);
					mat(gpi + i, gpj + j) += infl.first;
					mat(gpi + i, gpj + j + 1) += infl.second;
				}
				gpj += gj.geom.points.cols();
			}
			gpj = 0;
		}
		gpi += gi.geom.points.cols();
	}


	gpi = 0; gpj = 0;
	// Trailing edge panel for each geometry
	for(const auto& gi : geoms)
	{
		for(size_t i = 0; i < gi.geom.points.cols(); i++)
		{
			for(const auto& gj : geoms)
			{
				Vector2<S> in = gi.geom.points.col(i);
				Vector2<S> j0 = gj.geom.points.col(gj.geom.points.cols() - 1);
				Vector2<S> j1 = gj.geom.points.col(0);
				auto infl = internal::inviscid_influence<S, true>(in, j0, j1, i, gj.geom.points.cols() - 1, 0).first;

				Vector2<S> tvec = (j1 - j0).normalized();
				Vector2<S> svec;
				if(gj.trailing_edge_vector.has_value())
				{
					svec = gj.trailing_edge_vector.value();
				}
				else
				{
					// Calculate it with the trailing panels bisector
					//Vector2<S> svec =

				}

				S cross = std::abs(svec(0) * tvec(1) - svec(1) * tvec(0));
				infl *= cross;

				mat(gpi + i, gpj + 0) += infl;
				mat(gpi + i, gpj + gj.geom.points.cols() - 1) += infl;

				gpj += gj.geom.points.cols();
			}
			gpj = 0;
		}
		gpi += gi.geom.points.cols();
	}

	// Introduce the streamfunction
	for(size_t i = 0; i < size; i++)
	{
		mat(i, size) = -1;
	}

	// Finally, introduce the Kutta condition for each geometry
	gpi = 0;
	for(const auto& gi : geoms)
	{
		mat(gpi + gi.geom.points.cols(), 0) = 1;
		mat(gpi + gi.geom.points.cols(), gi.geom.points.cols() - 1) = 1;
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
