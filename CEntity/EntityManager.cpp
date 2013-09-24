#include "EntityManager.h"

EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
	DestroyAllEntities();
}

void EntityManager::RegisterComponentType(const ComponentTypeDef &def)
{
	RE_ASSERT(def.type != INVALID_COMPONENT_TYPE);
	RE_ASSERT(_prototypes.count(def.type) > 0);

	std::pair<ComponentTypeID, ComponentType> value;
	value.first = def.type;
	value.second.factory = def.factory;
	value.second.name = def.name;
	value.second.description = def.description;

	_componentTypes.insert(value);
	_componentTypesIndex.insert(std::make_pair(def.name, def.type));

	_entityComponents[def.type];
	_componentData[def.type];
}

void EntityManager::RegisterComponentTypes(const std::vector<ComponentTypeDef> &defs)
{
	for(auto def : defs)
	{
		RegisterComponentType(def);
	}
}

void EntityManager::RegisterPrototype(const PrototypeDef &def)
{
	static PrototypeConstructor defaultCtor = [](const EntityID){};

	RE_ASSERT(def.prototype != INVALID_PROTOTYPE);
	RE_ASSERT(_prototypes.count(def.prototype) > 0);

	std::pair<PrototypeID, Prototype> value;
	value.first = def.prototype;
	value.second.name = def.name;
	value.second.description = def.description;
	value.second.ctor = def.ctor ? def.ctor : defaultCtor;

	_prototypes.insert(value);
	_prototypesIndex.insert(std::make_pair(def.name, def.prototype));
}

void EntityManager::RegisterPrototypes(const std::vector<PrototypeDef> &defs)
{
	for(auto def : defs)
	{
		RegisterPrototype(def);
	}
}


void EntityManager::AddComponentToPrototype(const ComponentTypeID type, const PrototypeID prototype)
{
	RE_ASSERT(_prototypes.count(prototype) > 0);
	RE_ASSERT(_componentTypes.count(type) > 0);

	_prototypeComponents.insert(std::make_pair(prototype, type));
}

void EntityManager::AddComponentsToPrototype(const std::vector<ComponentTypeID> &types, const PrototypeID prototype)
{
	for(auto type : types)
	{
		AddComponentToPrototype(type, prototype);
	}
}

void EntityManager::AddDependencyToPrototype(const PrototypeID prototype, const PrototypeID dependency)
{
	RE_ASSERT(_prototypes.count(prototype) > 0);
	RE_ASSERT(_prototypes.count(dependency) > 0);

	_prototypeDependencies.insert(std::make_pair(prototype, dependency));
}

void EntityManager::AddDependenciesToPrototype(const PrototypeID prototype, const std::vector<PrototypeID> &dependencies)
{
	for(auto dependency : dependencies)
	{
		AddDependencyToPrototype(prototype, dependency);
	}
}

EntityID EntityManager::CreateEntity(const std::string &description)
{
	std::pair<EntityID, Entity> value;
	value.first = _entityIDAlloc.Allocate();
	value.second.description = description;
	value.second.prototype = INVALID_PROTOTYPE;

	_entities.insert(value);
	return value.first;
}

EntityID EntityManager::CreateEntity(const PrototypeID prototype, const std::string &description)
{
	// TODO: Prototype stuff below this

	EntityID entity = CreateEntity(description);
	ApplyPrototype(entity, prototype);
	return entity;
}

void EntityManager::DestroyEntity(const EntityID entity)
{
	for(auto type : QueryComponentTypes())
	{
		DestroyComponents(entity, type.first);
	}

	_entities.erase(entity);
	_entityIDAlloc.Free(entity);
}

void EntityManager::DestroyAllEntities()
{
	while(_entities.rbegin() != _entities.rend())
	{
		DestroyEntity((*_entities.rbegin()).first);
	}
}

ComponentID EntityManager::CreateComponent(const ComponentTypeID type, const EntityID entity)
{
	RE_ASSERT(_componentTypes.count(type) > 0);

	std::pair<EntityID, Component> componentValue;
	componentValue.first = entity;
	componentValue.second.component_id = _componentIDAlloc.Allocate();

	auto &factory = _componentTypes.at(type).factory;
	auto componentDataValue = std::make_pair(componentValue.second.component_id, factory->CreateComponentData());
	componentDataValue.second->component_id = componentDataValue.first;
	componentDataValue.second->component_type_id = type;
	componentDataValue.second->entity_id = componentValue.first;

	_entityComponents.at(type).insert(componentValue);
	_componentData.at(type).insert(componentDataValue);
	return componentValue.second.component_id;
}

