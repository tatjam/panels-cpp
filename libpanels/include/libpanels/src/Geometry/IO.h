/* DO NOT INCLUDE THIS FILE DIRECTLY */
#include "../../Assert.h"

template<typename S>
template<int Decimals>
std::string Geometry<S>::to_coordinate_data() const
{
	std::string out;
	std::string sane_name = name;
	if(!sane_name.empty() && (sane_name[0] == 'T' || sane_name[0] == 'F'))
		sane_name.insert(sane_name.begin(), '_');

	size_t file_size = 0;
	// Name size + backspace
	if(!sane_name.empty())
		file_size = sane_name.size() + 1;

	// Each point, which we write with given decimals, a maximum of digits,
	// plus the separator space and the backspace
	file_size += points.cols() * (2 * (Decimals + MAX_AIRFOIL_OUTPUT_DIGITS) + 1 + 1);

	out.reserve(file_size);

	if(!sane_name.empty())
		out.append(sane_name.begin(), sane_name.end());

	// The decimals, the number itself (maximum digits + sign) and the dot
	// Note that the string is NOT null terminated
	std::array<char, Decimals + 5> worker_array;
	for(auto vec : points.colwise())
	{
		if(vec(0) >= MAX_AIRFOIL_OUTPUT_COORDINATE)
			throw std::runtime_error("Airfoil x coordinate too big!");
		if(vec(1) >= MAX_AIRFOIL_OUTPUT_COORDINATE)
			throw std::runtime_error("Airfoil y coordinate too big!");

		size_t len = (size_t)(std::to_chars(worker_array.data(), worker_array.data() + worker_array.size(), vec(0),
					  std::chars_format::fixed, Decimals).ptr - &worker_array[0]);
		out.append(worker_array.begin(), worker_array.begin() + len);
		out += ' ';
		len = (size_t)(std::to_chars(worker_array.data(), worker_array.data() + worker_array.size(), vec(1),
								 std::chars_format::fixed, Decimals).ptr - &worker_array[0]);
		out.append(worker_array.begin(), worker_array.begin() + len);
		out += '\n';
	}

	return out;
}
