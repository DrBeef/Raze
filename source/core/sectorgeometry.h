#pragma once

#include "tarray.h"
#include "vectors.h"
#include "build.h"

struct Section;

struct SectorGeometryPlane
{
	TArray<FVector3> vertices;
	TArray<FVector2> texcoords;
	FVector3 normal{};
};

enum GeomFlags
{
	NoEarcut = 1,
	NoLibtess = 2,
	FloorDone = 4,
	CeilingDone = 8,
};

using SectionGeometryPlane = SectorGeometryPlane;

struct SectionGeometryData
{
	SectorGeometryPlane planes[2];
	TArray<FVector2> meshVertices;	// flat vertices. Stored separately so that plane changes won't require completely new triangulation.
	TArray<int> meshIndices;
	sectortype compare[2] = {};
	vec2_t poscompare[2] = {};
	vec2_t poscompare2[2] = {};
	FVector3 normal[2];
};

class SectionGeometry
{
	TArray<SectionGeometryData> data;

	bool ValidateSection(Section* section, int plane);
	void MarkDirty(sectortype* sector);
	bool CreateMesh(Section* section);
	void CreatePlaneMesh(Section* section, int plane, const FVector2& offset);


public:
	SectionGeometryPlane* get(Section* section, int plane, const FVector2& offset, TArray<int>** pIndices);

	void SetSize(unsigned sectcount)
	{
		data.Clear(); // delete old content
		data.Resize(sectcount);
	}
};

extern SectionGeometry sectionGeometry;

				