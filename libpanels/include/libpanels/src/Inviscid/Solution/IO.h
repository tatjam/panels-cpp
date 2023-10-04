/* DO NOT INCLUDE THIS FILE DIRECTLY */

template<typename S>
std::string InviscidSolution<S>::write_flow_field(AlignedBox<S, 2> in_box, Vector2<S> dens,
												  std::function<bool(Vector2<S>)> cond) const
{
	return std::string();
}

template<typename S>
template<int Decimals>
std::string InviscidSolution<S>::write_cp() const
{
	std::string fout;
	for(const auto& sgeom : geoms)
	{
		std::string out;
		const auto& geom = sgeom.geom;
		std::string sane_name = geom.get_sane_name();

		size_t file_size = 0;
		// Name size + backspace
		if(!sane_name.empty())
			file_size = sane_name.size() + 1;

		// Each x-position and cp value, which we write with given decimals, a maximum of digits,
		// plus the separator space and the backspace
		file_size += geom.points.cols() * (2 * (Decimals + Geometry<S>::MAX_AIRFOIL_OUTPUT_DIGITS) + 1 + 1);

		out.reserve(file_size);

		if(!sane_name.empty())
			out.append(sane_name.begin(), sane_name.end());


		std::array<char, Decimals + Geometry<S>::MAX_AIRFOIL_OUTPUT_DIGITS> worker_array;
		for(size_t i = 0; i < geom.points.cols(); i++)
		{
			out += '\n';

			/*if(geom.points(0, i) >= Geometry<S>::MAX_AIRFOIL_OUTPUT_COORD)
				throw std::runtime_error("Airfoil x coordinate too big!");
			// TODO: We use the same 5 digits, could be a separate value
			if(cps(i) >= Geometry<S>::MAX_AIRFOIL_OUTPUT_COORD)
				throw std::runtime_error("Airfoil cp value too big!");*/

			size_t len = (size_t)(std::to_chars(worker_array.data(), worker_array.data() + worker_array.size(),
												geom.points(0, i),
												std::chars_format::fixed, Decimals).ptr - &worker_array[0]);
			out.append(worker_array.begin(), worker_array.begin() + len);
			out += ' ';
			len = (size_t)(std::to_chars(worker_array.data(), worker_array.data() + worker_array.size(), cps(i),
										 std::chars_format::fixed, Decimals).ptr - &worker_array[0]);
			out.append(worker_array.begin(), worker_array.begin() + len);
		}

		fout.append(out.begin(), out.end());
	}

	return fout;
}
