/* DO NOT INCLUDE THIS FILE DIRECTLY */

template<typename S>
Vector2<S> InviscidSolution<S>::get_velocity(const Vector2<S>& at) const
{
	Vector2<S> out;
	// Derivation of the streamfunction given in the Xfoil paper

	return out;
}

template<typename S>
Array<S, 2, Dynamic> InviscidSolution<S>::get_velocity_field(
		const AlignedBox<S, 2>& in_box, const Vector2<S>& dens, std::function<bool(Vector2<S>)> cond) const
{

}
