#ifndef TYPES_H
#define TYPES_H

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <cstdint>
#include <cassert>

#define RE_ASSERT(x) assert(x)

#define INVALID_ENTITY 0xFFFFFFFF
#define INVALID_PROTOTYPE 0xFFFFFFFF
#define INVALID_COMPONENT 0xFFFFFFFF
#define INVALID_COMPONENT_TYPE 0xFFFFFFFF

typedef uint32_t ComponentTypeID;
typedef uint32_t EntityID;
typedef uint32_t ComponentID;
typedef uint32_t PrototypeID;

struct Component;
class ComponentData;
struct ComponentType;
struct Entity;
struct Prototype;
class IComponentFactory;
class EntityManager;

typedef std::function<void (const EntityID)> PrototypeConstructor;

class ComponentData
{
public:
	// For dynamic_cast support
	virtual ~ComponentData(){}

	ComponentID component_id;
	ComponentTypeID component_type_id;
	EntityID entity_id;
};

struct ComponentType
{
	std::shared_ptr<IComponentFactory> factory;
	std::string name;
	std::string description;
};

struct Component
{
	ComponentID component_id;
};

struct Entity
{
	std::string description;
	PrototypeID prototype;
};

struct Prototype
{
	std::string name;
	std::string description;
	PrototypeConstructor ctor;
};

struct ComponentTypeDef
{
	ComponentTypeID type;
	std::string name;
	std::string description;
	std::shared_ptr<IComponentFactory> factory;
};

struct PrototypeDef
{
	PrototypeID prototype;
	std::string name;
	std::string description;
	PrototypeConstructor ctor;
};

#endif // TYPES_H