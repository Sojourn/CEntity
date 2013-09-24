#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include "RelationTypes.h"
#include "IndexAllocator.h"
#include "ComponentFactory.h"
#include "QueryResult.h"
#include <map>

class EntityManager
{
public:
	typedef std::map<ComponentTypeID, ComponentType> ComponentTypeTable;
	typedef std::map<EntityID, Entity> EntityTable;
	typedef std::map<ComponentTypeID,
		std::multimap<EntityID, Component> > EntityComponentTable;
	typedef std::map<ComponentTypeID,
		std::map<ComponentID, ComponentData*> > ComponentDataTable;
	typedef std::map<PrototypeID, Prototype> PrototypeTable;
	typedef std::multimap<PrototypeID, ComponentTypeID> PrototypeComponentTable;
	typedef std::multimap<PrototypeID, PrototypeID> PrototypeDependencyTable;
	typedef std::map<std::string, ComponentTypeID> ComponentTypeIndex;
	typedef std::map<std::string, PrototypeID> PrototypeIndex;

	EntityManager();
	~EntityManager();

	void RegisterComponentType(const ComponentTypeDef &def);
	void RegisterComponentTypes(const std::vector<ComponentTypeDef> &defs);

	void RegisterPrototype(const PrototypeDef &def);
	void RegisterPrototypes(const std::vector<PrototypeDef> &defs);
	void AddComponentToPrototype(const ComponentTypeID type, const PrototypeID prototype);
	void AddComponentsToPrototype(const std::vector<ComponentTypeID> &types, const PrototypeID prototype);
	void AddDependencyToPrototype(const PrototypeID prototype, const PrototypeID dependency);
	void AddDependenciesToPrototype(const PrototypeID prototype, const std::vector<PrototypeID> &dependencies);

	EntityID CreateEntity(const std::string &description = "");
	EntityID CreateEntity(const PrototypeID prototype, const std::string &description = "");
	void DestroyEntity(const EntityID entity);
	void DestroyAllEntities();

	ComponentID CreateComponent(const ComponentTypeID type, const EntityID entity);
	void DestroyComponent(const ComponentID component, const ComponentTypeID type);
	void DestroyComponents(const EntityID entity, const ComponentTypeID type);
	void ApplyPrototype(const EntityID entity, const PrototypeID prototype);

	ComponentData &LookupComponentData(const ComponentID component, const ComponentTypeID type);

	const ComponentType &LookupComponentType(const ComponentTypeID type) const;
	const ComponentType &LookupComponentType(const std::string &name) const;
	ComponentTypeID LookupComponentTypeID(const std::string &name) const;
	const Prototype &LookupPrototype(const PrototypeID prototype) const;
	const Prototype &LookupPrototype(const std::string &name) const;
	PrototypeID LookupPrototypeID(const std::string &name) const;
	const Entity &LookupEntity(const EntityID entity) const;
	const ComponentData &LookupComponentData(const ComponentID component, const ComponentTypeID type) const;

	QueryResult<ComponentTypeTable::const_iterator> QueryComponentTypes() const;
	QueryResult<EntityTable::const_iterator> QueryEntities() const;
	QueryResult<EntityComponentTable::value_type::second_type::const_iterator> QueryComponents(ComponentTypeID type) const;
	QueryResult<EntityComponentTable::value_type::second_type::const_iterator> QueryEntityComponents(EntityID entity, ComponentTypeID type) const;
	QueryResult<ComponentDataTable::value_type::second_type::const_iterator> QueryComponentData(ComponentTypeID type) const;
	QueryResult<PrototypeTable::const_iterator> QueryPrototypes() const;
	QueryResult<PrototypeComponentTable::const_iterator> QueryPrototypeComponents() const;
	QueryResult<PrototypeComponentTable::const_iterator> QueryPrototypeComponents(PrototypeID prototype) const;

private:
	ComponentTypeTable _componentTypes;
	EntityTable _entities;
	EntityComponentTable _entityComponents;
	ComponentDataTable _componentData;
	PrototypeTable _prototypes;
	PrototypeDependencyTable _prototypeDependencies;
	PrototypeComponentTable _prototypeComponents;

	ComponentTypeIndex _componentTypesIndex;
	PrototypeIndex _prototypesIndex;

	IndexAllocator _entityIDAlloc;
	IndexAllocator _componentIDAlloc;
};

#endif // ENTITYMANAGER_H