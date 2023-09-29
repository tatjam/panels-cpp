#pragma once

template<typename S>
std::string InviscidSolver<S>::write_matrix() const
{
	if(computed)
	{
		throw std::runtime_error("Tried to write matrix of a computed inviscid solver. Write the matrix before first solve!");
	}

	std::stringstream s;
	s << mat << std::endl;

	return s.str();
}
