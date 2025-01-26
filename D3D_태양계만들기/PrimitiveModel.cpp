#include "PrimitiveModel.h"
void PrimitiveCube::MakePrimitive(const std::string& texFilename)
{
	std::vector<Texture> textures;
	if (!texFilename.empty()) textures.push_back(Texture(device, texFilename)); //텍스쳐 이름이 존재하면 텍스쳐를 벡터에 삽입
	else textures.push_back(Texture(device, Colors::UnloadedTextureColor));// 이름이 존재하지 않으면 텍스쳐 로드 실패시의 색상 텍스쳐를 삽입
	std::vector<ModelVertex> vertices =
	{
		//// vertex 좌표 (x,y,z) UV좌표계(U,V) 법선벡터(nx,ny,nz)
		ModelVertex(-0.5f, 0.5f, 0.5f,	 0.0f, 0.0f, -0.5f,	 0.5f, 0.5f), // 0 top far left
		ModelVertex(0.5f, 0.5f, 0.5f,	 1.0f, 0.0f, 0.5f,	 0.5f, 0.5f), // 1 top far right
		ModelVertex(0.5f, 0.5f, -0.5f,	 1.0f, 1.0f, 0.5f,	 0.5f, -0.5f), // 2 top near right
		ModelVertex(-0.5f, 0.5f, -0.5f,	 0.0f, 1.0f, -0.5f,	 0.5f, -0.5f), // 3 top near left

		ModelVertex(-0.5f, -0.5f, 0.5f,	 0.0f, 1.0f, -0.5f,	 -0.5f, 0.5f), // 4 bottom far left
		ModelVertex(0.5f, -0.5f, 0.5f,	 1.0f, 1.0f, 0.5f,	 -0.5f, 0.5f), // 5 bottom far right
		ModelVertex(0.5f, -0.5f, -0.5f,	 1.0f, 0.0f, 0.5f,	 -0.5f, -0.5f), // 6 bottom near right
		ModelVertex(-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -0.5f,	 -0.5f, -0.5f), // 7 bottom near left

		ModelVertex(0.5f, -0.5f, 0.5f,	 0.0f, 0.0f, 0.5f,	 -0.5f, 0.5f), // 8 other bottom far right
		ModelVertex(0.5f, -0.5f, -0.5f,	 0.0f, 1.0f, 0.5f,	 -0.5f, -0.5f), // 9 other bottom near right
		ModelVertex(-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, -0.5f,	 -0.5f, -0.5f), // 10 other bottom near left
		ModelVertex(-0.5f, -0.5f, 0.5f,	 1.0f, 0.0f, -0.5f,	 -0.5f, 0.5f), // 11 other bottom far left

		// vertex 좌표 (x,y,z) UV좌표계(U,V) 법선벡터(nx,ny,nz)
		//ModelVertex(-0.5f+1.5f	, 0.5f, 0.5f + 1.5f,	 0.0f, 0.0f, -0.5f,	 0.5f, 0.5f), // 12 top far left
		//ModelVertex(0.5f + 1.5f	, 0.5f, 0.5f + 1.5f,	 1.0f, 0.0f, 0.5f,	 0.5f, 0.5f), // 13 top far right
		//ModelVertex(0.5f + 1.5f	, 0.5f, -0.5f + 1.5f,	 1.0f, 1.0f, 0.5f,	 0.5f, -0.5f), // 14 top near right
		//ModelVertex(-0.5f + 1.5f	, 0.5f, -0.5f + 1.5f,	 0.0f, 1.0f, -0.5f,	 0.5f, -0.5f), // 15 top near left

		//ModelVertex(-0.5f + 1.5f		,	 -0.5f, 0.5f + 1.5f		,	 0.0f, 1.0f, -0.5f,	 -0.5f, 0.5f), // 16 bottom far left
		//ModelVertex(0.5f + 1.5f		,	-0.5f, 0.5f + 1.5f		,	 1.0f, 1.0f, 0.5f,	 -0.5f, 0.5f), // 17 bottom far right
		//ModelVertex(0.5f + 1.5f		,	-0.5f, -0.5f + 1.5f		,	 1.0f, 0.0f, 0.5f,	 -0.5f, -0.5f), // 18 bottom near right
		//ModelVertex(-0.5f + 1.5f	,	-0.5f, -0.5f + 1.5f	, 0.0f, 0.0f, -0.5f,	 -0.5f, -0.5f), // 19 bottom near left

		//ModelVertex(0.5f + 1.5f		, -0.5f, 0.5f + 1.5f		,	 0.0f, 0.0f, 0.5f,	 -0.5f, 0.5f), // 20 other bottom far right
		//ModelVertex(0.5f + 1.5f		, -0.5f, -0.5f + 1.5f		,	 0.0f, 1.0f, 0.5f,	 -0.5f, -0.5f), // 21 other bottom near right
		//ModelVertex(-0.5 + 1.5f		, -0.5f, -0.5f + 1.5f		, 1.0f, 1.0f, -0.5f,	 -0.5f, -0.5f), // 22 other bottom near left
		//ModelVertex(-0.5 + 1.5f	, -0.5f, 0.5f + 1.5f	,	 1.0f, 0.0f, -0.5f,	 -0.5f, 0.5f), // 23 other bottom far left
	};

	std::vector<DWORD> indices =
	{
		0, 1, 2,	0, 2, 3, // top
		7, 6, 5,	7, 5, 4, // bottom
		3, 2, 6,	3, 6, 7, // front
		1, 0, 4,	1, 4, 5, // back
		2, 1, 8,	2, 8, 9, // right
		0, 3, 10,	0, 10, 11, // left

		//0 + 12, 1 + 12, 2 + 12,	0 + 12, 2 + 12, 3 + 12, // top
		//7 + 12, 6 + 12, 5 + 12,	7 + 12, 5 + 12, 4 + 12, // bottom
		//3 + 12, 2 + 12, 6 + 12,	3 + 12, 6 + 12, 7 + 12, // front
		//1 + 12, 0 + 12, 4 + 12,	1 + 12, 4 + 12, 5 + 12, // back
		//2 + 12, 1 + 12, 8 + 12,	2 + 12, 8 + 12, 9 + 12, // right
		//0 + 12, 3 + 12, 10 + 12,	0 + 12, 10 + 12, 11 + 12, // left
	};

	//부모클래스,순수가상함,Model => std::unique_ptr<Mesh> mesh;
	//mesh 생성자 : 디바이스, 디바이스구조체(context), vertex 벡터 , index 벡터, 텍스쳐 벡터, Transform행렬
	mesh = std::make_unique<Mesh>(device, deviceContext, vertices, indices, textures, XMMatrixIdentity());
};

