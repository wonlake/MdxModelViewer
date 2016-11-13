#include "MdxModel.h"
#include "MathHelper.h"
#include "MdxHeader.h"

using namespace Ogre;
unsigned int CMdxModel::reference = 0;
CMDXFile* CMdxModel::mMdxFile	  = NULL;
bool CMdxModel::mTransparent = false;
CMdxModel::CMdxModel(void)
{
	mTotalFrames = 0xFFFFFFFF;
	mSceneMgr = NULL;
	mEntity = NULL;
	reference++;
	mTime.start = 0;
	mTime.end = 0;
	mTime.time = 0;
	mCurFrame = 0;
	if( reference == 1 )
	{
		if( mMdxFile == NULL )
			mMdxFile = new CMDXFile();
	}
}

CMdxModel::~CMdxModel(void)
{
	reference--;
	if( reference == 0 )
	{
		if( mMdxFile != NULL )
		{
			delete mMdxFile;
			mMdxFile = NULL;
		}
	}
}

void CMdxModel::SetSceneManager( const Ogre::SceneManager* sceneManager )
{
	if( mSceneMgr == NULL )
		mSceneMgr = const_cast<Ogre::SceneManager*>(sceneManager);
}

bool CMdxModel::CreateEntityFromFile( const char* filename )
{
	if( mSceneMgr == NULL )
		return false;
	const char* meshName = CreateMesh( filename );
	mEntity = mSceneMgr->createEntity( meshName, meshName );
	return true;
}

void CMdxModel::Update( const Ogre::FrameEvent& evt )
{
	mCurFrame += evt.timeSinceLastFrame * 1000;
	if( mTotalFrames == 0xFFFFFFFF )
	{
		mTotalFrames = mAnimation[mAnimation.size() - 1].dwEndTime;
		for( int i = 0; i < mAnimation.size(); i++ )
		{
			if( (mAnimation[i].dwStartTime & 0xF0000000) ||
				(mAnimation[i].dwEndTime & 0xF0000000 ) )
				continue;
			mTime.start = mAnimation[i].dwStartTime;
			mTime.end	= mAnimation[i].dwEndTime;
			break;
		}
		
	}
	if( (mTime.end <= mTotalFrames) && (mTime.start >= 0) )
	{
		if( mCurFrame <= mTime.start )
			mCurFrame = mTime.start;
		if( mCurFrame >= mTime.end )
			mCurFrame = mTime.start;
	}
	else
	{
		if( mCurFrame > mTotalFrames )
			mCurFrame = 0;
	}
	mTime.time = mCurFrame;
	
	int numGlobalSeq = mGlobalSequence.size();
	for( int k = 0; k < numGlobalSeq; k++ )
	{
		mGlobalSequence[k].dwTime += evt.timeSinceLastFrame * 1000;
		if( mGlobalSequence[k].dwTime >= mGlobalSequence[k].dwDuration)
			mGlobalSequence[k].dwTime = 0;
	}

	SetupBonesMatrix();
	SetupGeosetAnimation();
	SetupTextureAnimation();

	Mesh::SubMeshIterator it = mMeshPtr->getSubMeshIterator();
	int num = mMeshPtr->getNumSubMeshes();
	int count = 0;
	while( count < num )
	{
		SubMesh* sm = it.getNext();
		for( int i = 0; i < mGeosetAnimation.size(); i++ )
		{
			if( mGeosetAnimation[i].dwGeosetID == mSubMeshGroupID[count] )
			{
				if( mGeosetAnimation[i].fAlpha < 0.01 )
					mEntity->getSubEntity(count)->setMaterialName( "Transparent" );
				else
				{					
					mEntity->getSubEntity(count)->setMaterialName( mSubMeshMaterialName[count].c_str() );
				}
			}
		}
		
		HardwareVertexBufferSharedPtr vbuf = 
			sm->vertexData->vertexBufferBinding->getBuffer(0);
		float* pReal = static_cast<float*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));

		TransformVertex( pReal, 8, mSubMeshGroupID[count], count );

		vbuf->unlock();
		count++;
	}
}
void CMdxModel::SetAnimationByName( const char *lpszActionName)
{
	int numAnimation = mAnimation.size();
	for (DWORD i = 0; i < numAnimation; i++)
		if (!strcmp(mAnimation[i].szActionName, lpszActionName))
		{
			mTime.start = mAnimation[i].dwStartTime;
			mTime.end = mAnimation[i].dwEndTime;
			break;
		}
}

void CMdxModel::SetAnimationByID( unsigned int id)
{
	if( id >= mAnimation.size() )
		return;
	mTime.start = mAnimation[id].dwStartTime;
	mTime.end = mAnimation[id].dwEndTime;
}

Ogre::Entity* CMdxModel::GetEntity()
{
	return mEntity;
}

unsigned int CMdxModel::GetNumAnimations()
{
	return mAnimation.size();
}

