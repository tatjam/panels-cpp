/* DO NOT INCLUDE THIS FILE DIRECTLY */
#include "../../Assert.h"

template<typename S> requires std::is_floating_point_v<S>
std::string Geometry<S>::get_sane_name() const
{
	std::string sane_name = name;
	if(!sane_name.empty() && (sane_name[0] == 'T' || sane_name[0] == 'F'))
		sane_name.insert(sane_name.begin(), '_');
	return sane_name;
}


template<typename S> requires std::is_floating_point_v<S>
template<int Decimals>
std::string Geometry<S>::to_coordinate_data() const
{
	std::string out;
	std::string sane_name = get_sane_name();

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
	std::array<char, Decimals + MAX_AIRFOIL_OUTPUT_DIGITS> worker_array;
	for(auto vec : points.colwise())
	{
		out += '\n';

		if(vec(0) >= MAX_AIRFOIL_OUTPUT_COORD)
			throw std::runtime_error("Airfoil x coordinate too big!");
		if(vec(1) >= MAX_AIRFOIL_OUTPUT_COORD)
			throw std::runtime_error("Airfoil y coordinate too big!");

		size_t len = (size_t)(std::to_chars(worker_array.data(), worker_array.data() + worker_array.size(), vec(0),
					  std::chars_format::fixed, Decimals).ptr - &worker_array[0]);
		out.append(worker_array.begin(), worker_array.begin() + len);
		out += ' ';
		len = (size_t)(std::to_chars(worker_array.data(), worker_array.data() + worker_array.size(), vec(1),
								 std::chars_format::fixed, Decimals).ptr - &worker_array[0]);
		out.append(worker_array.begin(), worker_array.begin() + len);
	}

	return out;
}

template<typename S>
requires std::is_floating_point_v<S>
Geometry<S> Geometry<S>::from_data(const std::string &data)
{
	Geometry<S> out;
	std::stringstream ss(data);

	if(data.size() == 0) throw std::runtime_error("Data was empty");

	{
		std::string name_line;
		std::getline(ss >> std::ws, name_line);
		std::stringstream name_line_ss(name_line);
		S dummy;
		if(name_line_ss >> dummy)
		{
			ss.seekg(0);
		}
		else
		{
			out.name = name_line;
		}
	}

	std::string line;
	// Extract each coordinate, unless we find 999.9 999.9, skipping any 4 number line
	bool skipped_ises = false;

	std::vector<std::pair<S, S>> points;
	while(!std::getline(ss, line).eof())
	{
		if(!skipped_ises)
		{
			skipped_ises = true;
			std::istringstream skip_ises(line);
			S d1, d2, d3, d4;
			if(skip_ises >> d1 >> d2 >> d3 >> d4)
			{
				// This line must be ignored
				continue;
			}
		}

		std::istringstream iss(line);
		S x, y;
		if(!(iss >> x >> y))
		{
			throw std::runtime_error("Badly formatted geometry file");
		}

		points.emplace_back(x, y);
	}

	out.points = PointList(2, points.size());
	for(size_t i = 0; i < points.size(); i++)
	{
		out.points(0, i) = points[i].first;
		out.points(1, i) = points[i].second;
	}

	return out;
}
