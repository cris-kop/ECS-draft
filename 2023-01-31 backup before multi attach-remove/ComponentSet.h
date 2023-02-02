#ifndef COMPONENTSET_H
#define COMPONENTSET_H

#include <cstdint>


enum class ComponentSet : uint64_t
{
	None = 0x00,
    Transform = 0x01,
	Camera = 0x02
};


ComponentSet operator ~ (const ComponentSet pComponentSet)
{
    using T = std::underlying_type_t<ComponentSet>;
    return static_cast<ComponentSet>(~static_cast<T>(pComponentSet));
}

ComponentSet operator | (const ComponentSet lhs, const ComponentSet rhs)
{
    using T = std::underlying_type_t <ComponentSet>;
    return static_cast<ComponentSet>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

ComponentSet& operator |= (ComponentSet& lhs, const ComponentSet rhs)
{
    using T = std::underlying_type_t <ComponentSet>;
    lhs = static_cast<ComponentSet>(static_cast<T>(lhs) | static_cast<T>(rhs));
    return lhs;
}

ComponentSet operator & (const ComponentSet lhs, const ComponentSet rhs)
{
    using T = std::underlying_type_t <ComponentSet>;
    return static_cast<ComponentSet>(static_cast<T>(lhs) & static_cast<T>(rhs));;
}

ComponentSet operator &= (ComponentSet &lhs, const ComponentSet rhs)
{
    using T = std::underlying_type_t <ComponentSet>;
    lhs = static_cast<ComponentSet>(static_cast<T>(lhs) & static_cast<T>(rhs));;
    return lhs;
}

bool SetHasComponent(const ComponentSet pSet, const ComponentSet pHasComponent)
{
	if(std::underlying_type_t<ComponentSet>(pHasComponent & pSet))
    {
        return true;
    }
	return false;
}

bool Contains(const ComponentSet pSource, const ComponentSet pIncludesSet)
{
    return((pSource & pIncludesSet) == pIncludesSet);
}

bool SetHasOneComponent(const ComponentSet pSet)
{
    uint64_t componentSet = static_cast<uint64_t>(pSet);
    if(componentSet&(((bool)(componentSet&(componentSet-1)))-1))
    {
        return true;
    }
    return false;
}

#endif