void CMdxModel::CreateMaterial()
{
	size_t numMaterial = mMaterialGroups.size();

	for( size_t i = 0; i < numMaterial; i++ )
	{
		int numAnimPass = 0;
		int numPass = mMaterialGroups[i].size();
		for( int j = 0; j < numPass; j++ )
		{
			if( mMaterialGroups[i][j].TextureAnimationID != 0xFFFFFFFF )
			{
				numAnimPass++;
			}
		}
		
		if( numAnimPass == 0 )
		{
			IMAGEINFO imageInfo;
			char materialName[100];
			sprintf( materialName, "MDX/%s/Material%u", mModelName.c_str(), i );
			Ogre::MaterialPtr material = 
				Ogre::MaterialManager::getSingleton().getByName( materialName );
			if( !material.isNull() )
				continue;
			material = Ogre::MaterialManager::getSingleton().create( materialName, "General", true );
			Ogre::Technique* technique = material->getTechnique(0);

			for( size_t j = 0; j < numPass; j++ )
			{
				Ogre::Pass* pass = NULL;

				if( j == 0 )
					pass = technique->getPass(0);
				else
					pass = technique->createPass();

				switch( mMaterialGroups[i][j].FilterMode )
				{
				case 0:
					{
						pass->setDepthCheckEnabled( true );
						pass->setDepthWriteEnabled( true );
						break;
					}
				case 1:
					{
						pass->setDepthCheckEnabled( true );
						pass->setDepthWriteEnabled( true );
						pass->setAlphaRejectValue( 192 );
						pass->setSceneBlending( 
							SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA );
						break;
					}
				case 2:
					{
						pass->setDepthCheckEnabled( true );
						pass->setDepthWriteEnabled( false );
						pass->setSceneBlending( 
							SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA );
						break;
					}
				case 3:
					{
						pass->setDepthCheckEnabled( true );
						pass->setDepthWriteEnabled( false );
						pass->setSceneBlending( 
							SBF_SOURCE_COLOUR, SBF_ONE );
						break;
					}
				case 4:
					{
						pass->setDepthCheckEnabled( true );
						pass->setDepthWriteEnabled( false );
						pass->setSceneBlending( 
							SBF_SOURCE_ALPHA, SBF_ONE );
						break;
					}
				case 5:
					{
						pass->setDepthCheckEnabled( true );
						pass->setDepthWriteEnabled( false );
						pass->setSceneBlending( 
							SBF_ZERO, SBF_SOURCE_COLOUR );
						break;
					}
				default:
					break;
				}
				pass->setCullingMode(Ogre::CULL_NONE);
				pass->setAmbient( Ogre::ColourValue() );
				pass->setLightingEnabled( true );

				const char* pTextureName = mTextureGroups[mMaterialGroups[i][j].TextureID].c_str();
				
				TexturePtr texturePtr = 
					TextureManager::getSingleton().getByName( pTextureName );
				if( texturePtr.isNull() )
				{
					if( !mMdxFile->ParseTexture( &imageInfo, pTextureName ) )
						continue;
					texturePtr = TextureManager::getSingleton().createManual(
						pTextureName, 
						"General",
						TEX_TYPE_2D,     
						imageInfo.width, imageInfo.height,         // width & height
						0,               
						PF_BYTE_BGRA, 
						TU_DEFAULT );    

					HardwarePixelBufferSharedPtr pixelBuffer = texturePtr->getBuffer();
					pixelBuffer->lock( HardwareBuffer::HBL_NORMAL ); 
					const PixelBox& pixelBox = pixelBuffer->getCurrentLock();
					uint8* pDest = static_cast<uint8*>(pixelBox.data);

					memcpy( pDest, imageInfo.imageData,
						4 * imageInfo.width * imageInfo.height );

					if( imageInfo.imageData != NULL )
					{
						free( imageInfo.imageData );
						imageInfo.imageData = NULL;
					}

					pixelBuffer->unlock();
					texturePtr->load();
				}
				Ogre::TextureUnitState* textureUnitState = pass->createTextureUnitState();
				textureUnitState->setTextureAddressingMode( Ogre::TextureUnitState::TAM_CLAMP );
				textureUnitState->setTextureName( texturePtr->getName() );			
			}
			material->load();
		}
		else
		{
			for( size_t j = 0; j < numPass; j++)
			{
				IMAGEINFO imageInfo;
				char materialName[100];
				sprintf( materialName, "MDX/%s/Material%uPass%u", mModelName.c_str(), i, j );
				
				Ogre::MaterialPtr material = 
					Ogre::MaterialManager::getSingleton().getByName( materialName );
				if( !material.isNull() )
					continue;

				material = Ogre::MaterialManager::getSingleton().create( materialName, "General", true );
				Ogre::Technique* technique = material->getTechnique(0);

				Ogre::Pass* pass = technique->getPass(0);

				switch( mMaterialGroups[i][j].FilterMode )
				{
				case 0:
					{
						pass->setDepthCheckEnabled( true );
						pass->setDepthWriteEnabled( true );
						break;
					}
				case 1:
					{
						pass->setDepthCheckEnabled( true );
						pass->setDepthWriteEnabled( true );
						pass->setAlphaRejectValue( 192 );
						pass->setSceneBlending( 
							SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA );
						break;
					}
				case 2:
					{
						pass->setDepthCheckEnabled( true );
						pass->setDepthWriteEnabled( false );
						pass->setSceneBlending( 
							SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA );
						break;
					}
				case 3:
					{
						pass->setDepthCheckEnabled( true );
						pass->setDepthWriteEnabled( false );
						pass->setSceneBlending( 
							SBF_SOURCE_COLOUR, SBF_ONE );
						break;
					}
				case 4:
					{
						pass->setDepthCheckEnabled( true );
						pass->setDepthWriteEnabled( false );
						pass->setSceneBlending( 
							SBF_SOURCE_ALPHA, SBF_ONE );
						break;
					}
				case 5:
					{
						pass->setDepthCheckEnabled( true );
						pass->setDepthWriteEnabled( false );
						pass->setSceneBlending( 
							SBF_ZERO, SBF_SOURCE_COLOUR );
						break;
					}
				default:
					break;
				}
				pass->setCullingMode(Ogre::CULL_NONE);
				pass->setAmbient( Ogre::ColourValue() );
				pass->setLightingEnabled( true );

				const char* pTextureName = mTextureGroups[mMaterialGroups[i][j].TextureID].c_str();

				TexturePtr texturePtr = 
					TextureManager::getSingleton().getByName( pTextureName );
				if( texturePtr.isNull() )
				{	
					if( !mMdxFile->ParseTexture( &imageInfo, pTextureName ) )
						continue;
					texturePtr = TextureManager::getSingleton().createManual(
						pTextureName, 
						"General",
						TEX_TYPE_2D,     
						imageInfo.width, imageInfo.height,         // width & height
						0,               
						PF_BYTE_BGRA, 
						TU_DEFAULT );    

					HardwarePixelBufferSharedPtr pixelBuffer = texturePtr->getBuffer();
					pixelBuffer->lock( HardwareBuffer::HBL_NORMAL ); 
					const PixelBox& pixelBox = pixelBuffer->getCurrentLock();
					uint8* pDest = static_cast<uint8*>(pixelBox.data);

					memcpy( pDest, imageInfo.imageData,
						4 * imageInfo.width * imageInfo.height );

					if( imageInfo.imageData != NULL )
					{
						free( imageInfo.imageData );
						imageInfo.imageData = NULL;
					}

					pixelBuffer->unlock();
					texturePtr->load();
				}
				Ogre::TextureUnitState* textureUnitState = pass->createTextureUnitState();
				textureUnitState->setTextureAddressingMode( Ogre::TextureUnitState::TAM_CLAMP );
				textureUnitState->setTextureName( texturePtr->getName() );
				
				material->load();
			}
		}
	}

	if( !mTransparent )
	{
		Ogre::MaterialPtr tranparentMaterial = 
			Ogre::MaterialManager::getSingleton().create( "Transparent", "General", true );
		Ogre::Pass* transparentPass = 
			tranparentMaterial->getTechnique(0)->getPass(0);
		transparentPass->setColourWriteEnabled(false);
		transparentPass->setDepthCheckEnabled( false );
		transparentPass->setDepthWriteEnabled( false );
		tranparentMaterial->load();
		mTransparent = true;
	}
}

