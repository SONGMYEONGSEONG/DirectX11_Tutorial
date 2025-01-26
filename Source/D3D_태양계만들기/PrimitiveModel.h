//Primitive(������Ƽ��) : �̿� ������ ���� �ܼ���(����)ó�� ���� 

#pragma once
#include "Model.h" 
class PrimitiveModel : public Model
{
public:
	virtual void MakePrimitive(const std::string& texFilename = "") {}
	virtual void Set_size(float radius) { };
};

//���ڸ�� Mesh
class PrimitiveCube : public PrimitiveModel
{
public:
	virtual void MakePrimitive(const std::string& texFilename = "") override;
};

//Sphere : ��
//�� ��� Mesh
class PrimitiveSphere : public PrimitiveModel
{
protected:
	enum { SectorCount = 20, StackCount = 20};
	float radius = 1;
public:
	virtual void MakePrimitive(const std::string& texFilename = "") override;
	virtual void Set_size(float radius) { this->radius = radius; };
};
