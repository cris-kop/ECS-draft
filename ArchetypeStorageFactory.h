#ifndef ARCHETYPE_STORAGE_FACTORY_H
#define ARCHETYPE_STORAGE_FACTORY_H

#include <vector>
#include <map>
#include <unordered_map>
#include <functional>

#include "Archetypedata.h"
#include "ComponentSet.h"


struct ArchetypeStorageFactory
{
public:
	template<typename T>	
	void Register()
	{
		ComponentSet componentType = T::sType;
		if(componentType == ComponentSet::None)
		{
			return ;
		}

		ComponentSetToStorage.emplace(componentType, []()
		{
			return new ActualStorage<T>();
		});
	}

	ComponentStorage* Create(const ComponentSet componentType);
	void Destroy(ComponentStorage *pStorage);

private:
	std::unordered_map<ComponentSet, std::function<ComponentStorage*()>>ComponentSetToStorage;
};


#endif