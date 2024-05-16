#ifndef ARCHETYPE_STORAGE_FACTORY_H
#define ARCHETYPE_STORAGE_FACTORY_H

#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <typeindex>

#include "Archetypedata.h"
#include "ComponentSet.h"
#include "ComponentMappings.h"


struct ArchetypeStorageFactory
{
public:
	template<typename T>	
	void Register()
	{
		ComponentSet componentType = ComponentSet::None;

		std::unordered_map<std::type_index, ComponentSet>::const_iterator setIt = gCOMPONENT_MAP.find(typeid(T));
		if(setIt != gCOMPONENT_MAP.end())
		{
			componentType = setIt->second;
		}
		ComponentSetToStorage.emplace(componentType, []()
		{
			return new ActualStorage<T>();
		});
	}

	ComponentStorage* Create(const ComponentSet componentType);

private:
	std::unordered_map<ComponentSet, std::function<ComponentStorage*()>>ComponentSetToStorage;
};


#endif