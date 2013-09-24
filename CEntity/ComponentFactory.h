#ifndef COMPONENTFACTORY_H
#define COMPONENTFACTORY_H

class IComponentFactory
{
public:
	virtual ~IComponentFactory() {}
	virtual ComponentData *CreateComponentData() = 0;
	virtual void DestroyComponentData(ComponentData *data) = 0;
};

template<class T>
class ComponentFactory : public IComponentFactory
{
public:
	static std::shared_ptr<ComponentFactory<T> > CreateFactory()
	{
		return std::make_shared<ComponentFactory<T> >();
	}

	virtual ComponentData *CreateComponentData() override
	{
		return new T();
	}

	virtual void DestroyComponentData(ComponentData *data) override
	{
		delete reinterpret_cast<T*>(data);
	}
};

#endif // COMPONENTFACTORY_H