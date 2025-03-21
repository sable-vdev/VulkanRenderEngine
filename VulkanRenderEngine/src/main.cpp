#include "SableRender.hpp"
#include <cstdlib>

int main()
{
	try
	{
		SableRender window;
		window.Run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}