const char* CMdxModel::CreateMesh( const char* fileName )
{
	if( !mMdxFile->ParseMdxFileFromResource( fileName, this ) )
		MessageBox( NULL, TEXT("文件不存在"), NULL, MB_OK );

	CreateMaterial();
	mMeshPtr = MeshManager::getSingleton().getByName( mModelName );
	bool bExist = true;
	if( mMeshPtr.isNull() )
	{
		bExist = false;
		mMeshPtr = MeshManager::getSingleton().createManual( mModelName, 
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
	}
	mSubMeshGroupID.clear();
	mSubMeshMaterialName.clear();
	mTextureAnimationID.clear();

	for( int i = 0; i < mVertex.size(); i++ )
	{
		size_t numPass = mMaterialGroups[mMaterial[i]].size();
		size_t numSubMesh = 1;
		bool   hasAnim = false;
		for( size_t j = 0; j < numPass; j++ )
		{
			if( mMaterialGroups[mMaterial[i]][j].TextureAnimationID
				!= 0xFFFFFFFF )
			{
				if( j != 0 )
					numSubMesh++;
				hasAnim = true;
			}
		}
		for( size_t it = 0; it < numSubMesh; it++ )
		{
			char materialName[100];
			mSubMeshGroupID.push_back( i );
			if( numSubMesh == 1 && !hasAnim )
			{
				sprintf( materialName, "MDX/%s/Material%u", mModelName.c_str(), mMaterial[i] );
				mTextureAnimationID.push_back( 0xFFFFFFFF );
			}
			else
			{
				sprintf( materialName, "MDX/%s/Material%uPass%u", mModelName.c_str(), mMaterial[i], it );
				mTextureAnimationID.push_back( mMaterialGroups[mMaterial[i]][it].TextureAnimationID );
			}
			mSubMeshMaterialName.push_back(materialName);
			if( bExist )
				continue;
			else
			{				
				SubMesh* sm = mMeshPtr->createSubMesh();
				sm->useSharedVertices = false;
				sm->vertexData = new VertexData();
				sm->vertexData->vertexStart = 0;
				sm->vertexData->vertexCount = mVertex[i].size();
				VertexDeclaration* dcl = sm->vertexData->vertexDeclaration;
				size_t offset = 0;
				dcl->addElement(0, offset, VET_FLOAT3, VES_POSITION);
				offset += VertexElement::getTypeSize(VET_FLOAT3);
				dcl->addElement(0, offset, VET_FLOAT3, VES_NORMAL);
				offset += VertexElement::getTypeSize(VET_FLOAT3);
				dcl->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES);
				offset += VertexElement::getTypeSize(VET_FLOAT2);

				HardwareVertexBufferSharedPtr vbuf = HardwareBufferManager::getSingleton()
					.createVertexBuffer(
					offset, sm->vertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY );
				float* pReal = static_cast<float*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));

				for( int j = 0; j < sm->vertexData->vertexCount; j++ )
				{
					*pReal++ = mVertex[i][j].Position[0];
					*pReal++ = mVertex[i][j].Position[1];
					*pReal++ = mVertex[i][j].Position[2];

					*pReal++ = mNormal[i][j].Normal[0];
					*pReal++ = mNormal[i][j].Normal[1];
					*pReal++ = mNormal[i][j].Normal[2];

					*pReal++ = mTexCoord[i][j].UV[0];
					*pReal++ = mTexCoord[i][j].UV[1];
				}
				vbuf->unlock();
				sm->vertexData->vertexBufferBinding->setBinding( 0, vbuf);
				sm->indexData->indexCount = mFace[i].size() * 3;
				sm->indexData->indexBuffer = HardwareBufferManager::getSingleton()
					.createIndexBuffer(HardwareIndexBuffer::IT_16BIT, sm->indexData->indexCount,
					HardwareBuffer::HBU_STATIC_WRITE_ONLY);
				uint16* pI = static_cast<uint16*>(
					sm->indexData->indexBuffer->lock(HardwareBuffer::HBL_DISCARD));
				for( int j = 0; j < sm->indexData->indexCount / 3; j++ )
				{
					*pI++ = mFace[i][j].VertexIndex[0];
					*pI++ = mFace[i][j].VertexIndex[1];
					*pI++ = mFace[i][j].VertexIndex[2];
				}

				sm->indexData->indexBuffer->unlock();

				sm->setMaterialName( materialName );
			}
		}
	}
	if( !bExist )
	{
		mMeshPtr->_setBounds(AxisAlignedBox( mMin[0], mMin[1], mMin[2], mMax[0], mMax[1], mMax[2]));
		mMeshPtr->_setBoundingSphereRadius(
			std::max( mMax[0] - mMin[0], std::max(mMax[1] - mMin[1], mMax[2] - mMin[2])) / 2.0f );
		mMeshPtr->load();
	}
	return mModelName.c_str();
}

