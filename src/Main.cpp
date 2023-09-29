#include <libpanels/BuildSolver.h>
#include <libpanels/InviscidSolver.h>

#include <iostream>
#include "Util.h"

int main()
{
	using namespace libpanels;

	auto geom = Geometry<double>::from_data(read_file_to_string("naca0012"));
	auto solver = BuildSolver<libpanels::InviscidSolver<double>>()
			.begin_geometry(geom)
				.set_distance_to_close_trailing_edge(0.5)
				.finish_geometry()
			.begin_parameters()
				.disregard_smaller_than(0.2)
				.finish_parameters()
			.build();

	write_string_to_file("matrix.dat", solver.write_matrix());
	std::string rhs, sln;
	auto solved = solver.solve(Vector2<double>(100.0, 0.0), &rhs, &sln);
	write_string_to_file("rhs.dat", rhs);
	write_string_to_file("sln.dat", sln);
	write_string_to_file("fields.dat", solved.write_flow_field(
			AlignedBox2d(Vector2d(-1.0, -1.0), Vector2d(1.0, 1.0)),
			Vector2d(10.0, 10.0)));



}