//sphere 코드 출처 : http://www.songho.ca/opengl/gl_sphere.html
void PrimitiveSphere::MakePrimitive(const std::string& texFilename)
{
	std::vector<Texture> textures;
	if (!texFilename.empty()) textures.push_back(Texture(device, texFilename)); //텍스쳐 이름이 존재하면 텍스쳐를 벡터에 삽입
	else textures.push_back(Texture(device, Colors::UnloadedTextureColor));// 이름이 존재하지 않으면 텍스쳐 로드 실패시의 색상 텍스쳐를 삽입

	std::vector<ModelVertex> vertices;
	std::vector<DWORD> indices;

	float sectorStep = XM_2PI / SectorCount;
	float stackStep = XM_PI / StackCount;
	float sectorAngle, stackAngle;

	float x, y, z, xy;
	for (int i = 0; i <= StackCount; ++i)
	{
		stackAngle = XM_PI / 2 - i * stackStep; // radian : (pi/2)에서 (-pi/2)까지
		xy = radius * cosf(stackAngle); // radius * cos(radian);
		z = radius * sinf(stackAngle); // radius * sin(radian);

		for (int j = 0 ; j <= SectorCount; ++j)
		{
			ModelVertex vertex;
			sectorAngle = j * sectorStep; //0부터 2pi까지

			//setup position
			x = xy * cosf(sectorAngle); //radius * cos(radian) * cos(radian) 
			y = xy * sinf(sectorAngle); //radius * cos(radian) * sin(radian)
			vertex.pos = { x,y,z };

			//setup normal
			XMStoreFloat3(&vertex.normal, XMVector3Normalize(XMVectorSet(x, y, z, 0.0f)));

			//setup tex coord (u,v)
			vertex.texCoord = { (float)j / SectorCount, (float)i / StackCount };

			vertices.push_back(vertex);
		}
	}

	int k1, k2;
	for (int i = 0; i < StackCount; ++i)
	{
		k1 = i * (SectorCount + 1);
		k2 = k1 + SectorCount + 1;

		for (int j = 0; j < SectorCount; ++j,++k1,++k2)
		{
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if (i != (StackCount - 1))
			{
				indices.push_back(k1+1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	mesh = std::make_unique<Mesh>(device, deviceContext, vertices, indices, textures, XMMatrixIdentity());
}