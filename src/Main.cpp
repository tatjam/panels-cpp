#include <libpanels/BuildSolver.h>
#include <libpanels/InviscidSolver.h>

#include <iostream>
#include <fstream>

int main()
{
	using namespace libpanels;

	auto geom = Geometry<double>::from_naca(10, 1.0, "2412");
	auto text = geom.to_coordinate_data();

	{
		std::ofstream out("geom.dat");
		out << text;
	}

	auto solver = BuildSolver<libpanels::InviscidSolver<double>>()
			.begin_geometry(geom)
				.set_distance_to_close_trailing_edge(0.5)
				.finish_geometry()
			.begin_parameters()
				.disregard_smaller_than(0.2)
				.finish_parameters()
			.build();


	{
		std::ofstream out("matrix.dat");
		out << solver.write_matrix();
	}

	std::string rhs, sln;
	solver.solve(Vector2<double>(100.0, 0.0), &rhs, &sln);

	{
		std::ofstream out("rhs.dat");
		out << rhs;
	}

	{
		std::ofstream out("sln.dat");
		out << sln;
	}


}