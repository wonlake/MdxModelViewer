#ifndef __MDX_HEADER_H__
#define __MDX_HEADER_H__

#include <stdio.h>
#include <vector>
#include <string>
#include "ShareStruct.h"
#include <StormLib/StormLib.h>

class CMdxModel;

class CMDXFile
{
private:
	CMdxModel*              mModel;

	unsigned char*			m_pFileData;
	unsigned int			m_dwFileSize;
	unsigned int			m_CurrentChunkSize;
	unsigned int			m_CurrentDataPos;

	typedef std::vector< tagVERTEX >		VertexList;
	typedef std::vector< VertexList >		VertexGroups;
	typedef std::vector< tagVERTEXNORMAL >	NormalList;
	typedef std::vector< NormalList >		NormalGroups;
	typedef std::vector< tagTEXCOORD >		TexCoordList;
	typedef std::vector< TexCoordList >		TexCoordGroups;
	typedef std::vector< tagFACE >			FaceList;
	typedef std::vector< FaceList >			FaceGroups;
	typedef std::vector< unsigned int >     GeosetMaterialIDGroups;
	typedef std::vector< unsigned int >     GroupList;
	typedef std::vector< GroupList >		GroupGroups;
	typedef std::vector< unsigned int >     MatrixList;
	typedef std::vector< MatrixList >		MatrixGroups;
	typedef std::vector< unsigned int >     MatrixIDList;
	typedef std::vector< MatrixIDList >     MatrixIDGroups; 
	typedef std::vector< tagLAYER >			LayerList;
	typedef std::vector< LayerList >		MaterialGroups;
	typedef std::vector< std::string >		TextureGroups;

	GroupGroups             m_GroupGroups;
	MatrixGroups            m_MatrixGroups;
	MatrixIDGroups          m_MatrixIDGroups;
	std::vector<tagVERTEX>  m_PivotPoints;
	HANDLE					m_mpqWar3;
	
public:
	CMDXFile();
	~CMDXFile();
private:
	void ReadVersion();
	void ReadModel();
	void ReadSequence();
	void ReadGlobalSequence();
	void ReadMaterial();
	void ReadTexture();
	void ReadTextureAnimation();
	void ReadGeoset();
	void ReadGeosetAnimation();
	void ReadBone();
	void ReadLight();
	void ReadHelpher();
	void ReadAttachment();
	void ReadPivotPoint();
	void ReadParticleEmitter();
	void ReadParticleEmitter2();
	void ReadRibbonEmitter();
	void ReadEventObject();
	void ReadCamera();
	void ReadCollisionShape();
	void SortMatrix();
	void ParseData();
public:
	bool ParseMdxFile( const char* lpFileName, CMdxModel* model );
	bool ParseMdxFileFromResource( const char* lpFileName, CMdxModel* model );
	bool ParseTexture( IMAGEINFO* pImageInfo, const char* lpFileName );
	bool IsExist( const char* filename );
};

#endif