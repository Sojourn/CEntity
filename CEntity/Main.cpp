#include <iostream>
#include <array>
#include "EntityManager.h"

uint32_t prototypeID = 0;
static PrototypeDef prototypes[] =
{
	{
		++prototypeID,
		"",
		"",
		[](const EntityID entity){ std::cout << entity << std::endl; }
	},
	{
		++prototypeID,
		"",
		"",
		[](const EntityID entity){ std::cout << entity << std::endl; }
	},
	{
		++prototypeID,
		"",
		"",
		[](const EntityID entity){ std::cout << entity << std::endl; }
	},
	{
		INVALID_PROTOTYPE,
		"",
		"",
		[](const EntityID entity){ std::cout << entity << std::endl; }
	}
};

uint32_t componentTypeID = 0;
static ComponentTypeDef componentTypes[] =
{
	{
		++componentTypeID,
			"",
			"",
			nullptr
	}
};

int main(int argc, char **argv)
{
	EntityManager manager;

	size_t i = 0;
	for(auto p : prototypes)
	{
		p.ctor(++i);
	}

	std::system("pause");
	return 0;
}