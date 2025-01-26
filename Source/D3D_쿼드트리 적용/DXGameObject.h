#pragma once
#include <typeinfo> // auto 같은 자료형 사용시 유추하기 힘든 변수의 자료형을 확인하고자 할 때
#include <unordered_map>
#include <set>
#include <memory>
#include "Components/DXTransform.h"
#include "Components/ComponentException.h"

//reference_wrapper : 템플릿 등에 레퍼런스로 전달할수 있게하는 함수 
using TypeInfoRef = std::reference_wrapper<const std::type_info>;
using LPCOMP = std::unique_ptr<BaseComponent>;

struct HashCode
{
	std::size_t operator()(TypeInfoRef code) const
	{
		return code.get().hash_code();
	}
};

struct EqualTo
{
	bool operator()(TypeInfoRef lhs, TypeInfoRef rhs) const
	{
		return lhs.get() == rhs.get();
	}
};

class DXGameObject
{
private:
	std::unordered_map<TypeInfoRef, LPCOMP, HashCode, EqualTo> components;//<정렬 기준을 인자값>>
	//set 각 원소의 key값을 만들어 중복을 만들지 않게하는(고유한 객체) 컨테이너
	std::set<ActionComponent*> actionables;
	std::set<RenderComponent*> renderables;
	std::unique_ptr<DXTransform> transform;

public:
	DXGameObject();

	DXTransform* GetTransform() const;

	//이때 T는 DXTransform class가 대입된다.
	template<typename T, typename = std::enable_if<std::is_base_of<BaseComponent, T>::value>>
	T* AddComponent()
	{
		if (nullptr != components[typeid(T)])
		{
			std::string msg = "Already Component : ";
			msg += typeid(T).name();
			ThrowComponentException(msg);
		}

		components[typeid(T)] = std::make_unique<T>(this);
		switch (components[typeid(T)]->GetType())
		{
		case ComponentType::ACTION: actionables.insert(reinterpret_cast<ActionComponent*>(components[typeid(T)].get())); break;
		case ComponentType::RENDER: renderables.insert(reinterpret_cast<RenderComponent*>(components[typeid(T)].get())); break;
		}

		return dynamic_cast<T*>(components[typeid(T)].get());
	}
	template<typename T, typename = std::enable_if<std::is_base_of<BaseComponent, T>::value>>
	T* GetComponent()
	{
		if (components.end() == components.find(typeid(T))) return nullptr;
		return dynamic_cast<T*>(components[typeid(T)].get());
	}
	template<>
	DXTransform* GetComponent<DXTransform>()
	{
		if (nullptr != transform) return transform.get();
		return nullptr;
	}

	void Update();
	void Draw(const XMMATRIX& viewProjectionMatrix);

private:
	template<>
	DXTransform* AddComponent<DXTransform>()
	{
		if (nullptr != transform) return transform.get();

		transform = std::make_unique<DXTransform>(this);
		return transform.get();
	}
};