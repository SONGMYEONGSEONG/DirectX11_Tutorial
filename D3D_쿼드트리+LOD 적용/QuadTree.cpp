#include "QuadTree.h"

void QuadTree::Initialize(ModelVertex* vertices, int cx, int cy)
{
	rootSize = cx;

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

	BuildNeighbor(root.get());//�ֻ��� Node�� ������ �� �̿�Node�� �ۼ��Ѵ�. 

}

std::vector<DWORD> QuadTree::GenerateIndices()
{
	std::vector<DWORD> indices; // �ε��� ���� �ʱ�ȭ

	if (lod)
	{
		MakeUpIndicesToLOD(root.get(), indices);	//LOD ��ü�� �����ϸ� LOD ����� ������ �ε������� ����
	}
	else
	{
		MakeUpIndices(root.get(), indices);	//LOD ��ü�� ������ Node���� �������ִ� �̹����� ũ�Ⱑ 1�� �ɶ����� ����(���� ����Ʈ�� �۾� ����)
	}
	return indices;
}

void QuadTree::SetLOD(LOD* lod)
{
	this->lod = lod;
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

void QuadTree::MakeUpIndicesToLOD(QuadTreeNode* node, std::vector<DWORD>& indices)
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

	//���ǿ� �´ٸ� �θ� ��带 �̿��ؼ� �׸���.
	//���� �ش� ����� �̹��� �߽������� vertex��ġ�� ���ڰ����� ������ ����� LodLevel�� ��ȯ�ѵ�,
	//�ش� ����� �̹��� ũ�⺸�� ū��� �����Ѵ�. 
	if (lod->GetInvLevel(vertices[node->center].pos) >= node->size)
	{
		bool b[4] = { true, true , true, true }; // �̿� ��尡 ���ٸ� ���� �� �ʿ䰡 �����Ƿ� true�� �ʱ�ȭ�Ѵ�.
		
		for (int i = 0; 4 > i; i++)
		{
			if (node->neighbors[i])
			{
				//�̿��� ������ LOD������ Ȯ���Ͽ� ��ȯ �� , �ش� �̿� ����� size(�̹���(height map) ũ�⺸�� ũ�� true
				//��, �̿��� ������ ���� LodLevel���� üũ�ϴ� �Լ� , �ٸ� ��� �տ����� �� �������� ������ؾߵǱ⶧����
				b[i] = (lod->GetInvLevel(vertices[node->neighbors[i]->center].pos) >= node->neighbors[i]->size);
			}
		}

		// �̿� ��尡 ��� Draw������ ���¶��, ��������
		// ���� �� �ʿ䰡 ���� Draw�Ѵ�. -> Index�� Set�� ���� 
		if (b[0] & b[1] & b[2] & b[3])
		{
			SetIndices(node, indices);
			return;
		}
		
		//�ڽ��� ��带 ����. -> �տ����� �۾��� ����. 
		//�ش� ������ �̿����� �ʴ� ���� ã�� �տ��� �߻��Ѱ��� �޲ٴ� �۾�
		//Vertex���� ��Ȱ���Ͽ� �ε������� �� ������ �������� ������. 
		int n = 0;
		if (!b[0]) // �ϴ� ���� O
		{
			//Node Bottom : �ش� ����� TL ��ǥ�� TR ��ǥ�� ���� /2 , �ϴ��� �߽�(center)
			n = (node->corners[Corner::TL] + node->corners[Corner::TR]) * 0.5f;

			//Down Left Division
			indices.push_back(node->center);				//node center
			indices.push_back(n);							//node bottom
			indices.push_back(node->corners[Corner::TL]);	//node bottom left

			//Down Right Division
			indices.push_back(node->center);				//node center
			indices.push_back(node->corners[Corner::TR]);	//node bottom right
			indices.push_back(n);							//node bottom
		}
		else // �ϴ� ���� X
		{
			indices.push_back(node->center);				//node center
			indices.push_back(node->corners[Corner::TR]);	//node bottom right
			indices.push_back(node->corners[Corner::TL]);	//node bottom left
		}

		if (!b[1]) // ��� ���� O
		{
			// Node Top
			n = (node->corners[Corner::BL] + node->corners[Corner::BR]) * 0.5f;

			// Top Left Division
			indices.push_back(node->center);				// node center
			indices.push_back(node->corners[Corner::BL]);	// node top left
			indices.push_back(n);							 // node top

			// Top Right Division
			indices.push_back(node->center);				// node center
			indices.push_back(n);							// node top
			indices.push_back(node->corners[Corner::BR]);	// node top right
		}
		else // ��� ���� X
		{
			indices.push_back(node->center);				// node center
			indices.push_back(node->corners[Corner::BL]);	// node top left
			indices.push_back(node->corners[Corner::BR]);	// node top right
		}

		if (!b[2]) // ���� ���� O
		{
			// Node Left
			n = (node->corners[Corner::TL] + node->corners[Corner::BL]) * 0.5f;

			// Left Bottom Division
			indices.push_back(node->center);				// node center
			indices.push_back(node->corners[Corner::TL]);	// node bottom left
			indices.push_back(n);							// node left

			// Left Top Division
			indices.push_back(node->center);				// node center
			indices.push_back(n);							// node left
			indices.push_back(node->corners[Corner::BL]);	// node top right
		}
		else // ���� ���� X
		{
			indices.push_back(node->center);				// node center
			indices.push_back(node->corners[Corner::TL]);	// node bottom left
			indices.push_back(node->corners[Corner::BL]);	// node top left
		}

		if (!b[3]) // ���� ����.
		{
			// Node Right
			n = (node->corners[Corner::TR] + node->corners[Corner::BR]) * 0.5f;

			// Right Top Division
			indices.push_back(node->center);				// node center
			indices.push_back(node->corners[Corner::BR]);	// node top right
			indices.push_back(n);							// node right

			// Right Bottom Division
			indices.push_back(node->center);				// node center
			indices.push_back(n);							// node right
			indices.push_back(node->corners[Corner::TR]);	// node bottom right
		}
		else
		{
			indices.push_back(node->center);				// node center
			indices.push_back(node->corners[Corner::BR]);	// node top right
			indices.push_back(node->corners[Corner::TR]);	// node bottom right
		}
		return;
	}
	

	//�ڽ��� Draw�� �� ���ٸ� ������ �ڽ��� ã�Ƽ� Draw�Ѵ�.
	//�ش� ����� ������ �̹�����size���� ������� �ڽ��� ã�� 
	MakeUpIndicesToLOD(node->childs[0].get(),indices);
	MakeUpIndicesToLOD(node->childs[1].get(), indices);
	MakeUpIndicesToLOD(node->childs[2].get(), indices);
	MakeUpIndicesToLOD(node->childs[3].get(), indices);
}//QuadTree::MakeUpIndicesToLOD()