unsigned int CMdxModel::GetMatrixIndexByID( unsigned int id )
{
	for( unsigned int i = 0; i < mNumMatrix; i++ )
	{
		if( mMatrix[i].objectID == id )
			return i;
	}
	return 0xFFFFFFFF;
}

void CMdxModel::SetupTextureAnimation()
{
	//mTime.time = 234307;
	TIME tempTime;
	size_t numTextureAnimation = mTextureAnimation.size();
	for( size_t i = 0; i < numTextureAnimation; i++ )
	{
		Ogre::Vector4 translateVector = Ogre::Vector4(0.0f,0.0f,0.0f,1.0f);
		Ogre::Quaternion rotVector = Ogre::Quaternion::IDENTITY;
		Ogre::Vector4 scaleVector = Ogre::Vector4(1.0f,1.0f,1.0f,1.0f);

		tagTEXTUREANIMATION* pTextureAnimation = &mTextureAnimation[i];
		size_t numTranslation = pTextureAnimation->translation.size();
		if( numTranslation > 0 )
		{
			size_t i1 = 0xFFFFFFFF;
			size_t i2 = 0xFFFFFFFF;

			if( pTextureAnimation->translationGlobalSeqID != 0xFFFFFFFF )
			{
				tempTime.start = 0;
				tempTime.end = mGlobalSequence[pTextureAnimation->translationGlobalSeqID ].dwDuration;
				tempTime.time = mGlobalSequence[pTextureAnimation->translationGlobalSeqID ].dwTime;
			}
			else
			{
				tempTime.start = mTime.start;
				tempTime.end = mTime.end;
				tempTime.time = mTime.time;
			}
			DWORD time = tempTime.time;

			for ( size_t i = 0; i < (numTranslation - 1); i++ )
			{
				if ( time >= pTextureAnimation->translation[i].dwTime && 
					time < pTextureAnimation->translation[i + 1].dwTime)
				{
					i1 = i;
					i2 = i + 1;
					break;
				}
			}

			if( i1 == 0xFFFFFFFF || i2 == 0xFFFFFFFF )
			{
				if ( (time < pTextureAnimation->translation[0].dwTime) && 
					(tempTime.end >= pTextureAnimation->translation[0].dwTime) ) //取首帧的值
				{
					translateVector.x = pTextureAnimation->translation[0].fTrans[0];
					translateVector.y = pTextureAnimation->translation[0].fTrans[1];
					translateVector.z = pTextureAnimation->translation[0].fTrans[2];
				}	
				else if ((time >= pTextureAnimation->translation[numTranslation - 1].dwTime) &&
					(tempTime.start <= pTextureAnimation->translation[numTranslation - 1].dwTime) ) //取尾帧的值
				{
					translateVector.x = pTextureAnimation->translation[numTranslation - 1].fTrans[0];
					translateVector.y = pTextureAnimation->translation[numTranslation - 1].fTrans[1];
					translateVector.z = pTextureAnimation->translation[numTranslation - 1].fTrans[2];
				}
			}
			else
			{
				bool p1 = false;
				bool p2 = false;
				if( tempTime.start > pTextureAnimation->translation[i1].dwTime)
					p1 = true;
				if( tempTime.end < pTextureAnimation->translation[i2].dwTime)
					p2 = true;
				if( p1 && p2 )
				{}
				else if( p1 && (!p2) )
				{	
					translateVector.x = pTextureAnimation->translation[i2].fTrans[0];
					translateVector.y = pTextureAnimation->translation[i2].fTrans[1];
					translateVector.z = pTextureAnimation->translation[i2].fTrans[2];
				}
				else if( (!p1) && p2 )
				{
					translateVector.x = pTextureAnimation->translation[i1].fTrans[0];
					translateVector.y = pTextureAnimation->translation[i1].fTrans[1];
					translateVector.z = pTextureAnimation->translation[i1].fTrans[2];
				}
				else
				{
					MathHelper::GetInterpolatedValue( translateVector, time, 
						pTextureAnimation->translationType,
						&pTextureAnimation->translation[i1], 
						&pTextureAnimation->translation[i2]);
				}
			}
		}

		size_t numRotation= pTextureAnimation->rotation.size();
		if( numRotation > 0 )
		{
			size_t i1 = 0xFFFFFFFF;
			size_t i2 = 0xFFFFFFFF;

			if( pTextureAnimation->rotationGlobalSeqID != 0xFFFFFFFF )
			{
				tempTime.start = 0;
				tempTime.end = mGlobalSequence[pTextureAnimation->rotationGlobalSeqID].dwDuration;
				tempTime.time = mGlobalSequence[pTextureAnimation->rotationGlobalSeqID].dwTime;
			}
			else
			{
				tempTime.start = mTime.start;
				tempTime.end = mTime.end;
				tempTime.time = mTime.time;
			}
			DWORD time = tempTime.time;

			for ( size_t i = 0; i < (numRotation - 1); i++)
			{
				if( time >= pTextureAnimation->rotation[i].dwTime && 
					time < pTextureAnimation->rotation[i + 1].dwTime)
				{
					i1 = i;
					i2 = i + 1;
					break;
				}
			}
			if ( i1 == 0xFFFFFFFF || i2 == 0xFFFFFFFF )        //无旋转帧
			{
				// 设置为首帧
				if ( (time < pTextureAnimation->rotation[0].dwTime) && 
					(tempTime.end >= pTextureAnimation->rotation[0].dwTime) ) //取首帧的值
				{
					rotVector.x = pTextureAnimation->rotation[0].fRot[0];
					rotVector.y = pTextureAnimation->rotation[0].fRot[1];
					rotVector.z = pTextureAnimation->rotation[0].fRot[2];
					rotVector.w = pTextureAnimation->rotation[0].fRot[3];
				}

				// 设置为尾帧
				else if ((time >= pTextureAnimation->rotation[numRotation - 1].dwTime) &&
					(tempTime.start <= pTextureAnimation->rotation[numRotation - 1].dwTime)) //取尾帧的值
				{
					rotVector.x = pTextureAnimation->rotation[numRotation - 1].fRot[0];
					rotVector.y = pTextureAnimation->rotation[numRotation - 1].fRot[1];
					rotVector.z = pTextureAnimation->rotation[numRotation - 1].fRot[2];
					rotVector.w = pTextureAnimation->rotation[numRotation - 1].fRot[3];
				}
			}
			else
			{
				bool p1 = false;
				bool p2 = false;
				if( tempTime.start > pTextureAnimation->rotation[i1].dwTime)
					p1 = true;
				if( tempTime.end < pTextureAnimation->rotation[i2].dwTime)
					p2 = true;
				if( p1 && p2 )	
				{}
				else if( p1 && (!p2))
				{	
					rotVector.x = pTextureAnimation->rotation[i2].fRot[0];
					rotVector.y = pTextureAnimation->rotation[i2].fRot[1];
					rotVector.z = pTextureAnimation->rotation[i2].fRot[2];
					rotVector.w = pTextureAnimation->rotation[i2].fRot[3];
				}
				else if((!p1)&&p2)
				{
					rotVector.x = pTextureAnimation->rotation[i1].fRot[0];
					rotVector.y = pTextureAnimation->rotation[i1].fRot[1];
					rotVector.z = pTextureAnimation->rotation[i1].fRot[2];
					rotVector.w = pTextureAnimation->rotation[i2].fRot[3];
				}
				else
				{
					MathHelper::GetQuaternionValue( rotVector, time,
						pTextureAnimation->rotationType,
						&pTextureAnimation->rotation[i1],&pTextureAnimation->rotation[i2]);
				}
			}
		}

		size_t numScaling = pTextureAnimation->scaling.size();
		if( numScaling > 0)
		{
			size_t i1 = 0xFFFFFFFF;
			size_t i2 = 0xFFFFFFFF;

			if( pTextureAnimation->scalingGlobalSeqID != 0xFFFFFFFF )
			{
				tempTime.start = 0;
				tempTime.end = mGlobalSequence[pTextureAnimation->scalingGlobalSeqID].dwDuration;
				tempTime.time = mGlobalSequence[pTextureAnimation->scalingGlobalSeqID].dwTime;
			}
			else
			{
				tempTime.start = mTime.start;
				tempTime.end   = mTime.end;
				tempTime.time  = mTime.time;
			}
			DWORD time = tempTime.time;

			for ( size_t i = 0; i < (numScaling - 1); i++)
			{
				if( time >= pTextureAnimation->scaling[i].dwTime && 
					time < pTextureAnimation->scaling[i + 1].dwTime )
				{
					i1 = i;
					i2 = i + 1;
					break;
				}
			}

			if( i1 == 0xFFFFFFFF || i2 == 0xFFFFFFFF )
			{
				if ( (time < pTextureAnimation->scaling[0].dwTime) &&
					(tempTime.end >= pTextureAnimation->scaling[0].dwTime) ) //取首帧的值
				{
					scaleVector.x = pTextureAnimation->scaling[0].fScale[0];
					scaleVector.y = pTextureAnimation->scaling[0].fScale[1];
					scaleVector.z = pTextureAnimation->scaling[0].fScale[2];
				}	
				else if( (time >= pTextureAnimation->scaling[numScaling - 1].dwTime) &&
					(tempTime.start<=pTextureAnimation->scaling[numScaling-1].dwTime) ) //取尾帧的值
				{
					scaleVector.x = pTextureAnimation->scaling[numScaling - 1].fScale[0];
					scaleVector.y = pTextureAnimation->scaling[numScaling - 1].fScale[1];
					scaleVector.z = pTextureAnimation->scaling[numScaling - 1].fScale[2];
				}
			}
			else
			{
				bool p1 = false;
				bool p2 = false;
				if( tempTime.start > pTextureAnimation->scaling[i1].dwTime)
					p1 = true;
				if( tempTime.end < pTextureAnimation->scaling[i2].dwTime)
					p2= true;
				if( p1 && p2 )
				{}
				else if( p1 && (!p2) )
				{	
					scaleVector.x = pTextureAnimation->scaling[i2].fScale[0];
					scaleVector.y = pTextureAnimation->scaling[i2].fScale[1];
					scaleVector.z = pTextureAnimation->scaling[i2].fScale[2];
				}
				else if((!p1)&&p2)
				{
					scaleVector.x = pTextureAnimation->scaling[i1].fScale[0];
					scaleVector.y = pTextureAnimation->scaling[i1].fScale[1];
					scaleVector.z = pTextureAnimation->scaling[i1].fScale[2];
				}
				else
				{
					MathHelper::GetInterpolatedValue( scaleVector, time, pTextureAnimation->scalingType,
						&pTextureAnimation->scaling[i1],&pTextureAnimation->scaling[i2]);
				}
			}
		}
		pTextureAnimation->fRot[0]		= rotVector[0];
		pTextureAnimation->fRot[1]		= rotVector[1];
		pTextureAnimation->fRot[2]		= rotVector[2];
		pTextureAnimation->fRot[3]		= rotVector[3];
		pTextureAnimation->fTrans[0]	= translateVector[0];
		pTextureAnimation->fTrans[1]	= translateVector[1];
		pTextureAnimation->fTrans[2]	= translateVector[2];
		pTextureAnimation->fScaling[0]	= scaleVector[0];
		pTextureAnimation->fScaling[1]	= scaleVector[1];
		pTextureAnimation->fScaling[2]	= scaleVector[2];
	}
}
void CMdxModel::SetupGeosetAnimation()
{
	TIME tempTime;
	int numGeosetAnimation = mGeosetAnimation.size();
	for( size_t index = 0; index < numGeosetAnimation; index++ )
	{
		tagGEOSETANIMATION* it = &mGeosetAnimation[index];
		Ogre::Vector4 tmpVector = Ogre::Vector4(1.0f,0.0f,0.0f,1.0f);
		size_t numAlphas = it->alpha.size();
		
		if( numAlphas > 0 )
		{
			size_t i1 = 0xFFFFFFFF;
			size_t i2 = 0xFFFFFFFF;
			if( it->alphaGlobalSeqID != 0xFFFFFFFF )
			{
				tempTime.start = 0;
				tempTime.end = mGlobalSequence[it->alphaGlobalSeqID].dwDuration;
				tempTime.time = mGlobalSequence[it->alphaGlobalSeqID].dwTime;
			}
			else
			{
				tempTime.start = mTime.start;
				tempTime.end = mTime.end;
				tempTime.time = mTime.time;
			}
			unsigned int  time = tempTime.time;
			for ( int i = 0; i < (numAlphas - 1); i++ )
			{
				if ( time >= it->alpha[i].time && 
					 time < it->alpha[i+1].time )
				{
					i1 = i;
					i2 = i + 1;
					break;
				}
			}

			if( i1 == 0xFFFFFFFF || i2 == 0xFFFFFFFF )
			{
				if ( (time < it->alpha[0].time) &&
					 (tempTime.end >= it->alpha[0].time) )					//取首帧的值
				{
					tmpVector.x = it->alpha[0].alpha;
				}	
				else if ( (time >= it->alpha[numAlphas - 1].time) &&
						  (tempTime.start <= it->alpha[numAlphas-1].time) )		//取尾帧的值
				{
					tmpVector.x = it->alpha[numAlphas-1].alpha;
				}
			}
			else
			{
				bool p1 = false;
				bool p2 = false;
				if( tempTime.start > it->alpha[i1].time )
					p1 = true;
				if( tempTime.end < it->alpha[i2].time )
					p2 = true;
				if( p1 && p2 )	
				{}
				else if( p1 && (!p2) )
					tmpVector.x = it->alpha[i2].alpha;
				else if( (!p1) && p2 )
					tmpVector.x = it->alpha[i1].alpha;
				else
				{
					tagTRANSLATION p1, p2;
					ZeroMemory( &p1, sizeof(tagTRANSLATION) );
					ZeroMemory( &p2, sizeof(tagTRANSLATION) );
					p1.fTrans[0] = it->alpha[i1].alpha;
					p1.fInTan[0] = it->alpha[i1].inTan;
					p1.fOutTan[0] = it->alpha[i1].outTan;
					p2.fTrans[0] = it->alpha[i2].alpha;
					p2.fInTan[0] = it->alpha[i2].inTan;
					p2.fOutTan[0] = it->alpha[i2].outTan;
					MathHelper::GetInterpolatedValue( tmpVector, time,
							it->alphaType, &p1, &p2 );
				}
			}
		}
		it->fAlpha = tmpVector.x;
	}
}

