#pragma once

#include "ShareStruct.h"

class CMdxModel
{
	friend class CMDXFile;
public:
	CMdxModel(void);
	~CMdxModel(void);
private:
	TIME				mTime;
	float				mMin[3];
	float				mMax[3];
	unsigned int		mCurFrame;
	unsigned int		mNumMatrix;
	unsigned int		mTotalFrames;
	std::string			mModelName;
	Ogre::Entity*		mEntity;
	Ogre::SceneManager* mSceneMgr;
	Ogre::MeshPtr		mMeshPtr;

	typedef std::vector<unsigned int>		boneIndex;
	typedef std::vector<boneIndex>			groupIndex;
	typedef std::vector<tagVERTEX>			groupVertex;
	typedef std::vector<tagVERTEXNORMAL>	groupNormal;
	typedef std::vector<tagTEXCOORD>		groupTexCoord;
	typedef std::vector<tagFACE>			groupFace;
	typedef std::vector<tagLAYER >			LayerList;
	typedef unsigned int					MaterialID;

	std::vector<MaterialID>				mMaterial;
	std::vector<groupTexCoord>			mTexCoord;
	std::vector<groupFace>				mFace;
	std::vector<groupVertex>			mVertex;
	std::vector<groupNormal>			mNormal;
	std::vector<groupIndex>				mMatrixGroups;
	std::vector<tagBONEMATRIX>			mMatrix;
	std::vector<unsigned int>			mMatrixOrder;
	std::vector<tagGLOBALSEQ>			mGlobalSequence;
	std::vector<tagANIMATION>			mAnimation;
	std::vector<LayerList>				mMaterialGroups;
	std::vector<std::string>			mTextureGroups;
	std::vector<tagGEOSETANIMATION>		mGeosetAnimation;
	std::vector<tagTEXTUREANIMATION>	mTextureAnimation;

	std::vector< unsigned int >         mSubMeshGroupID;
	std::vector< std::string >          mSubMeshMaterialName;
	std::vector< unsigned int >         mTextureAnimationID;
private:
	static unsigned int reference;
	static CMDXFile*	mMdxFile;
	static bool         mTransparent;
public:
	void SetSceneManager( const Ogre::SceneManager* sceneManager );
	bool CreateEntityFromFile( const char* filename );
	void Update( const Ogre::FrameEvent& evt );
	void SetupTextureAnimation();
	void SetAnimationByName( const char *lpszActionName);
	void SetAnimationByID( unsigned int id);
	Ogre::Entity* GetEntity();
	unsigned int GetNumAnimations();
	void Clear();
	bool IsExist( const char* filename );

private:
	void CreateMaterial();
	const char* CreateMesh( const char* fileName );	
	unsigned int GetMatrixIndexByID( unsigned int id );
	void SetupGeosetAnimation();
	void SetupBonesMatrix();
	void TransformVertex( float* pVertex, 			
						  const unsigned int bytes,
						  const int group,
						  const int index );
};