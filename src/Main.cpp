#include <libpanels/BuildSolver.h>
#include <libpanels/InviscidSolver.h>

#include <iostream>
#include "Util.h"

#define DIR "naca0012 test case/"

int main()
{
	using namespace libpanels;

	auto geom = Geometry<double>::from_data(read_file_to_string(DIR "geom"));
	auto solver = BuildSolver<libpanels::InviscidSolver<double>>()
			.begin_geometry(geom)
				.finish_geometry()
			.build();

	write_string_to_file(DIR "matrix.dat", solver.write_matrix());

	for(int alpha = -20; alpha < 30; alpha += 10)
	{
		double dalpha = (double)(alpha) * EIGEN_PI / 180.0;
		auto solved = solver.solve(Vector2<double>(cos(dalpha), sin(dalpha)));

		std::string base = DIR "libpanels";
		base += std::to_string(alpha);
		write_string_to_file(base, solved.write_cp());
	}




}