void QuadTree::BuildNeighbor(QuadTreeNode* node)
{
	//�ֻ��� �θ���� �۾� ���� 
	if (root.get() != node)
	{
		int topLeft = node->corners[Corner::TL];
		int topRight = node->corners[Corner::TR];
		int bottomLeft = node->corners[Corner::BL];
		int bottomRight = node->corners[Corner::BR];
		
		//enum Neigbor(�̿������� ����)�� ��ŭ �ݺ����� ���� �� ���⿡ ���� �̿������� data�� �����ϴ��Լ� 
		for (int i = 0; 4 > i; i++)
		{
			node->neighbors[i] = FindNeighborNode((Neighbor)i, topLeft, topRight, bottomLeft, bottomRight);
		}
	}

	//�ش� node�� heightmap size�� 1���� Ŭ��� �ڽĳ���� �̿������� ����
	if (1 < node->size)
	{
		BuildNeighbor(node->childs[0].get());
		BuildNeighbor(node->childs[1].get());
		BuildNeighbor(node->childs[2].get());
		BuildNeighbor(node->childs[3].get());
	}
}

//�ش� ����� data�� �����¿� ���ִ� node���� TL,TR,BL,BR,Center���� �����ϰ� 
QuadTreeNode* QuadTree::FindNeighborNode(Neighbor pos, int topLeft, int topRight, int bottomLeft, int bottomRight)
{
	int gap = topRight - topLeft;

	switch (pos)
	{
	case Neighbor::Up:
		bottomLeft = topLeft;
		bottomRight = topRight;

		topLeft = topLeft - rootSize * gap;
		topRight = topRight - rootSize * gap;
		break;

	case Neighbor::Down:
		topLeft = bottomLeft;
		topRight = bottomRight;

		bottomLeft = bottomLeft + rootSize * gap;
		bottomRight = bottomRight + rootSize * gap;
		break;

	case Neighbor::Left:
		topRight = topLeft;
		topLeft = topLeft - gap;

		bottomRight = bottomLeft;
		bottomLeft = bottomLeft - gap;
		break;

	case Neighbor::Right:
		topLeft = topRight;
		topRight = topRight + gap;

		bottomLeft = bottomRight;
		bottomRight = bottomRight + gap;
		break;

	default: 
		return nullptr;
	}

	int center = (topLeft + bottomRight) * 0.5f;
	//center�� index ���� �ȿ� �ִ� ���ΰ�?
	//(rootSize * rootSize -1) : height map(�̹���)�� ���� -1 (cx=cy=5, 25���� 0~24������ ����ϹǷ� -1)
	if ((0 <= center) && ((rootSize * rootSize - 1) >= center))
	{
		//���� switch������ TL,TR,BL,BR�� ������ ��� ���ڰ��� ������ �̿�������� ã�� ���� 
		POINT pt = { vertices[center].pos.x , vertices[center].pos.z };
		return FindNode(root.get(), pt, topLeft, topRight, bottomLeft, bottomRight);
	}
	return nullptr;
} // QuadTree::FindNeighborNode()

QuadTreeNode* QuadTree::FindNode(QuadTreeNode* node, POINT center, const int& topLeft, const int& topRight, const int& bottomLeft, const int&
	bottomRight)
{
	//���ڰ����� ������ �ڳ� ��ǥ���� �ش� ����� �ڳ� ��ǥ���� ������ ���ڰ����� ������ node�� ��ȯ
	if (topLeft == node->corners[Corner::TL] &&
		topRight == node->corners[Corner::TR] &&
		bottomLeft == node->corners[Corner::BL] &&
		bottomRight == node->corners[Corner::BR]) return node;

	if (1 < node->size)
	{
		RECT rc;
		XMFLOAT3 TL, BR;
		QuadTreeNode* findNode = nullptr;

		
		for (int i = 0; 4 > i; i++)
		{
			TL = vertices[node->childs[i]->corners[Corner::TL]].pos;
			BR = vertices[node->childs[i]->corners[Corner::BR]].pos;
			SetRect(&rc, TL.x, TL.z, BR.x, BR.z);

			//�ڽĳ���� ������ ���ڰ����� ���� center(pt,����� �߽���)�� �����ϸ� , �� �ڽ� ����� �ڽĳ����� Ž���� ����. 
			if (PtInRect(&rc, center))
			{
				findNode = FindNode(node->childs[i].get(), center, topLeft, topRight, bottomLeft, bottomRight);
			}

			//�ڽĳ���� ������ center�� �������� ������ �� ��尡 �̿� ����̴�. 
			if (findNode)
			{
				return findNode;
			}
		}
	}
	return nullptr;

}