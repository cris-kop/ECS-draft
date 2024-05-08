#ifndef COMPONENTSET_H
#define COMPONENTSET_H

#include <cstdint>
#include <type_traits>


enum class ComponentSet : uint64_t
{
	None        = 0,            // 0
    Transform   = 1 << 0,       // 1
	Camera      = 1 << 1        // 2
};

inline ComponentSet operator ~ (const ComponentSet pComponentSet)
{
    using T = std::underlying_type_t<ComponentSet>;
    return static_cast<ComponentSet>(~static_cast<T>(pComponentSet));
}

inline ComponentSet operator | (const ComponentSet lhs, const ComponentSet rhs)
{
    using T = std::underlying_type_t <ComponentSet>;
    return static_cast<ComponentSet>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

inline ComponentSet& operator |= (ComponentSet& lhs, const ComponentSet rhs)
{
    using T = std::underlying_type_t <ComponentSet>;
    lhs = static_cast<ComponentSet>(static_cast<T>(lhs) | static_cast<T>(rhs));
    return lhs;
}

inline ComponentSet operator & (const ComponentSet lhs, const ComponentSet rhs)
{
    using T = std::underlying_type_t <ComponentSet>;
    return static_cast<ComponentSet>(static_cast<T>(lhs) & static_cast<T>(rhs));;
}

inline ComponentSet operator &= (ComponentSet &lhs, const ComponentSet rhs)
{
    using T = std::underlying_type_t <ComponentSet>;
    lhs = static_cast<ComponentSet>(static_cast<T>(lhs) & static_cast<T>(rhs));;
    return lhs;
}

inline bool SetHasComponent(const ComponentSet pSet, const ComponentSet pHasComponent)
{
	if(std::underlying_type_t<ComponentSet>(pHasComponent & pSet))
    {
        return true;
    }
	return false;
}

inline bool Contains(const ComponentSet pSource, const ComponentSet pIncludesSet)
{
    return((pSource & pIncludesSet) == pIncludesSet);
}

inline bool SetHasOneComponent(const ComponentSet pSet)
{
    uint64_t componentSet = static_cast<uint64_t>(pSet);
    if(componentSet&(((bool)(componentSet&(componentSet-1)))-1))
    {
        return true;
    }
    return false;
}

#endif