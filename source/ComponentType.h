#pragma once
#include <set>
#include <typeinfo>

typedef size_t ComponentType;

typedef std::set<ComponentType> ComponentTypeSet;

namespace ecs {
	template<typename C>
	size_t getComponentType() {
		return typeid(C).hash_code();
	}
}