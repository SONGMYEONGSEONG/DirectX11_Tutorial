#include "QuadTree.h"


void QuadTree::Initialize(ModelVertex* vertices, int cx, int cy)
{
	this->vertices = vertices;
	// ** ����� Top�� Bottom�� �ݴ��̴� **
	// cx = 3, cy = 3   cx = 5, cy = 5
	// TL		  TR	TL			TR
	// 0 --- 1 --- 2	00-01-02-03-04  
	// |	 |	   |	05-06-07-08-09		 
	// 3 --- 4 --- 5	10-11-12-13-14
	// |	 |	   |	15-16-17-18-19	 
	// 6 --- 7 --- 8	20-21-22-23-24
	// BL	       BR	BL			BR
	root = std::make_unique<QuadTreeNode>();
	Build(root.get(), 0, (cx - 1), (cx * (cy - 1)), (cx * cy - 1));
}

std::vector<DWORD> QuadTree::GenerateIndices()
{
	std::vector<DWORD> indices; // �ε��� ���� �ʱ�ȭ
	MakeUpIndices(root.get(), indices);
	return indices;
}

void QuadTree::ProcessFrustumCulling(class Frustum* frustum) { FrustumCulling(root.get(), frustum); }

void QuadTree::Build(QuadTreeNode* node, int topLeft, int topRight, int bottomLeft, int bottomRight)
{
	//node ���� �ʱ�ȭ 
	node->corners[Corner::TL] = topLeft;
	node->corners[Corner::TR] = topRight;
	node->corners[Corner::BL] = bottomLeft;
	node->corners[Corner::BR] = bottomRight;
	node->center = (topLeft + bottomRight) / 2;
	node->size = (topRight - topLeft);
	node->isCulled = false;

	//��� �� �� �������� ���ϱ� ���� ����
	float x = (vertices[bottomRight].pos.x - vertices[topLeft].pos.x);
	float y = (vertices[bottomRight].pos.y - vertices[topLeft].pos.y);
	float z = (vertices[bottomRight].pos.z - vertices[topLeft].pos.z);
	node->radius = std::sqrt(x * x + y * y + z * z) / 2; // �� ������ ���ϴ� ����

	//��尡 ������ �ִ� �̹��� ����� 1�� �Ǳ� ������ �ڽĳ��(4��)�� ����
	if (1 < node->size)
	{
		int topCenter = (topLeft + topRight) / 2;
		int bottomCenter = (bottomLeft + bottomRight) / 2;
		int leftCenter = (topLeft + bottomLeft) / 2;
		int rightCenter = (topRight + bottomRight) / 2;


		//node->childs ( �ڽĳ�� 4���� ��������� ������ ����)
		for (int i = 0; 4 > i; i++) node->childs[i] = std::make_unique<QuadTreeNode>();
		//�� �̹����� 4����Ͽ� �� �������� node�� �ʱ�ȭ ��, �̶� node�� size�� 1 �̻��̸� �� ���۾��� �ݺ��� 
		Build(node->childs[0].get(), topLeft, topCenter, leftCenter, node->center);// top left
		Build(node->childs[1].get(), topCenter, topRight, node->center, rightCenter);// top right
		Build(node->childs[2].get(), leftCenter, node->center, bottomLeft, bottomCenter);// bottom left
		Build(node->childs[3].get(), node->center, rightCenter, bottomCenter, bottomRight);// bottom right
	}
}//QuadTree::Build()


void QuadTree::FrustumCulling(QuadTreeNode* node, class Frustum* frustum)
{
	//��� ���� �̿��Ͽ� �ش� ������ ����ü ���� �ִ� �� Ȯ��(������ �ܺ�)
	if (!frustum->IsInFrustumBoundsSphereExceptUpDown(vertices[node->center].pos, node->radius))
	{
		//�ش� ���������� Terrain�� vertex �迭�� �����ð��̴�. vertices[node->center] �ε����� �ִ� vertex�� Ŭ���� ���� ���� üũ
		node->isCulled = true;
		return;
	}
	node->isCulled = false;

	//������ ������ �ܺΰ� �ƴѰ�� ( ClipSpace�ȿ� ��ü�� �ִٴ� ��), Terrain�� 4����ؼ� Ŭ���� ���� ���� üũ
	bool b[4];
	b[0] = frustum->IsInFrustumExceptUpDown(vertices[node->corners[Corner::TL]].pos);
	b[1] = frustum->IsInFrustumExceptUpDown(vertices[node->corners[Corner::TR]].pos);
	b[2] = frustum->IsInFrustumExceptUpDown(vertices[node->corners[Corner::BL]].pos);
	b[3] = frustum->IsInFrustumExceptUpDown(vertices[node->corners[Corner::BR]].pos);
	if (b[0] & b[1] & b[2] & b[3]) return; // &(and) ��Ʈ�������ؼ� 1(4���� ���� true) �Լ� Ż�� // ���� true�� ������ �ڽ� ������ Ž���� �ʿ䰡 ���� ���� 


	//������ �Լ��� ��ȯ�� �ȵǰ� ����� �̹��� ũ�Ⱑ 1�� �ȵȰ�� �ڽĳ����� Ŭ������ ���� , 1�� �ɶ����� �ݺ� 
	if (1 < node->size)
	{
		FrustumCulling(node->childs[0].get(), frustum);
		FrustumCulling(node->childs[1].get(), frustum);
		FrustumCulling(node->childs[2].get(), frustum);
		FrustumCulling(node->childs[3].get(), frustum);
	}
} // QuadTree::FrustumCulling()

void QuadTree::SetIndices(QuadTreeNode* node, std::vector<DWORD>& indices)
{
	//������ Top�� Bottom�� �ݴ��̴�.
	indices.push_back(node->corners[Corner::TL]);//bottom left;
	indices.push_back(node->corners[Corner::BL]);// top left
	indices.push_back(node->corners[Corner::TR]);// bottom right

	indices.push_back(node->corners[Corner::TR]);// bottom right
	indices.push_back(node->corners[Corner::BL]);// top left
	indices.push_back(node->corners[Corner::BR]);// top right
}

void QuadTree::MakeUpIndices(QuadTreeNode* node, std::vector<DWORD>& indices)
{
	//�ش� ��尡 �ø� ���ΰ� True�� �ε��� ���� x -> ���������� �����ǵ� �ε��� �����ʿ� x
	if (node->isCulled)
	{
		node->isCulled = false;
		return;
	}

	//leaf node(�ڽĳ��)�� ��츸, ����� �̹���(����) ũ�Ⱑ 1 ������ ��� �ε����� ���� 
	if (1 >= node->size)
	{
		SetIndices(node, indices);
		return;
	}

	//���� ���ǹ��� false�ΰ�� 4�� �ڽĳ�忡 index ���� 
	MakeUpIndices(node->childs[0].get(), indices);
	MakeUpIndices(node->childs[1].get(), indices);
	MakeUpIndices(node->childs[2].get(), indices);
	MakeUpIndices(node->childs[3].get(), indices);
}
