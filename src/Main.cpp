#include <libpanels/Geometry.h>
#include <iostream>
int main()
{
	auto geom = libpanels::Geometry<double>::from_naca(100, 1.0, "4244");
	auto text = geom.to_coordinate_data();
	std::cout << text << std::endl;

}