void CMdxModel::SetupBonesMatrix()
{
	TIME tempTime;
	size_t numMatrix = mMatrix.size();
	for( size_t index = 0; index < numMatrix; index++ )
	{
		Ogre::Vector4 translateVector = Ogre::Vector4(0.0f,0.0f,0.0f,1.0f);
		Ogre::Quaternion rotVector = Ogre::Quaternion::IDENTITY;
		Ogre::Vector4 scaleVector = Ogre::Vector4(1.0f,1.0f,1.0f,1.0f);

		tagBONEMATRIX* pElement = &mMatrix[mMatrixOrder[index]];

		size_t numTranslation = (int)pElement->translation.size();
		if( numTranslation > 0 )
		{
			size_t i1 = 0xFFFFFFFF;
			size_t i2 = 0xFFFFFFFF;

			if( pElement->translationGlobalSeqID != 0xFFFFFFFF )
			{
				tempTime.start = 0;
				tempTime.end = mGlobalSequence[pElement->translationGlobalSeqID ].dwDuration;
				tempTime.time = mGlobalSequence[pElement->translationGlobalSeqID ].dwTime;
			}
			else
			{
				tempTime.start = mTime.start;
				tempTime.end = mTime.end;
				tempTime.time = mTime.time;
			}
			DWORD time = tempTime.time;

			for ( size_t i = 0; i < (numTranslation - 1); i++ )
			{
				if ( time >= pElement->translation[i].dwTime && 
					time < pElement->translation[i + 1].dwTime)
				{
					i1 = i;
					i2 = i + 1;
					break;
				}
			}

			if( i1 == 0xFFFFFFFF || i2 == 0xFFFFFFFF )
			{
				if ( (time < pElement->translation[0].dwTime) && 
					(tempTime.end >= pElement->translation[0].dwTime) ) //取首帧的值
				{
					translateVector.x = pElement->translation[0].fTrans[0];
					translateVector.y = pElement->translation[0].fTrans[1];
					translateVector.z = pElement->translation[0].fTrans[2];
				}	
				else if ((time >= pElement->translation[numTranslation - 1].dwTime) &&
					(tempTime.start <= pElement->translation[numTranslation - 1].dwTime) ) //取尾帧的值
				{
					translateVector.x = pElement->translation[numTranslation - 1].fTrans[0];
					translateVector.y = pElement->translation[numTranslation - 1].fTrans[1];
					translateVector.z = pElement->translation[numTranslation - 1].fTrans[2];
				}
			}
			else
			{
				bool p1 = false;
				bool p2 = false;
				if( tempTime.start > pElement->translation[i1].dwTime)
					p1 = true;
				if( tempTime.end < pElement->translation[i2].dwTime)
					p2 = true;
				if( p1 && p2 )
				{}
				else if( p1 && (!p2) )
				{	
					translateVector.x = pElement->translation[i2].fTrans[0];
					translateVector.y = pElement->translation[i2].fTrans[1];
					translateVector.z = pElement->translation[i2].fTrans[2];
				}
				else if( (!p1) && p2 )
				{
					translateVector.x = pElement->translation[i1].fTrans[0];
					translateVector.y = pElement->translation[i1].fTrans[1];
					translateVector.z = pElement->translation[i1].fTrans[2];
				}
				else
				{
					MathHelper::GetInterpolatedValue( translateVector, time, 
						pElement->translationType,
						&pElement->translation[i1], &pElement->translation[i2]);
				}
			}
		}

		size_t numRotation= pElement->rotation.size();
		if( numRotation > 0 )
		{
			size_t i1 = 0xFFFFFFFF;
			size_t i2 = 0xFFFFFFFF;

			if( pElement->rotationGlobalSeqID != 0xFFFFFFFF )
			{
				tempTime.start = 0;
				tempTime.end = mGlobalSequence[pElement->rotationGlobalSeqID].dwDuration;
				tempTime.time = mGlobalSequence[pElement->rotationGlobalSeqID].dwTime;
			}
			else
			{
				tempTime.start = mTime.start;
				tempTime.end = mTime.end;
				tempTime.time = mTime.time;
			}
			DWORD time = tempTime.time;

			for ( size_t i = 0; i < (numRotation - 1); i++)
			{
				if( time >= pElement->rotation[i].dwTime && 
					time < pElement->rotation[i + 1].dwTime)
				{
					i1 = i;
					i2 = i + 1;
					break;
				}
			}
			if ( i1 == 0xFFFFFFFF || i2 == 0xFFFFFFFF )        //无旋转帧
			{
				// 设置为首帧
				if ( (time < pElement->rotation[0].dwTime) && 
					(tempTime.end >= pElement->rotation[0].dwTime) ) //取首帧的值
				{
					rotVector.x = pElement->rotation[0].fRot[0];
					rotVector.y = pElement->rotation[0].fRot[1];
					rotVector.z = pElement->rotation[0].fRot[2];
					rotVector.w = pElement->rotation[0].fRot[3];
				}

				// 设置为尾帧
				else if ((time >= pElement->rotation[numRotation - 1].dwTime) &&
					(tempTime.start <= pElement->rotation[numRotation - 1].dwTime)) //取尾帧的值
				{
					rotVector.x = pElement->rotation[numRotation - 1].fRot[0];
					rotVector.y = pElement->rotation[numRotation - 1].fRot[1];
					rotVector.z = pElement->rotation[numRotation - 1].fRot[2];
					rotVector.w = pElement->rotation[numRotation - 1].fRot[3];
				}
			}
			else
			{
				bool p1 = false;
				bool p2 = false;
				if( tempTime.start > pElement->rotation[i1].dwTime)
					p1 = true;
				if( tempTime.end < pElement->rotation[i2].dwTime)
					p2 = true;
				if( p1 && p2 )	
				{}
				else if( p1 && (!p2))
				{	
					rotVector.x = pElement->rotation[i2].fRot[0];
					rotVector.y = pElement->rotation[i2].fRot[1];
					rotVector.z = pElement->rotation[i2].fRot[2];
					rotVector.w = pElement->rotation[i2].fRot[3];
				}
				else if((!p1)&&p2)
				{
					rotVector.x = pElement->rotation[i1].fRot[0];
					rotVector.y = pElement->rotation[i1].fRot[1];
					rotVector.z = pElement->rotation[i1].fRot[2];
					rotVector.w = pElement->rotation[i1].fRot[3];
				}
				else
				{
					MathHelper::GetQuaternionValue( rotVector, time,
						pElement->rotationType,
						&pElement->rotation[i1],&pElement->rotation[i2]);
				}
			}
		}

		size_t numScaling = pElement->scaling.size();
		if( numScaling > 0)
		{
			size_t i1 = 0xFFFFFFFF;
			size_t i2 = 0xFFFFFFFF;

			if( pElement->scalingGlobalSeqID != 0xFFFFFFFF )
			{
				tempTime.start = 0;
				tempTime.end = mGlobalSequence[pElement->scalingGlobalSeqID].dwDuration;
				tempTime.time = mGlobalSequence[pElement->scalingGlobalSeqID].dwTime;
			}
			else
			{
				tempTime.start = mTime.start;
				tempTime.end   = mTime.end;
				tempTime.time  = mTime.time;
			}
			DWORD time = tempTime.time;

			for ( size_t i = 0; i < (numScaling - 1); i++)
			{
				if( time >= pElement->scaling[i].dwTime && 
					time < pElement->scaling[i + 1].dwTime )
				{
					i1 = i;
					i2 = i + 1;
					break;
				}
			}

			if( i1 == 0xFFFFFFFF || i2 == 0xFFFFFFFF )
			{
				if ( (time < pElement->scaling[0].dwTime) &&
					(tempTime.end >= pElement->scaling[0].dwTime) ) //取首帧的值
				{
					scaleVector.x = pElement->scaling[0].fScale[0];
					scaleVector.y = pElement->scaling[0].fScale[1];
					scaleVector.z = pElement->scaling[0].fScale[2];
				}	
				else if( (time >= pElement->scaling[numScaling - 1].dwTime) &&
					(tempTime.start<=pElement->scaling[numScaling-1].dwTime) ) //取尾帧的值
				{
					scaleVector.x = pElement->scaling[numScaling - 1].fScale[0];
					scaleVector.y = pElement->scaling[numScaling - 1].fScale[1];
					scaleVector.z = pElement->scaling[numScaling - 1].fScale[2];
				}
			}
			else
			{
				bool p1 = false;
				bool p2 = false;
				if( tempTime.start > pElement->scaling[i1].dwTime)
					p1 = true;
				if( tempTime.end < pElement->scaling[i2].dwTime)
					p2= true;
				if( p1 && p2 )
				{}
				else if( p1 && (!p2) )
				{	
					scaleVector.x = pElement->scaling[i2].fScale[0];
					scaleVector.y = pElement->scaling[i2].fScale[1];
					scaleVector.z = pElement->scaling[i2].fScale[2];
				}
				else if((!p1)&&p2)
				{
					scaleVector.x = pElement->scaling[i1].fScale[0];
					scaleVector.y = pElement->scaling[i1].fScale[1];
					scaleVector.z = pElement->scaling[i1].fScale[2];
				}
				else
				{
					MathHelper::GetInterpolatedValue( scaleVector, time, pElement->scalingType,
						&pElement->scaling[i1],&pElement->scaling[i2]);
				}
			}

		}

		Ogre::Vector3 translationVector = Ogre::Vector3(0.0f,0.0f,0.0f);
		Ogre::Vector3 ScalingVector = Ogre::Vector3(1.0f,1.0f,1.0f);
		Ogre::Vector3 PivotPoint = Ogre::Vector3(0.0f,0.0f,0.0f);

		translationVector.x = translateVector.x;
		translationVector.y = translateVector.y;
		translationVector.z = translateVector.z;

		ScalingVector.x = scaleVector.x;
		ScalingVector.y = scaleVector.y;
		ScalingVector.z = scaleVector.z;

		PivotPoint.x = pElement->pivotPoint[0];
		PivotPoint.y = pElement->pivotPoint[1];
		PivotPoint.z = pElement->pivotPoint[2];

		Ogre::Matrix4 scMatrix;
		Ogre::Matrix4 scInverseMatrix;
		Ogre::Matrix4 sMatrix;
		Ogre::Matrix4 rMatrix;
		Ogre::Matrix4 tMatrix;

		scMatrix.makeTransform( PivotPoint, Ogre::Vector3::UNIT_SCALE, Ogre::Quaternion::IDENTITY);
		sMatrix.makeTransform( Ogre::Vector3::ZERO, ScalingVector, Ogre::Quaternion::IDENTITY );
		rMatrix.makeTransform( Ogre::Vector3::ZERO, Ogre::Vector3::UNIT_SCALE, rotVector );
		tMatrix.makeTransform( translationVector, Ogre::Vector3::UNIT_SCALE, Ogre::Quaternion::IDENTITY );
		scInverseMatrix = scMatrix.inverse();

		pElement->matrix = tMatrix * scMatrix * rMatrix * scInverseMatrix * scMatrix * sMatrix * scInverseMatrix;

		if( pElement->parentID != 0xFFFFFFFF)
		{
			tagBONEMATRIX *parent=NULL;
			unsigned int x = GetMatrixIndexByID( pElement->parentID );
			parent = &mMatrix[x];
			pElement->matrix = parent->matrix * pElement->matrix;
		}
	}
}

