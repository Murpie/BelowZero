#pragma once
#include "MeshStructs.h"
#include <vector>
#include <string>
#include <fstream>

using namespace std;

class LeapMesh
{
private:
	void loader(const char* fileName);

public:
	LeapMesh();
	LeapMesh(const char* fileName);
	~LeapMesh();

	int getVertexCount();

	VertexInformation* vertices;
	MeshInfo* transform;
	MaterialInformation* material;
	//BoundingBox* BBox;
	vector <BoundingBox*> boundingBoxes;
	//BoundingBoxVertex* BBoxVertices;
	JointInformation* joints;
	AnimationInformation* animation;
	KeyFrame* keyFrame;
	KeyFrameData* keyFrameData;
	BlendShape* blendShape;
	BlendShapeVertex* blendShapeVertices;
	Group* group;
	CustomMayaAttributes* customMayaAttribute;
	Counter counterReader;
};