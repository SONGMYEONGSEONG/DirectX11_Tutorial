#include "QuadTree.h"

void QuadTree::Initialize(ModelVertex* vertices, int cx, int cy)
{
	rootSize = cx;

	this->vertices = vertices;
	// ** 실재로 Top과 Bottom이 반대이다 **
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

	BuildNeighbor(root.get());//최상위 Node를 가지고 가 이웃Node을 작성한다. 

}

std::vector<DWORD> QuadTree::GenerateIndices()
{
	std::vector<DWORD> indices; // 인덱스 벡터 초기화

	if (lod)
	{
		MakeUpIndicesToLOD(root.get(), indices);	//LOD 객체가 존재하면 LOD 기법을 적용한 인덱스들을 구성
	}
	else
	{
		MakeUpIndices(root.get(), indices);	//LOD 객체가 없으면 Node에서 가지고있는 이미지의 크기가 1이 될때까지 구성(기존 쿼드트리 작업 실행)
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
	//node 변수 초기화 
	node->corners[Corner::TL] = topLeft;
	node->corners[Corner::TR] = topRight;
	node->corners[Corner::BL] = bottomLeft;
	node->corners[Corner::BR] = bottomRight;
	node->center = (topLeft + bottomRight) / 2;
	node->size = (topRight - topLeft);
	node->isCulled = false;

	//경계 구 의 반지름을 구하기 위한 연산
	float x = (vertices[bottomRight].pos.x - vertices[topLeft].pos.x);
	float y = (vertices[bottomRight].pos.y - vertices[topLeft].pos.y);
	float z = (vertices[bottomRight].pos.z - vertices[topLeft].pos.z);
	node->radius = std::sqrt(x * x + y * y + z * z) / 2; // 구 반지름 구하는 공식

	//노드가 가지고 있는 이미지 사이즈가 1이 되기 전까지 자식노드(4개)를 만듬
	if (1 < node->size)
	{
		int topCenter = (topLeft + topRight) / 2;
		int bottomCenter = (bottomLeft + bottomRight) / 2;
		int leftCenter = (topLeft + bottomLeft) / 2;
		int rightCenter = (topRight + bottomRight) / 2;


		//node->childs ( 자식노드 4개를 만들기위해 포인터 선언)
		for (int i = 0; 4 > i; i++) node->childs[i] = std::make_unique<QuadTreeNode>();
		//각 이미지를 4등분하여 각 구역마다 node를 초기화 함, 이때 node의 size가 1 이상이면 또 이작업을 반복함 
		Build(node->childs[0].get(), topLeft, topCenter, leftCenter, node->center);// top left
		Build(node->childs[1].get(), topCenter, topRight, node->center, rightCenter);// top right
		Build(node->childs[2].get(), leftCenter, node->center, bottomLeft, bottomCenter);// bottom left
		Build(node->childs[3].get(), node->center, rightCenter, bottomCenter, bottomRight);// bottom right
	}
}//QuadTree::Build()


void QuadTree::FrustumCulling(QuadTreeNode* node, class Frustum* frustum)
{
	//경계 구를 이용하여 해당 영역이 절두체 밖이 있는 지 확인(완전한 외부)
	if (!frustum->IsInFrustumBoundsSphereExceptUpDown(vertices[node->center].pos, node->radius))
	{
		//해당 예제에서는 Terrain의 vertex 배열을 가져올것이다. vertices[node->center] 인덱스에 있는 vertex를 클리핑 가능 여부 체크
		node->isCulled = true;
		return;
	}
	node->isCulled = false;

	//위에서 완전한 외부가 아닌경우 ( ClipSpace안에 물체가 있다는 뜻), Terrain을 4등분해서 클리핑 가능 여부 체크
	bool b[4];
	b[0] = frustum->IsInFrustumExceptUpDown(vertices[node->corners[Corner::TL]].pos);
	b[1] = frustum->IsInFrustumExceptUpDown(vertices[node->corners[Corner::TR]].pos);
	b[2] = frustum->IsInFrustumExceptUpDown(vertices[node->corners[Corner::BL]].pos);
	b[3] = frustum->IsInFrustumExceptUpDown(vertices[node->corners[Corner::BR]].pos);
	if (b[0] & b[1] & b[2] & b[3]) return; // &(and) 비트연산을해서 1(4구역 전부 true) 함수 탈출 // 전부 true가 나오면 자식 노드들을 탐색할 필요가 없기 때문 


	//위에서 함수가 반환이 안되고 노드의 이미지 크기가 1이 안된경우 자식노드들의 클리핑을 진행 , 1이 될때까지 반복 
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
	//실제로 Top과 Bottom이 반대이다.
	indices.push_back(node->corners[Corner::TL]);//bottom left;
	indices.push_back(node->corners[Corner::BL]);// top left
	indices.push_back(node->corners[Corner::TR]);// bottom right

	indices.push_back(node->corners[Corner::TR]);// bottom right
	indices.push_back(node->corners[Corner::BL]);// top left
	indices.push_back(node->corners[Corner::BR]);// top right
}

void QuadTree::MakeUpIndices(QuadTreeNode* node, std::vector<DWORD>& indices)
{
	//해당 노드가 컬링 여부가 True면 인덱스 생성 x -> 렌더링하지 않을건데 인덱스 만들필요 x
	if (node->isCulled)
	{
		node->isCulled = false;
		return;
	}

	//leaf node(자식노드)일 경우만, 노드의 이미지(구역) 크기가 1 이하인 경우 인덱스를 생성 
	if (1 >= node->size)
	{
		SetIndices(node, indices);
		return;
	}

	//위의 조건문이 false인경우 4개 자식노드에 index 생성 
	MakeUpIndices(node->childs[0].get(), indices);
	MakeUpIndices(node->childs[1].get(), indices);
	MakeUpIndices(node->childs[2].get(), indices);
	MakeUpIndices(node->childs[3].get(), indices);
}

void QuadTree::MakeUpIndicesToLOD(QuadTreeNode* node, std::vector<DWORD>& indices)
{
	//해당 노드가 컬링 여부가 True면 인덱스 생성 x -> 렌더링하지 않을건데 인덱스 만들필요 x
	if (node->isCulled)
	{
		node->isCulled = false;
		return;
	}
	//leaf node(자식노드)일 경우만, 노드의 이미지(구역) 크기가 1 이하인 경우 인덱스를 생성 
	if (1 >= node->size)
	{
		SetIndices(node, indices);
		return;
	}

	//조건에 맞다면 부모 노드를 이용해서 그린다.
	//현재 해당 노드의 이미지 중심점값의 vertex위치를 인자값으로 보내서 노드의 LodLevel를 반환한뒤,
	//해당 노드의 이미지 크기보다 큰경우 동작한다. 
	if (lod->GetInvLevel(vertices[node->center].pos) >= node->size)
	{
		bool b[4] = { true, true , true, true }; // 이웃 노드가 없다면 분할 할 필요가 없으므로 true로 초기화한다.
		
		for (int i = 0; 4 > i; i++)
		{
			if (node->neighbors[i])
			{
				//이웃된 노드들의 LOD레벨을 확인하여 반환 후 , 해당 이웃 노드의 size(이미지(height map) 크기보다 크면 true
				//즉, 이웃된 노드들이 같은 LodLevel인지 체크하는 함수 , 다를 경우 균열방지 및 폴리곤을 재생성해야되기때문에
				b[i] = (lod->GetInvLevel(vertices[node->neighbors[i]->center].pos) >= node->neighbors[i]->size);
			}
		}

		// 이웃 노드가 모두 Draw가능한 상태라면, 같은레벨
		// 따라서 할 필요가 없이 Draw한다. -> Index를 Set을 동작 
		if (b[0] & b[1] & b[2] & b[3])
		{
			SetIndices(node, indices);
			return;
		}
		
		//자신의 노드를 분할. -> 균열방지 작업의 시작. 
		//해당 정점과 이웃되지 않는 면을 찾아 균열이 발생한곳을 메꾸는 작업
		//Vertex들을 재활용하여 인덱스들을 더 생성해 폴리곤을 만들어낸다. 
		int n = 0;
		if (!b[0]) // 하단 분할 O
		{
			//Node Bottom : 해당 노드의 TL 좌표와 TR 좌표의 합의 /2 , 하단의 중심(center)
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
		else // 하단 분할 X
		{
			indices.push_back(node->center);				//node center
			indices.push_back(node->corners[Corner::TR]);	//node bottom right
			indices.push_back(node->corners[Corner::TL]);	//node bottom left
		}

		if (!b[1]) // 상단 분할 O
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
		else // 상단 분할 X
		{
			indices.push_back(node->center);				// node center
			indices.push_back(node->corners[Corner::BL]);	// node top left
			indices.push_back(node->corners[Corner::BR]);	// node top right
		}

		if (!b[2]) // 좌측 분할 O
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
		else // 좌측 분할 X
		{
			indices.push_back(node->center);				// node center
			indices.push_back(node->corners[Corner::TL]);	// node bottom left
			indices.push_back(node->corners[Corner::BL]);	// node top left
		}

		if (!b[3]) // 우측 분할.
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
	

	//자신이 Draw될 수 없다면 가능한 자식을 찾아서 Draw한다.
	//해당 노드의 레벨이 이미지의size보다 작은경우 자식을 찾음 
	MakeUpIndicesToLOD(node->childs[0].get(),indices);
	MakeUpIndicesToLOD(node->childs[1].get(), indices);
	MakeUpIndicesToLOD(node->childs[2].get(), indices);
	MakeUpIndicesToLOD(node->childs[3].get(), indices);
}//QuadTree::MakeUpIndicesToLOD()

void QuadTree::BuildNeighbor(QuadTreeNode* node)
{
	//최상위 부모노드는 작업 제외 
	if (root.get() != node)
	{
		int topLeft = node->corners[Corner::TL];
		int topRight = node->corners[Corner::TR];
		int bottomLeft = node->corners[Corner::BL];
		int bottomRight = node->corners[Corner::BR];
		
		//enum Neigbor(이웃노드들의 방향)값 만큼 반복문을 돌려 각 방향에 따른 이웃노드들의 data를 저장하는함수 
		for (int i = 0; 4 > i; i++)
		{
			node->neighbors[i] = FindNeighborNode((Neighbor)i, topLeft, topRight, bottomLeft, bottomRight);
		}
	}

	//해당 node의 heightmap size가 1보다 클경우 자식노드의 이웃노드들을 만듬
	if (1 < node->size)
	{
		BuildNeighbor(node->childs[0].get());
		BuildNeighbor(node->childs[1].get());
		BuildNeighbor(node->childs[2].get());
		BuildNeighbor(node->childs[3].get());
	}
}

//해당 노드의 data로 상하좌우 에있는 node들의 TL,TR,BL,BR,Center값을 지정하고 
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
	//center가 index 범위 안에 있는 값인가?
	//(rootSize * rootSize -1) : height map(이미지)의 넓이 -1 (cx=cy=5, 25지만 0~24까지만 사용하므로 -1)
	if ((0 <= center) && ((rootSize * rootSize - 1) >= center))
	{
		//위에 switch문에서 TL,TR,BL,BR을 지정한 노드 인자값을 가지고 이웃노드인지 찾기 시작 
		POINT pt = { vertices[center].pos.x , vertices[center].pos.z };
		return FindNode(root.get(), pt, topLeft, topRight, bottomLeft, bottomRight);
	}
	return nullptr;
} // QuadTree::FindNeighborNode()

QuadTreeNode* QuadTree::FindNode(QuadTreeNode* node, POINT center, const int& topLeft, const int& topRight, const int& bottomLeft, const int&
	bottomRight)
{
	//인자값으로 가져온 코너 좌표값과 해당 노드의 코너 좌표값이 같으면 인자값으로 가져온 node를 반환
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

			//자식노드의 영역에 인자값으로 들어온 center(pt,노드의 중심점)가 존재하면 , 그 자식 노드의 자식노드들을 탐색을 시작. 
			if (PtInRect(&rc, center))
			{
				findNode = FindNode(node->childs[i].get(), center, topLeft, topRight, bottomLeft, bottomRight);
			}

			//자식노드의 영엑에 center가 존재하지 않으면 그 노드가 이웃 노드이다. 
			if (findNode)
			{
				return findNode;
			}
		}
	}
	return nullptr;

}