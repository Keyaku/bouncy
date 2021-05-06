#ifndef BOUNCY_CPP_API_H
#define BOUNCY_CPP_API_H

#include <string>

std::string get_filename(std::string path)
{
	std::string retval = path.substr(path.find_last_of("/\\") + 1);
	std::string::size_type const p(retval.find_last_of('.'));
	retval = retval.substr(0, p);

	return retval;
}

std::string get_extension(std::string path)
{
	return path.substr(path.find_last_of('.') +1, path.length()-1);
}

#endif
