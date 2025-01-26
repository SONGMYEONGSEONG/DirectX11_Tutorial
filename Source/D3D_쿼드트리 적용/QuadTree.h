#pragma once
#include <vector>
#include "Model.h"
#include "Frustum.h"
enum Corner { TL =0, TR,BL,BR };

struct QuadTreeNode // ����Ʈ������ ����� ��� ����ü�� �����.
{
	int size; //��忡�� �������ִ� �̹����� ũ��(TR - TL)

	int center; // �̹�� 4��� �������� �߽���
	int corners[4]; // �� Corner�� ��ǥ�� �迭�� ������ ����
	//corners[Corner::TL]�� corners[0]�� ���� �ǹ� 

	bool isCulled; // �ø� ����
	float radius; // ��� ���� ������

	std::unique_ptr<QuadTreeNode> childs[4]; // �ڽĳ��(Leaf) ��带 �����ϴ� �迭
};

class QuadTree
{
private:
	int triangleCount, drawCount;
	ModelVertex* vertices; // vertex ����, �ܺο��� init�� �Ҷ� �� class�� vertex vector�� ������ �����ͼ� ����ϰԵ�
	std::unique_ptr<QuadTreeNode> root; //�ֻ��� �θ� ���, �� ��� ������ �ڽĳ��� 4���� �����Ǿ��ִ�. 

public:
	void Initialize(ModelVertex* vertices, int cx, int cy); //  �̹����� w(cx),h(cy)�� �����ͼ� QuadTreeNode �� �ʱ�ȭ �Ѵ�.
	void ProcessFrustumCulling(class Frustum* frustum); // private�� FrustumCulling �Լ��� ȣ���ϴ� ���� 
	std::vector<DWORD> GenerateIndices(); //�ε����� �����Ͽ� �ε��� ���͸� ��ȯ , private�� MakeUpIndice�� ȣ�� 

private:
	void Build(QuadTreeNode* node, int topleft, int topright, int bottomleft, int bottomright);
	void FrustumCulling(QuadTreeNode* node, class Frustum* frustum);//4������ ���� Ŭ������ �ǰų� �ƴϸ� ����� �̹��� ����� 1������ ��� �Լ��� ��ȯ�ϰ� �Ǿ����� 
	void SetIndices(QuadTreeNode* node, std::vector<DWORD>& indeices);//vertex�� ��Ȱ���ϱ� ���� index Set (�׸��� ����)
	void MakeUpIndices(QuadTreeNode* node, std::vector<DWORD>& indices);//index���� ����� ���� �Լ� (SetIndeces�� ȣ��)
};


