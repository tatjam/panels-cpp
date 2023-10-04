#pragma once
#include <optional>

namespace libpanels
{
	namespace internal
	{
		template<typename S>
		class InviscidGeometry
		{
		public:
			Geometry<S> geom;
			std::optional<Vector2<S>> trailing_edge_vector;
			// Set to infinity to always close the trailing edge
			S distance_for_closed_trailing_edge;

		};
	}

	template<typename S>
	class InviscidSolver;

	template<typename S>
	class InviscidSolution
	{
		friend class InviscidSolver<S>;
	private:
		std::vector<internal::InviscidGeometry<S>> geoms;

		/**
		 * Pressure coefficient at each node, in the same order as they appeared in the geometries
		 * used to generate the solution.
		 */
		ArrayX<S> cps;
		Vector2<S> freestream;
	public:

		/**
		 * Outputs a file in a format very similar to Xfoil's CPWR
		 * (Although it allows multiple geometries, which are written inline,
		 * and doesn't have aligned numbers)
		 * @return
		 */
		template<int Decimals=8>
		[[nodiscard]] std::string write_cp() const;

		/**
		 * Obtain velocity at a point. This includes the freestream velocity!
		 * @param at
		 * @return
		 */
		[[nodiscard]] Vector2<S> get_velocity(const Vector2<S>& at) const;

		[[nodiscard]] Array<S, 2, Dynamic> get_velocity_field(const AlignedBox<S, 2>& in_box, const Vector2<S>& dens) const
		{
			return get_velocity_field(in_box, dens, [](Vector2<S> d){ return true; });
		}

		/**
		 * Obtain velocity field at a rectangular region, including a given conditional function,
		 * using a given sampling density.
		 * @param in_box Specify the bounds in which to evaluate the flow field
		 * @param dens Vector density in each direction (vectors per unit)
		 * @return An array of vector pairs, in the form {pos0, vel0, pos1, vel1, ...}
		 */
		[[nodiscard]] Array<S, 2, Dynamic> get_velocity_field(
				const AlignedBox<S, 2>& in_box, const Vector2<S>& dens, std::function<bool(Vector2<S>)> cond) const;

		[[nodiscard]] std::string write_flow_field(const AlignedBox<S, 2>& in_box, const Vector2<S>& dens) const
		{
			return write_flow_field(in_box, dens, [](Vector2<S> d){ return true; });
		}

		/**
		 * Same as get_velocity_field, but outputs as a string easily readable in Mathematica.
		 * @return A string in the form "{{x0, y0}, {u0, v0}, {x1, y1}, {u1, v1}, ...}"
		 */
		[[nodiscard]] std::string write_flow_field(
				AlignedBox<S, 2> in_box, Vector2<S> dens, std::function<bool(Vector2<S>)> cond) const;

	};


#include "src/Inviscid/Solution/IO.h"
#include "src/Inviscid/Solution/VelField.h"

}