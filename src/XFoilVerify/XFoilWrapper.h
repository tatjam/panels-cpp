#pragma once
#include <string>

/**
 * This class generates outputs to be piped into an original XFoil installation.
 * It will generate files named xfoil_temp_[number] in the working directory, which
 * will be removed after operation.
 */
class XFoilWrapper
{
private:
	const std::string& xfoil_name;


public:

	explicit XFoilWrapper(const std::string& xfoil_name = "./xfoil");
};
