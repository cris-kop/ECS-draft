#include "ArchetypeStorageFactory.h"

ComponentStorage* ArchetypeStorageFactory::Create(const ComponentSet componentType)
{
	auto factoryIt = ComponentSetToStorage.find(componentType);
	if(factoryIt == ComponentSetToStorage.end())
	{
		return nullptr;
	}
	return factoryIt->second();
}

void ArchetypeStorageFactory::Destroy(ComponentStorage *pStorage)
{
	delete pStorage;
}