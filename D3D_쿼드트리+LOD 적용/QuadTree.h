#pragma once
#include <vector>
#include "Model.h"
#include "Frustum.h"
#include "LOD.h"
enum Corner { TL =0, TR,BL,BR };

struct QuadTreeNode // 쿼드트리에서 사용할 노드 구조체를 만든다.
{
	int size; //노드에서 가지고있는 이미지(HeightMap)의 크기(TR - TL)

	int center; // 이미지를 4등분 했을때 중심점
	int corners[4]; // 각 Corner의 좌표를 배열을 가지고 있음
	//corners[Corner::TL]은 corners[0]와 같은 의미 

	bool isCulled; // 컬링 여부
	float radius; // 경계 구의 반지름

	std::unique_ptr<QuadTreeNode> childs[4]; // 자식노드(Leaf) 노드를 저장하는 배열

	QuadTreeNode* neighbors[4] = { 0 }; //LOD에 필요한 변수, 해당 Node와 이웃되는 Node를 알려주는 배열 
};

class QuadTree
{
private:
	int triangleCount, drawCount;
	ModelVertex* vertices; // vertex 변수, 외부에서 init을 할때 그 class의 vertex vector의 포인터 가져와서 사용하게됨
	std::unique_ptr<QuadTreeNode> root; //최상위 부모 노드, 이 노드 밑으로 자식노드들 4개씩 구성되어있다. 

	int rootSize; //Terrain에서 가져온 heightmap의 width
	LOD* lod = nullptr; //LOD 함수 호출하는 객체 

public:
	void Initialize(ModelVertex* vertices, int cx, int cy); //  이미지의 w(cx),h(cy)을 가져와서 QuadTreeNode 를 초기화 한다.
	void ProcessFrustumCulling(class Frustum* frustum); // private의 FrustumCulling 함수를 호출하는 역할 
	std::vector<DWORD> GenerateIndices(); //인덱스를 구성하여 인덱스 벡터를 반환 , private의 MakeUpIndice를 호출 

	void SetLOD(LOD* lod); // Lod가 적용되는 mesh에 LOD를 Set해주는함수 ( 해당 코드는 Terrain에만 적용됨)

private:
	void Build(QuadTreeNode* node, int topleft, int topright, int bottomleft, int bottomright); // 부모Node 및 자식Node생성 후 data(좌표,경계 구 등등) 초기화 (Init때 첫 호출) 
	void FrustumCulling(QuadTreeNode* node, class Frustum* frustum);//4구역이 전부 클리핑이 되거나 아니면 노드의 이미지 사이즈가 1이하인 경우 함수가 반환하게 되어있음 
	void SetIndices(QuadTreeNode* node, std::vector<DWORD>& indeices);//vertex를 재활용하기 위한 index Set (그리는 순서)
	void MakeUpIndices(QuadTreeNode* node, std::vector<DWORD>& indices);//index들을 만들기 위한 함수 (SetIndeces를 호출)

	void MakeUpIndicesToLOD(QuadTreeNode* node, std::vector<DWORD>& indices); // LOD 변수가 존재하는 경우, Index 생성
	void BuildNeighbor(QuadTreeNode* node); //해당 노드(매개변수)의 이웃노드를 만들어주는 함수 
	
	//이웃 Corner 좌표들을 초기화 후 이웃 노드 탐색
	QuadTreeNode* FindNeighborNode(Neighbor pos, int topLeft, int topRight, int bottomLeft, int bottomRight); // 자식노드들의 이웃노드들을 만드는 함수 
	
	//Corner 좌표들을 토대로 자식노드들을 탐색하는 코드 
	QuadTreeNode* FindNode(QuadTreeNode* node, POINT center, const int& topLeft, const int& topRight, const int& bottomLeft, const int&
		bottomRight);
};


