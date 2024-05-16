#ifndef COMPONENT_MAPPINGS_H
#define COMPONENT_MAPPINGS_H

#include "ComponentSet.h"
#include "Components.h"

#include <unordered_map>
#include <typeindex>


static const std::unordered_map<std::type_index, ComponentSet> gCOMPONENT_MAP = 
{
	{ typeid(TransformComponent), ComponentSet::Transform },
	{ typeid(CameraComponent), ComponentSet::Camera }
};

static const std::unordered_map<ComponentSet, std::type_index> gCOMPONENT_MAP_INV = 
{
	{ ComponentSet::Transform, typeid(TransformComponent) },
	{ ComponentSet::Camera, typeid(CameraComponent) }
};

#endif