void CMdxModel::TransformVertex( float* pVertex,
								const unsigned int bytes,
								const int group,
								const int index )
{
	Ogre::Vector3 TempVector;
	Ogre::Vector4 TempVector2;
	Ogre::Vector3 TempNormalTarget;
	Ogre::Vector4 TempNormalTarget2;
	Ogre::Vector3 NormalTarget;
	Ogre::Vector3 TransformedNormalTarget;
	int MatrixListSize;
	float Scale;
	tagBONEMATRIX* p;
	Ogre::Vector3 vert_pos;

	for( int j = 0; j < mMatrixGroups[group].size(); j++ )
	{
		Ogre::Vector3 Position = Ogre::Vector3::ZERO;
		TransformedNormalTarget = Ogre::Vector3::ZERO;
		vert_pos = Ogre::Vector3( mVertex[group][j].Position[0],
			mVertex[group][j].Position[1],
			mVertex[group][j].Position[2]);
		Ogre::Vector3 vert_normal = Ogre::Vector3( mNormal[group][j].Normal[0],
			mNormal[group][j].Normal[1],
			mNormal[group][j].Normal[2]);

		MatrixListSize = mMatrixGroups[group][j].size();
		for( int k = 0; k < MatrixListSize; k++ )
		{
			int index = GetMatrixIndexByID(mMatrixGroups[group][j][k]);
			p = &mMatrix[index];
			TempVector2 = p->matrix * vert_pos;
			NormalTarget = vert_pos + vert_normal;
			TempNormalTarget2 =  p->matrix * NormalTarget;

			TempVector.x = TempVector2.x;
			TempVector.y = TempVector2.y;
			TempVector.z = TempVector2.z;

			TempNormalTarget.x = TempNormalTarget2.x;
			TempNormalTarget.y = TempNormalTarget2.y;
			TempNormalTarget.z = TempNormalTarget2.z;

			Position = Position + TempVector;
			TransformedNormalTarget = TransformedNormalTarget + 
				TempNormalTarget;

		}
		Scale = (MatrixListSize == 0) ? 0.0f : (1.0f / static_cast<float>(MatrixListSize));
		Position = Position * Scale;
		TransformedNormalTarget = TransformedNormalTarget * Scale;

		pVertex[j * bytes]		= Position[0];
		pVertex[j * bytes + 1]	= Position[1];
		pVertex[j * bytes + 2]	= Position[2];
		pVertex[j * bytes + 3]	= TransformedNormalTarget.x - Position.x;
		pVertex[j * bytes + 4]	= TransformedNormalTarget.y - Position.y;
		pVertex[j * bytes + 5]	= TransformedNormalTarget.z - Position.z;
		if( mTextureAnimationID[index] != 0xFFFFFFFF )
		{
			pVertex[j * bytes + 6] = mTexCoord[group][j].UV[0] + 
				mTextureAnimation[mTextureAnimationID[index]].fTrans[0];
			pVertex[j * bytes + 7] = mTexCoord[group][j].UV[1] + 
				mTextureAnimation[mTextureAnimationID[index]].fTrans[1];
		}
	}
}
void CMdxModel::Clear()
{
	mTotalFrames = 0xFFFFFFFF;

	mTime.start = 0;
	mTime.end = 0;
	mTime.time = 0;
	mCurFrame = 0;

	mMaterial.clear();
	mTexCoord.clear();
	mFace.clear();
	mVertex.clear();
	mNormal.clear();
	mMatrixGroups.clear();
	mMatrix.clear();
	mMatrixOrder.clear();
	mGlobalSequence.clear();
	mAnimation.clear();
	mMaterialGroups.clear();
	mTextureGroups.clear();
	mGeosetAnimation.clear();
	mTextureAnimation.clear();

	if( mEntity != NULL )
	{
		mSceneMgr->destroyEntity( mEntity );
		mEntity = NULL;
	}
}
bool CMdxModel::IsExist( const char *filename )
{
	return mMdxFile->IsExist( filename );
}