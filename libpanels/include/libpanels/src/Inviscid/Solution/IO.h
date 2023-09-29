/* DO NOT INCLUDE THIS FILE DIRECTLY */

template<typename S>
std::string InviscidSolution<S>::write_flow_field(AlignedBox<S, 2> in_box, Vector2<S> dens,
												  std::function<bool(Vector2<S>)> cond) const
{
	return std::string();
}
