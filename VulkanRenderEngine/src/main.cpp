#include "VulkanApp.hpp"
#include <cstdlib>

int main()
{
	VulkanApp window;

	try
	{
		window.Run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}