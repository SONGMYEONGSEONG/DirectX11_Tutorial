#pragma once
#include <vector>
#include "Model.h"
#include "Frustum.h"
enum Corner { TL =0, TR,BL,BR };

struct QuadTreeNode // 쿼드트리에서 사용할 노드 구조체를 만든다.
{
	int size; //노드에서 가지고있는 이미지의 크기(TR - TL)

	int center; // 이미즈를 4등분 했을때읭 중심점
	int corners[4]; // 각 Corner의 좌표를 배열을 가지고 있음
	//corners[Corner::TL]은 corners[0]와 같은 의미 

	bool isCulled; // 컬링 여부
	float radius; // 경계 구의 반지름

	std::unique_ptr<QuadTreeNode> childs[4]; // 자식노드(Leaf) 노드를 저장하는 배열
};

class QuadTree
{
private:
	int triangleCount, drawCount;
	ModelVertex* vertices; // vertex 변수, 외부에서 init을 할때 그 class의 vertex vector의 포인터 가져와서 사용하게됨
	std::unique_ptr<QuadTreeNode> root; //최상위 부모 노드, 이 노드 밑으로 자식노드들 4개씩 구성되어있다. 

public:
	void Initialize(ModelVertex* vertices, int cx, int cy); //  이미지의 w(cx),h(cy)을 가져와서 QuadTreeNode 를 초기화 한다.
	void ProcessFrustumCulling(class Frustum* frustum); // private의 FrustumCulling 함수를 호출하는 역할 
	std::vector<DWORD> GenerateIndices(); //인덱스를 구성하여 인덱스 벡터를 반환 , private의 MakeUpIndice를 호출 

private:
	void Build(QuadTreeNode* node, int topleft, int topright, int bottomleft, int bottomright);
	void FrustumCulling(QuadTreeNode* node, class Frustum* frustum);//4구역이 전부 클리핑이 되거나 아니면 노드의 이미지 사이즈가 1이하인 경우 함수가 반환하게 되어있음 
	void SetIndices(QuadTreeNode* node, std::vector<DWORD>& indeices);//vertex를 재활용하기 위한 index Set (그리는 순서)
	void MakeUpIndices(QuadTreeNode* node, std::vector<DWORD>& indices);//index들을 만들기 위한 함수 (SetIndeces를 호출)
};


