#pragma once
#include "../../Eigen/Eigen"
#include <string>
#include <array>
#include <charconv>

namespace libpanels
{
	using namespace Eigen;

	/**
	 *
	 */
	template<typename S>
	class Geometry
	{
	private:
		// TODO: Maybe using Dynamic, 2 is better for performance (or RowMajor, easier...)
		using PointList = Array<S, 2, Dynamic, ColMajor>;
		PointList points;

	public:
		// Only limited for output purposes
		static constexpr int MAX_AIRFOIL_OUTPUT_DIGITS = 4;
		static constexpr S MAX_AIRFOIL_OUTPUT_COORDINATE = std::pow(10, MAX_AIRFOIL_OUTPUT_DIGITS);

		std::string name;

		/**
		 * Generates geometry from a NACA 4 digit code, using cosine sampling.
		 * Decimals, and extra properties are NOT supported.
		 * Note that the trailing edge panel is included in the count!
		 * @param num_panels Total number of panels to use.
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
		std::string to_coordinate_data() const;

		/**
		 * Generates DXF data from the geometry.
		 * @return
		 */
		std::string to_dxf_data() const;
	};


#include "src/Geometry/NACA.h"
#include "src/Geometry/IO.h"

}