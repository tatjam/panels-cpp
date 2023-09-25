#include <libpanels/BuildSolver.h>
#include <libpanels/InviscidSolver.h>

#include <iostream>
#include <fstream>

int main()
{
	using namespace libpanels;

	auto geom = Geometry<double>::from_naca(100, 1.0, "2412");
	auto text = geom.to_coordinate_data();

	std::ofstream out("geom.dat");
	out << text;
	out.close();

	auto solver_builder = BuildSolver<libpanels::InviscidSolver<double>>()
			.begin_geometry(geom)
				.enable_kutta_condition()
				.finish_geometry()
			.begin_parameters()
				.disregard_smaller_than(0.2)
				.finish_parameters();

	auto solver1 = solver_builder.build();
	auto solver2 = solver_builder
			.begin_parameters()
				.disregard_smaller_than(0.0)
				.finish_parameters()
			.build();

}