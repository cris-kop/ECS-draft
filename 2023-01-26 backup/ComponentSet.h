#include <cstdint>


enum class ComponentSet : uint64_t
{
	Transform = 0x01,
	Camera = 0x02
};

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

ComponentSet operator& (const ComponentSet lhs, const ComponentSet rhs)
{
    using T = std::underlying_type_t <ComponentSet>;
    return static_cast<ComponentSet>(static_cast<T>(lhs) & static_cast<T>(rhs));;
}

bool Contains(const ComponentSet pSource, const ComponentSet pIncludesSet)
{
	return ((pSource & pIncludesSet) == pSource);
}

bool SetHasComponent(const ComponentSet pSet, const ComponentSet pHasComponent)
{
	if(std::underlying_type<ComponentSet>::type(pHasComponent & pSet)) return true;
	return false;
}
