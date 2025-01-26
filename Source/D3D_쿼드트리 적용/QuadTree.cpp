#include "QuadTree.h"


void QuadTree::Initialize(ModelVertex* vertices, int cx, int cy)
{
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
}

std::vector<DWORD> QuadTree::GenerateIndices()
{
	std::vector<DWORD> indices; // 인덱스 벡터 초기화
	MakeUpIndices(root.get(), indices);
	return indices;
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
