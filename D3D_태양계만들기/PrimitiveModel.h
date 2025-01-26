//Primitive(프리미티브) : 이용 가능한 가장 단순한(작은)처리 단위 

#pragma once
#include "Model.h" 
class PrimitiveModel : public Model
{
public:
	virtual void MakePrimitive(const std::string& texFilename = "") {}
	virtual void Set_size(float radius) { };
};

//상자모양 Mesh
class PrimitiveCube : public PrimitiveModel
{
public:
	virtual void MakePrimitive(const std::string& texFilename = "") override;
};

//Sphere : 구
//구 모양 Mesh
class PrimitiveSphere : public PrimitiveModel
{
protected:
	enum { SectorCount = 20, StackCount = 20};
	float radius = 1;
public:
	virtual void MakePrimitive(const std::string& texFilename = "") override;
	virtual void Set_size(float radius) { this->radius = radius; };
};