void EntityManager::DestroyComponent(const ComponentID component, const ComponentTypeID type)
{
	auto &factory = _componentTypes.at(type).factory;
	auto &componentData = LookupComponentData(component, type);
	const EntityID entity = componentData.entity_id;

	_componentData.at(type).erase(component);
	
	auto result = QueryEntityComponents(entity, type);
	for(auto i = result.begin(); i != result.end(); i++)
	{
		if((*i).second.component_id == component)
		{
			_entityComponents.at(type).erase(i);
			break;
		}
	}

	_componentIDAlloc.Free(component);
}

void EntityManager::DestroyComponents(const EntityID entity, const ComponentTypeID type)
{
	auto &factory = _componentTypes.at(type).factory;
	for(auto i : QueryEntityComponents(entity, type))
	{
		auto &componentData = LookupComponentData(i.second.component_id, type);
		factory->DestroyComponentData(&componentData);
		_componentData.at(type).erase(i.second.component_id);
		_componentIDAlloc.Free(i.second.component_id);
	}

	_entityComponents.at(type).erase(entity);
}

void EntityManager::ApplyPrototype(const EntityID entity, const PrototypeID prototype)
{
	for(auto component : QueryPrototypeComponents(prototype))
	{
		CreateComponent(component.second, entity);
	}

	auto const &tmp = LookupPrototype(prototype);
	if(tmp.ctor)
	{
		tmp.ctor(entity);
	}
}

const ComponentType &EntityManager::LookupComponentType(const ComponentTypeID type) const
{
	return _componentTypes.at(type);
}

const ComponentType &EntityManager::LookupComponentType(const std::string &name) const
{
	return _componentTypes.at(LookupComponentTypeID(name));
}

ComponentTypeID EntityManager::LookupComponentTypeID(const std::string &name) const
{
	return _componentTypesIndex.at(name);
}

const Prototype &EntityManager::LookupPrototype(const PrototypeID prototype) const
{
	return _prototypes.at(prototype);
}

const Prototype &EntityManager::LookupPrototype(const std::string &name) const
{
	return _prototypes.at(LookupPrototypeID(name));
}

PrototypeID EntityManager::LookupPrototypeID(const std::string &name) const
{
	return _prototypesIndex.at(name);
}

const Entity &EntityManager::LookupEntity(const EntityID entity) const
{
	return _entities.at(entity);
}
	
ComponentData &EntityManager::LookupComponentData(const ComponentID component, const ComponentTypeID type)
{
	return *_componentData.at(type).at(component);
}

const ComponentData &EntityManager::LookupComponentData(const ComponentID component, const ComponentTypeID type) const
{
	return *_componentData.at(type).at(component);
}

QueryResult<EntityManager::ComponentTypeTable::const_iterator> EntityManager::QueryComponentTypes() const
{
	return QueryResult<ComponentTypeTable::const_iterator>(std::make_pair(
		_componentTypes.begin(),
		_componentTypes.end()));
}

QueryResult<EntityManager::EntityTable::const_iterator> EntityManager::QueryEntities() const
{
	return QueryResult<EntityTable::const_iterator>(std::make_pair(_entities.begin(), _entities.end()));
}

QueryResult<EntityManager::EntityComponentTable::value_type::second_type::const_iterator> EntityManager::QueryComponents(ComponentTypeID type) const
{
	return QueryResult<EntityComponentTable::value_type::second_type::const_iterator>(std::make_pair(
		_entityComponents.at(type).begin(),
		_entityComponents.at(type).end()));
}

QueryResult<EntityManager::EntityComponentTable::value_type::second_type::const_iterator> EntityManager::QueryEntityComponents(EntityID entity, ComponentTypeID type) const
{
	return QueryResult<EntityComponentTable::value_type::second_type::const_iterator>(
		_entityComponents.at(type).equal_range(entity));
}

QueryResult<EntityManager::ComponentDataTable::value_type::second_type::const_iterator> EntityManager::QueryComponentData(ComponentTypeID type) const
{
	return QueryResult<ComponentDataTable::value_type::second_type::const_iterator>(std::make_pair(
		_componentData.at(type).begin(),
		_componentData.at(type).end()));
}

QueryResult<EntityManager::PrototypeTable::const_iterator> EntityManager::QueryPrototypes() const
{
	return QueryResult<PrototypeTable::const_iterator>(std::make_pair(
		_prototypes.begin(),
		_prototypes.end()));
}

QueryResult<EntityManager::PrototypeComponentTable::const_iterator> EntityManager::QueryPrototypeComponents() const
{
	return QueryResult<PrototypeComponentTable::const_iterator>(std::make_pair(
		_prototypeComponents.begin(),
		_prototypeComponents.end()));
}

QueryResult<EntityManager::PrototypeComponentTable::const_iterator> EntityManager::QueryPrototypeComponents(PrototypeID prototype) const
{
	return QueryResult<PrototypeComponentTable::const_iterator>(
		_prototypeComponents.equal_range(prototype));
}