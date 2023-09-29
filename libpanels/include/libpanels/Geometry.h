#pragma once
#include "../../Eigen/Eigen"

#include "Utils.h"
#include <string>
#include <array>
#include <charconv>
#include <cctype>

namespace libpanels
{
	using namespace Eigen;

	/**
	 * Geometry of a single, "closed" element in the simulation, typically an airfoil.
	 * Includes tools for generating geometry, similar to XFoil's GDES toolbench.
	 */
	template<typename S> requires std::is_floating_point_v<S>
	class Geometry
	{
	public:
		// TODO: Maybe using Dynamic, 2 is better for performance (or RowMajor, less code change...)
		using PointList = Array<S, 2, Dynamic, ColMajor>;
		PointList points;

		// Only limited for output purposes.
		static constexpr int MAX_AIRFOIL_OUTPUT_DIGITS = 5;
		static constexpr S MAX_AIRFOIL_OUTPUT_COORD = ipow(10, MAX_AIRFOIL_OUTPUT_DIGITS);


		/**
		 * Name of the airfoil, for display and output purposes.
		 */
		std::string name;

		/**
		 * Generates geometry from a NACA 4 digit code, using cosine sampling.
		 * Decimals, and extra properties are NOT supported.
		 * Note that the trailing edge panel is NOT included in the count!
		 * @param num_panels Number of panels to use, excluding trailing edge panel.
		 * @param chord Length of the airfoil
		 * @param code The 4 digit code, in string form
		 * @return
		 */
		static Geometry<S> from_naca(int num_panels, S chord, const std::string &code);

		/**
		 * Generates geometry from one of the supported geometry formats. See \ref geom_formats "geometry formats."
		 * @param data Data to import, format automatically detected.
		 * @return
		 */
		static Geometry<S> from_data(const std::string& data);
		/**
		 * Generates labelled coordinates from the geometry. If name is empty, plain coordinates are output,
		 * otherwise, labelled plain coordinates are output.
		 * @return
		 */
		template<int Decimals=4>
		[[nodiscard]] std::string to_coordinate_data() const;

		/**
		 * Generates DXF data from the geometry.
		 * @return
		 */
		std::string to_dxf_data() const;

		/**
		 * Returns true if the point is inside the geometry. We assume the trailing
		 * edge is closed with a straight line.
		 * @param point Point to check
		 * @return Is point inside the geometry?
		 */
		bool is_inside(Vector2<S> point);

	};


#include "src/Geometry/NACA.h"
#include "src/Geometry/IO.h"

}