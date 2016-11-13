#include "MdxHeader.h"
#include "MdxModel.h"
#include "LoadBlpImage.h"

#define TAG(x) (unsigned int)(  (((unsigned int)x & 0x0000ff00) << 8  ) +  \
	(((unsigned int)x & 0x000000ff) << 24 ) +  \
	(((unsigned int)x & 0x00ff0000) >> 8  ) +  \
	(((unsigned int)x & 0xff000000) >> 24 ) )

CMDXFile::CMDXFile()
{
	m_pFileData = NULL;
	BOOL result = SFileOpenArchive("E:\\Warcraft III\\War3.mpq", 
		0, 0, &m_mpqWar3 );
	if( !result )
		MessageBox( NULL, TEXT("Can't open the file War3.mpq!"),
					NULL, MB_OK);
}

CMDXFile::~CMDXFile()
{
	if( m_pFileData != NULL )
	{
		delete []m_pFileData;
		m_pFileData = NULL;
	}
	if( m_mpqWar3 != NULL )
		SFileCloseArchive( m_mpqWar3 );
}

void CMDXFile::ReadVersion()
{
	unsigned int Version = *(unsigned int*)(m_pFileData + m_CurrentDataPos);
	m_CurrentDataPos += 4;
	printf( "version:%d\n", Version );
}

void CMDXFile::ReadModel()
{
	char szModelName[80];
	char szAnimFileName[260];
	float fRadius;
	float fMin[3];
	float fMax[3];
	unsigned int dwBlendTime;
	memcpy( szModelName, m_pFileData + m_CurrentDataPos, 80 );
	m_CurrentDataPos += 80;
	memcpy( szAnimFileName, m_pFileData + m_CurrentDataPos, 260 );
	m_CurrentDataPos += 260;
	memcpy( &fRadius, m_pFileData + m_CurrentDataPos, 4 );
	m_CurrentDataPos += 4;
	memcpy( mModel->mMin, m_pFileData + m_CurrentDataPos, 4 * 3 );
	m_CurrentDataPos += 4 * 3;
	memcpy( mModel->mMax, m_pFileData + m_CurrentDataPos, 4 * 3 );
	m_CurrentDataPos += 4 * 3;
	memcpy( &dwBlendTime, m_pFileData + m_CurrentDataPos, 4 );
	m_CurrentDataPos += 4;

	mModel->mModelName = szModelName;
}

void CMDXFile::ReadSequence()
{
	char szActionName[80];
	unsigned int startTime;
	unsigned int endTime;
	float fMoveSpeed;
	unsigned int Flags;
	float fRarity;
	float fSyncPoint;
	float fRadius;
	float fMin[3];
	float fMax[3];
	tagANIMATION anim;
	for( unsigned int i = 0; i < m_CurrentChunkSize / 132; i++ )
	{
		memcpy( anim.szActionName, m_pFileData + m_CurrentDataPos, 80 );
		m_CurrentDataPos += 80;
		memcpy( &anim.dwStartTime, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &anim.dwEndTime, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &anim.fMoveSpeed, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &Flags, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &fRarity, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &fSyncPoint, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &fRadius, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( fMin, m_pFileData + m_CurrentDataPos, 4 * 3 );
		m_CurrentDataPos += 4 * 3;
		memcpy( fMax, m_pFileData + m_CurrentDataPos, 4 * 3 );
		m_CurrentDataPos += 4 * 3;
		mModel->mAnimation.push_back(anim);
	}
}

void CMDXFile::ReadGlobalSequence()
{
	tagGLOBALSEQ globalSeq;
	for( unsigned int i = 0; i < m_CurrentChunkSize / 4; i++ )
	{
		memcpy( &globalSeq.dwDuration, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		globalSeq.dwTime = 0;
		mModel->mGlobalSequence.push_back(globalSeq);
	}
}

void CMDXFile::ReadMaterial()
{
	unsigned int OrigPos = m_CurrentDataPos;
	unsigned int PriorityPlan = 0;
	unsigned int Flags = 0;
	unsigned int IncludeSize = 0;
	unsigned int Tag = 0;

	while( m_CurrentDataPos < OrigPos + m_CurrentChunkSize )
	{
		LayerList LayerCollector; 
		memcpy( &IncludeSize, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &PriorityPlan, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &Flags, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &Tag, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		static int count = 1;
		printf( "Material %d:\n", count++ );
		if( TAG(Tag) == 'LAYS' )
		{
			unsigned int NumLayers;
			unsigned int _IncludeSize;
			memcpy( &NumLayers, m_pFileData + m_CurrentDataPos, 4 );
			m_CurrentDataPos += 4;
			
			for( unsigned int i = 0; i < NumLayers; i++ )
			{
				tagLAYER TempLayer;
				unsigned int FilterMode;
				unsigned int ShadingFlags;
				unsigned int TextureID;
				unsigned int TextureAnimationID;
				unsigned int CoordinateID;
				float Alpha;
				unsigned int _OrigPos = m_CurrentDataPos;
				memcpy( &_IncludeSize, m_pFileData + m_CurrentDataPos, 4 );
				m_CurrentDataPos += 4;
				memcpy( &FilterMode, m_pFileData + m_CurrentDataPos, 4 );
				m_CurrentDataPos += 4;
				memcpy( &ShadingFlags, m_pFileData + m_CurrentDataPos, 4 );
				m_CurrentDataPos += 4;
				memcpy( &TextureID, m_pFileData + m_CurrentDataPos, 4 );
				m_CurrentDataPos += 4;
				memcpy( &TextureAnimationID, m_pFileData + m_CurrentDataPos, 4 );
				m_CurrentDataPos += 4;
				memcpy( &CoordinateID, m_pFileData + m_CurrentDataPos, 4 );
				m_CurrentDataPos += 4;
				memcpy( &Alpha, m_pFileData + m_CurrentDataPos, 4 );
				m_CurrentDataPos += 4;
				///////////////////////////////////////////////////////////////////////////
				TempLayer.FilterMode			= FilterMode;
				TempLayer.ShadingFlags			= ShadingFlags;
				TempLayer.TextureID				= TextureID;
				TempLayer.TextureAnimationID	= TextureAnimationID;
				
				///////////////////////////////////////////////////////////////////////////
				while( m_CurrentDataPos < _OrigPos + _IncludeSize )
				{
					memcpy( &Tag, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;

					switch( TAG(Tag) )
					{
					case 'KMTA':
						{
							unsigned int NumTracks;
							unsigned int Type;
							unsigned int GlobalSequenceID;
							unsigned int Time;
							float Alpha;
							float InTan;
							float OutTan;
							tagALPHATIME alphaTime;

							memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
							m_CurrentDataPos += 4;
							memcpy( &TempLayer.dwAlphaType, m_pFileData + m_CurrentDataPos, 4 );
							m_CurrentDataPos += 4;
							memcpy( &TempLayer.dwAlphaGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
							m_CurrentDataPos += 4;
							for( unsigned int k = 0; k < NumTracks; k++ )
							{
								memcpy( &alphaTime.time, m_pFileData + m_CurrentDataPos, 4 );
								m_CurrentDataPos += 4;
								memcpy( &alphaTime.alpha, m_pFileData + m_CurrentDataPos, 4 );
								m_CurrentDataPos += 4;
								if( TempLayer.dwAlphaType > 1 )
								{
									memcpy( &alphaTime.inTan, m_pFileData + m_CurrentDataPos, 4 );
									m_CurrentDataPos += 4;
									memcpy( &alphaTime.outTan, m_pFileData + m_CurrentDataPos, 4 );
									m_CurrentDataPos += 4;
								}
								TempLayer.alpha.push_back( alphaTime );
							}
							break;
						}
					case 'KMTF':
						{
							unsigned int NumTracks;
							unsigned int Type;
							unsigned int GlobalSequenceID;
							unsigned int Time;
							unsigned int TextureID;
							float InTan;
							float OutTan;
							tagTEXTUREIDTIME textureIDTime;
							memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
							m_CurrentDataPos += 4;
							memcpy( &TempLayer.dwTextureIDType, m_pFileData + m_CurrentDataPos, 4 );
							m_CurrentDataPos += 4;
							memcpy( &TempLayer.dwTextureIDGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
							m_CurrentDataPos += 4;
							for( unsigned int k = 0; k < NumTracks; k++ )
							{
								memcpy( &textureIDTime.time, m_pFileData + m_CurrentDataPos, 4 );
								m_CurrentDataPos += 4;
								memcpy( &textureIDTime.textureID, m_pFileData+ m_CurrentDataPos, 4 );
								m_CurrentDataPos += 4;
								if( TempLayer.dwTextureIDType > 1 )
								{
									memcpy( &textureIDTime.inTan, m_pFileData + m_CurrentDataPos, 4 );
									m_CurrentDataPos += 4;
									memcpy( &textureIDTime.outTan, m_pFileData + m_CurrentDataPos, 4 );
									m_CurrentDataPos += 4;
								}
								TempLayer.textureID.push_back( textureIDTime );
							}
							break;
						}
					default:
						break;
					}
				}

				LayerCollector.push_back( TempLayer );
			}
		}
		else
			m_CurrentDataPos -= 4 * 3;
		mModel->mMaterialGroups.push_back( LayerCollector );
	}
}

void CMDXFile::ReadTexture()
{
	char szTextureFileName[260];
	unsigned int ReplaceableID;
	unsigned int Flags;
	for( unsigned int i = 0; i < m_CurrentChunkSize / 268; i++ )
	{
		std::string TempTexture;
		memcpy( &ReplaceableID, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( szTextureFileName, m_pFileData + m_CurrentDataPos, 260 );
		m_CurrentDataPos += 260;
		memcpy( &Flags, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;

		size_t TextureNameLength = strlen( szTextureFileName );
		if( TextureNameLength > 0 )
		{
			mModel->mTextureGroups.push_back( szTextureFileName );
		}
		else
		{
			switch( ReplaceableID )
			{
			case 1:
				mModel->mTextureGroups.push_back( "ReplaceableTextures\\TeamColor\\TeamColor00.blp" );
				break;
			case 2:
				mModel->mTextureGroups.push_back( "ReplaceableTextures\\TeamGlow\\TeamGlow00.blp" );
				break;
			case 11:
				mModel->mTextureGroups.push_back( "ReplaceableTextures\\Cliff\\Cliff0.blp" );
				break;
			case 31:
				mModel->mTextureGroups.push_back( "ReplaceableTextures\\LordaeronTree\\LordaeronSummerTree.blp" );
				break;
			case 32:
				mModel->mTextureGroups.push_back( "ReplaceableTextures\\AshenvaleTree\\AshenTree.blp" );
				break;
			case 33:
				mModel->mTextureGroups.push_back( "ReplaceableTextures\\BarrensTree\\BarrensTree.blp" );
				break;
			case 34:
				mModel->mTextureGroups.push_back( "ReplaceableTextures\\NorthrendTree\\NorthTree.blp" );
				break;
			case 35:
				mModel->mTextureGroups.push_back( "ReplaceableTextures\\Mushroom\\MushroomTree.blp" );
				break;
			default:
				mModel->mTextureGroups.push_back( "ReplaceableTextures\\TeamColor\\TeamColor00.blp" );
				break;
			}
		}
	}
}

void CMDXFile::ReadTextureAnimation()
{
	unsigned int Tag;
	unsigned int OrigPos = m_CurrentDataPos;
	unsigned int IncludeSize;
	while( m_CurrentDataPos < OrigPos + m_CurrentChunkSize )
	{
		tagTEXTUREANIMATION textureAnimation;
		tagTRANSLATION		translation;
		tagROTATION			rotation;
		tagSCALING			scaling;

		unsigned int _OrigPos = m_CurrentDataPos;
		memcpy( &IncludeSize, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		while( m_CurrentDataPos < _OrigPos + IncludeSize )
		{
			memcpy( &Tag, m_pFileData + m_CurrentDataPos, 4 );
			m_CurrentDataPos += 4;
			switch( TAG(Tag) )
			{
			case 'KTAT':
				{
					unsigned int NumTracks;
					unsigned int Type;
					unsigned int GlobalSequenceID;

					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &textureAnimation.translationType, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &textureAnimation.translationGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						memcpy( &translation.dwTime, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( translation.fTrans, m_pFileData + m_CurrentDataPos, 4 * 3);
						m_CurrentDataPos += 4 * 3;
						if( textureAnimation.translationType > 1 )
						{
							memcpy( translation.fInTan, m_pFileData + m_CurrentDataPos, 4 * 3);
							m_CurrentDataPos += 4 * 3;
							memcpy( translation.fOutTan, m_pFileData + m_CurrentDataPos, 4 * 3);
							m_CurrentDataPos += 4 * 3;
						}
						textureAnimation.translation.push_back( translation );
					}
					break;
				}
			case 'KTAR':
				{
					unsigned int NumTracks;
					unsigned int Type;
					unsigned int GlobalSequenceID;

					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &textureAnimation.rotationType, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &textureAnimation.rotationGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						memcpy( &rotation.dwTime, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( rotation.fRot, m_pFileData + m_CurrentDataPos, 4 * 4);
						m_CurrentDataPos += 4 * 4;
						if( textureAnimation.rotationType > 1 )
						{
							memcpy( rotation.fInTan, m_pFileData + m_CurrentDataPos, 4 * 4);
							m_CurrentDataPos += 4 * 4;
							memcpy( rotation.fOutTan, m_pFileData + m_CurrentDataPos, 4 * 4);
							m_CurrentDataPos += 4 * 4;
						}
						textureAnimation.rotation.push_back( rotation );
					}
					break;
				}
			case 'KTAS':
				{
					unsigned int NumTracks;
					unsigned int Type;
					unsigned int GlobalSequenceID;

					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &textureAnimation.scalingType, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &textureAnimation.scalingGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						memcpy( &scaling.dwTime, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( scaling.fScale, m_pFileData + m_CurrentDataPos, 4 * 3);
						m_CurrentDataPos += 4 * 3;
						if( textureAnimation.scalingType > 1 )
						{
							memcpy( scaling.fInTan, m_pFileData + m_CurrentDataPos, 4 * 3);
							m_CurrentDataPos += 4 * 3;
							memcpy( scaling.fOutTan, m_pFileData + m_CurrentDataPos, 4 * 3);
							m_CurrentDataPos += 4 * 3;
						}
						textureAnimation.scaling.push_back( scaling );
					}
					break;
				}
			default:
				break;
			}
		}
		mModel->mTextureAnimation.push_back( textureAnimation );
	}
}

void CMDXFile::ReadGeoset()
{
	unsigned int Tag;
	unsigned int OrigPos = m_CurrentDataPos;
	while( m_CurrentDataPos < OrigPos + m_CurrentChunkSize )
	{
		VertexList   VertexCollector;
		NormalList	 NormalCollector;
		TexCoordList TexCoordCollector;
		FaceList     FaceCollector;
		GroupList    GroupCollector;
		MatrixList   MatrixCollector;
		MatrixIDList MatrixIDCollector;

		unsigned int InclusiveSize;
		unsigned int ChunkCount = 0;
		unsigned int _OrigPos = m_CurrentDataPos;
		memcpy( &InclusiveSize, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		while( ChunkCount < 8 )
		{
			memcpy( &Tag, m_pFileData + m_CurrentDataPos, 4 );
			m_CurrentDataPos += 4;
			switch( TAG(Tag) )
			{
			case 'VRTX':
				{
					unsigned int _NumVertices;
					tagVERTEX TempVertex;
					memcpy( &_NumVertices, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < _NumVertices; i++ )
					{
						float Pos[3];
						memcpy( Pos, m_pFileData + m_CurrentDataPos, 4 * 3 );
						m_CurrentDataPos += 4 * 3;
						TempVertex.Position[0] = Pos[0];
						TempVertex.Position[1] = Pos[1];
						TempVertex.Position[2] = Pos[2];
						for( int idx = 0; idx < 3; ++idx )
						{
							if( Pos[idx] > mModel->mMax[idx] )
								mModel->mMax[idx] = Pos[idx];
							if( Pos[idx] < mModel->mMin[idx] )
								mModel->mMin[idx] = Pos[idx];
						}
						VertexCollector.push_back( TempVertex );
					}
					if( _NumVertices > 0 )
					{
						mModel->mVertex.push_back( VertexCollector );
					}
					ChunkCount++;
					break;
				}
			case 'NRMS':
				{
					unsigned int _NumNormals;
					tagVERTEXNORMAL TempNormal;
					memcpy( &_NumNormals, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < _NumNormals; i++ )
					{
						float Normal[3];
						memcpy( Normal, m_pFileData + m_CurrentDataPos, 4 * 3 );
						m_CurrentDataPos += 4 * 3;
						TempNormal.Normal[0] = Normal[0];
						TempNormal.Normal[1] = Normal[1];
						TempNormal.Normal[2] = Normal[2];
						NormalCollector.push_back( TempNormal );
					}
					if( _NumNormals > 0 )
					{
						mModel->mNormal.push_back( NormalCollector );
					}
					ChunkCount++;
					break;
				}
			case 'PTYP':
				{
					unsigned int NumFaceTypeGroups;
					memcpy( &NumFaceTypeGroups, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumFaceTypeGroups; i++ )
					{
						unsigned int Type;
						memcpy( &Type, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
					}
					ChunkCount++;
					break;
				}
			case 'PCNT':
				{
					unsigned int FaceGroups;
					memcpy( &FaceGroups, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < FaceGroups; i++ )
					{
						unsigned int NumIndices;
						memcpy( &NumIndices, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
					}
					ChunkCount++;
					break;
				}
			case 'PVTX':
				{
					unsigned int NumTotalIndices;
					tagFACE TempFace;
					memcpy( &NumTotalIndices, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTotalIndices / 3; i++ )
					{
						unsigned short Index[3];
						memcpy( Index, m_pFileData + m_CurrentDataPos, 2 * 3 );
						m_CurrentDataPos += 2 * 3;
						TempFace.VertexIndex[0] = Index[0];
						TempFace.VertexIndex[1] = Index[1];
						TempFace.VertexIndex[2] = Index[2];
						FaceCollector.push_back( TempFace );
					}
					mModel->mFace.push_back( FaceCollector );
					ChunkCount++;
					break;
				}
			case 'GNDX':
				{
					unsigned int NumMatrixGroups;
					memcpy( &NumMatrixGroups, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumMatrixGroups; i++ )
					{
						unsigned char MatrixGroup;
						memcpy( &MatrixGroup, m_pFileData + m_CurrentDataPos, 1 );
						m_CurrentDataPos += 1;
						GroupCollector.push_back( MatrixGroup );
					}
					m_GroupGroups.push_back( GroupCollector );
					ChunkCount++;
					break;
				}
			case 'MTGC':
				{
					unsigned int NumMatrixGroupIndices;
					memcpy( &NumMatrixGroupIndices, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumMatrixGroupIndices; i++ )
					{
						unsigned int MatrixIndex;
						memcpy( &MatrixIndex, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						MatrixCollector.push_back( MatrixIndex );
					}
					m_MatrixGroups.push_back( MatrixCollector );
					ChunkCount++;
					break;
				}
			case 'MATS':
				{
					unsigned int NumMatrixID;
					memcpy( &NumMatrixID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumMatrixID; i++ )
					{
						unsigned int MatrixID;
						memcpy( &MatrixID, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						MatrixIDCollector.push_back( MatrixID );
					}
					m_MatrixIDGroups.push_back( MatrixIDCollector );
					ChunkCount++;
					break;
				}
			default:
				break;
			}
		}
		unsigned int MaterialID;
		unsigned int SelectionGroup;
		unsigned int SelectionFlags;
		memcpy( &MaterialID, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		/////////////////////////////////////////////////////////////////
		mModel->mMaterial.push_back( MaterialID );

		/////////////////////////////////////////////////////////////////
		memcpy( &SelectionGroup, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &SelectionFlags, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		float BoundingRadius;
		float Min[3];
		float Max[3];
		unsigned int NumExtents;
		memcpy( &BoundingRadius, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( Min, m_pFileData + m_CurrentDataPos, 4 * 3 );
		m_CurrentDataPos += 4 * 3;
		memcpy( Max, m_pFileData + m_CurrentDataPos, 4 * 3 );
		m_CurrentDataPos += 4 * 3;
		memcpy( &NumExtents, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		for( unsigned int k = 0; k < NumExtents; k++ )
		{
			float _BoundingRadius, _Min[3], _Max[3];
			memcpy( &_BoundingRadius, m_pFileData + m_CurrentDataPos, 4 );
			m_CurrentDataPos += 4;
			memcpy( _Min, m_pFileData + m_CurrentDataPos, 4 * 3 );
			m_CurrentDataPos += 4 * 3;
			memcpy( _Max, m_pFileData + m_CurrentDataPos, 4 * 3 );
			m_CurrentDataPos += 4 * 3;
		}


		for( unsigned int i = 0; i < 2; i++ )
		{
			memcpy( &Tag, m_pFileData + m_CurrentDataPos, 4 );
			m_CurrentDataPos += 4;
			switch( TAG(Tag) )
			{
			case 'UVAS':
				{
					unsigned int NumTextureVertexGroups;
					memcpy( &NumTextureVertexGroups, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					break;
				}
			case 'UVBS':
				{
					unsigned int NumVertexTexturePosition;
					tagTEXCOORD TempTexCoord;
					memcpy( &NumVertexTexturePosition, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumVertexTexturePosition; i++ )
					{
						float UVPos[2];
						memcpy( UVPos, m_pFileData + m_CurrentDataPos, 4 * 2 );
						m_CurrentDataPos += 4 * 2;
						TempTexCoord.UV[0] = UVPos[0];
						TempTexCoord.UV[1] = UVPos[1];
						TexCoordCollector.push_back( TempTexCoord );
					}
					if( NumVertexTexturePosition > 0 )
					{
						mModel->mTexCoord.push_back( TexCoordCollector );
					}
					break;
				}
			default:
				m_CurrentDataPos -= 4;
			}
		}
	}
}

void CMDXFile::ReadGeosetAnimation()
{
	unsigned int OrigPos = m_CurrentDataPos;
	while( m_CurrentDataPos < OrigPos + m_CurrentChunkSize )
	{
		tagGEOSETANIMATION geosetAnimation;
		unsigned int InclusiveSize;
		float Alpha;
		unsigned int Flags;
		float Color[3];
		unsigned int GeosetID;
		unsigned int Tag;
		unsigned int _OrigPos = m_CurrentDataPos;
		memcpy( &InclusiveSize, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &geosetAnimation.fAlpha, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &geosetAnimation.dwFlags, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( geosetAnimation.fColor, m_pFileData + m_CurrentDataPos, 4 * 3 );
		m_CurrentDataPos += 4 * 3;
		memcpy( &geosetAnimation.dwGeosetID, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;

		while( m_CurrentDataPos < _OrigPos + InclusiveSize )
		{
			tagALPHATIME alpha;
			tagCOLORTIME color;
			memcpy( &Tag, m_pFileData + m_CurrentDataPos, 4 );
			m_CurrentDataPos += 4;
			switch( TAG(Tag) )
			{
			case 'KGAO':
				{
					unsigned int NumTracks;
					unsigned int Type;
					unsigned int GlobalSequenceID;
					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &geosetAnimation.alphaType, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &geosetAnimation.alphaGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						unsigned int Time;
						float _Alpha;
						memcpy( &alpha.time, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( &alpha.alpha, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						if( geosetAnimation.alphaType > 1 )
						{
							float InTan, OutTan;
							memcpy( &alpha.inTan, m_pFileData + m_CurrentDataPos, 4 );
							m_CurrentDataPos += 4;
							memcpy( &alpha.outTan, m_pFileData + m_CurrentDataPos, 4 );
							m_CurrentDataPos += 4;
						}
						geosetAnimation.alpha.push_back( alpha );
					}
					break;
				}
			case 'KGAC':
				{
					unsigned int NumTracks;
					unsigned int Type;
					unsigned int GlobalSequenceID;
					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &geosetAnimation.colorType, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &geosetAnimation.colorGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						unsigned int Time;
						float Color[3];
						memcpy( &color.time, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( color.color, m_pFileData + m_CurrentDataPos, 4 * 3 );
						m_CurrentDataPos += 4 * 3;
						if( geosetAnimation.colorType > 1 )
						{
							float InTan[3], OutTan[3];
							memcpy( color.inTan, m_pFileData + m_CurrentDataPos, 4 *3 );
							m_CurrentDataPos += 4 * 3;
							memcpy( color.outTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;
						}
						geosetAnimation.color.push_back( color );
					}
					break;
				}
			default:
				break;
			}
		}
		mModel->mGeosetAnimation.push_back( geosetAnimation );
	}
}

void CMDXFile::ReadBone()
{
	unsigned int OrigPos = m_CurrentDataPos;
	while( m_CurrentDataPos < OrigPos + m_CurrentChunkSize )
	{
		tagBONEMATRIX matrix;
		tagTRANSLATION translation;
		tagROTATION rotation;
		tagSCALING scaling;

		char NodeName[80];
		unsigned int InclusiveSize;
		unsigned int ObjectID;
		unsigned int ParentID;
		unsigned int Flags;
		unsigned int Tag;
		unsigned _OrigPos = m_CurrentDataPos;
		memcpy( &InclusiveSize, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( NodeName, m_pFileData + m_CurrentDataPos, 80 );
		m_CurrentDataPos += 80;
		memcpy( &ObjectID, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &ParentID, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &Flags, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		while( m_CurrentDataPos < _OrigPos + InclusiveSize )
		{
			memcpy( &Tag, m_pFileData + m_CurrentDataPos, 4 );
			m_CurrentDataPos += 4;
			switch( TAG(Tag) )
			{
			case 'KGTR':
				{
					unsigned int NumTracks;
					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.translationType, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.translationGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						memcpy( &translation.dwTime, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( translation.fTrans, m_pFileData + m_CurrentDataPos, 4 * 3 );
						m_CurrentDataPos += 4 * 3;
						if( matrix.translationType > 1 )
						{
							memcpy( translation.fInTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;
							memcpy( translation.fOutTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;							 
						}
						matrix.translation.push_back(translation);
					}
					break;
				}
			case 'KGRT':
				{
					unsigned int NumTracks;
					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.rotationType, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.rotationGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						memcpy( &rotation.dwTime, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( rotation.fRot, m_pFileData + m_CurrentDataPos, 4 * 4 );
						m_CurrentDataPos += 4 * 4;
						if( matrix.rotationType > 1 )
						{
							memcpy( rotation.fInTan, m_pFileData + m_CurrentDataPos, 4 * 4 );
							m_CurrentDataPos += 4 * 4;
							memcpy( rotation.fOutTan, m_pFileData + m_CurrentDataPos, 4 * 4 );
							m_CurrentDataPos += 4 * 4;
						}
						matrix.rotation.push_back(rotation);
					}
					break;
				}
			case 'KGSC':
				{
					unsigned int NumTracks;
					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.scalingType, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.scalingGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						memcpy( &scaling.dwTime, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( scaling.fScale, m_pFileData + m_CurrentDataPos, 4 * 3 );
						m_CurrentDataPos += 4 * 3;
						if( matrix.scalingType > 1 )
						{
							memcpy( scaling.fInTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;
							memcpy( scaling.fOutTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;
						}
						matrix.scaling.push_back(scaling);
					}
					break;
				}
			default:
				break;
			}
		}
		unsigned int GeosetID;
		unsigned int GeosetAnimationID;

		memcpy( &GeosetID, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &GeosetAnimationID, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;

		matrix.objectID = ObjectID;
		matrix.parentID = ParentID;
		mModel->mMatrix.push_back(matrix);
	}
}

void CMDXFile::ReadLight()
{
	m_CurrentDataPos += m_CurrentChunkSize;
}
void CMDXFile::ReadHelpher()
{
	unsigned int OrigPos = m_CurrentDataPos;
	while( m_CurrentDataPos < OrigPos + m_CurrentChunkSize )
	{
		tagBONEMATRIX matrix;
		tagTRANSLATION translation;
		tagROTATION rotation;
		tagSCALING scaling;

		char NodeName[80];
		unsigned int InclusiveSize;
		unsigned int ObjectID;
		unsigned int ParentID;
		unsigned int Flags;
		unsigned int Tag;
		unsigned _OrigPos = m_CurrentDataPos;
		memcpy( &InclusiveSize, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( NodeName, m_pFileData + m_CurrentDataPos, 80 );
		m_CurrentDataPos += 80;
		memcpy( &ObjectID, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &ParentID, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &Flags, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		while( m_CurrentDataPos < _OrigPos + InclusiveSize )
		{
			memcpy( &Tag, m_pFileData + m_CurrentDataPos, 4 );
			m_CurrentDataPos += 4;
			switch( TAG(Tag) )
			{
			case 'KGTR':
				{
					unsigned int NumTracks;
					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.translationType, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.translationGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						memcpy( &translation.dwTime, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( translation.fTrans, m_pFileData + m_CurrentDataPos, 4 * 3 );
						m_CurrentDataPos += 4 * 3;
						if( matrix.translationType > 1 )
						{
							memcpy( translation.fInTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;
							memcpy( translation.fOutTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;							 
						}
						matrix.translation.push_back(translation);
					}
					break;
				}
			case 'KGRT':
				{
					unsigned int NumTracks;
					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.rotationType, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.rotationGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						memcpy( &rotation.dwTime, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( rotation.fRot, m_pFileData + m_CurrentDataPos, 4 * 4 );
						m_CurrentDataPos += 4 * 4;
						if( matrix.rotationType > 1 )
						{
							memcpy( rotation.fInTan, m_pFileData + m_CurrentDataPos, 4 * 4 );
							m_CurrentDataPos += 4 * 4;
							memcpy( rotation.fOutTan, m_pFileData + m_CurrentDataPos, 4 * 4 );
							m_CurrentDataPos += 4 * 4;
						}
						matrix.rotation.push_back(rotation);
					}
					break;
				}
			case 'KGSC':
				{
					unsigned int NumTracks;
					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.scalingType, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.scalingGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						memcpy( &scaling.dwTime, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( scaling.fScale, m_pFileData + m_CurrentDataPos, 4 * 3 );
						m_CurrentDataPos += 4 * 3;
						if( matrix.scalingType > 1 )
						{
							memcpy( scaling.fInTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;
							memcpy( scaling.fOutTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;
						}
						matrix.scaling.push_back(scaling);
					}
					break;
				}
			default:
				break;
			}
		}
		matrix.objectID = ObjectID;
		matrix.parentID = ParentID;
		mModel->mMatrix.push_back(matrix);
	}
}

void CMDXFile::ReadAttachment()
{
	unsigned int OrigPos = m_CurrentDataPos;
	while( m_CurrentDataPos < OrigPos + m_CurrentChunkSize )
	{
		tagBONEMATRIX matrix;
		tagTRANSLATION translation;
		tagROTATION rotation;
		tagSCALING scaling;

		unsigned int InclusiveSize;
		unsigned int _OrigPos = m_CurrentDataPos;
		memcpy( &InclusiveSize, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;

		char NodeName[80];
		unsigned int _InclusiveSize;
		unsigned int ObjectID;
		unsigned int ParentID;
		unsigned int Flags;
		unsigned int Tag;
		unsigned __OrigPos = m_CurrentDataPos;
		memcpy( &_InclusiveSize, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( NodeName, m_pFileData + m_CurrentDataPos, 80 );
		m_CurrentDataPos += 80;
		memcpy( &ObjectID, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &ParentID, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &Flags, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		while( m_CurrentDataPos < _OrigPos + _InclusiveSize )
		{
			memcpy( &Tag, m_pFileData + m_CurrentDataPos, 4 );
			m_CurrentDataPos += 4;
			switch( TAG(Tag) )
			{
			case 'KGTR':
				{
					unsigned int NumTracks;
					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.translationType, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.translationGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						memcpy( &translation.dwTime, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( translation.fTrans, m_pFileData + m_CurrentDataPos, 4 * 3 );
						m_CurrentDataPos += 4 * 3;
						if( matrix.translationType > 1 )
						{
							memcpy( translation.fInTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;
							memcpy( translation.fOutTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;							 
						}
						matrix.translation.push_back(translation);
					}
					break;
				}
			case 'KGRT':
				{
					unsigned int NumTracks;
					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.rotationType, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.rotationGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						memcpy( &rotation.dwTime, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( rotation.fRot, m_pFileData + m_CurrentDataPos, 4 * 4 );
						m_CurrentDataPos += 4 * 4;
						if( matrix.rotationType > 1 )
						{
							memcpy( rotation.fInTan, m_pFileData + m_CurrentDataPos, 4 * 4 );
							m_CurrentDataPos += 4 * 4;
							memcpy( rotation.fOutTan, m_pFileData + m_CurrentDataPos, 4 * 4 );
							m_CurrentDataPos += 4 * 4;
						}
						matrix.rotation.push_back(rotation);
					}
					break;
				}
			case 'KGSC':
				{
					unsigned int NumTracks;
					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.scalingType, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.scalingGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						memcpy( &scaling.dwTime, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( scaling.fScale, m_pFileData + m_CurrentDataPos, 4 * 3 );
						m_CurrentDataPos += 4 * 3;
						if( matrix.scalingType > 1 )
						{
							memcpy( scaling.fInTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;
							memcpy( scaling.fOutTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;
						}
						matrix.scaling.push_back(scaling);
					}
					break;
				}
			default:
				break;
			}
		}

		matrix.objectID = ObjectID;
		matrix.parentID = ParentID;
		mModel->mMatrix.push_back(matrix);

		char Path[260];
		unsigned int AttachmentID;
		memcpy( Path, m_pFileData + m_CurrentDataPos, 260 );
		m_CurrentDataPos += 260;
		memcpy( &AttachmentID, m_pFileData + m_CurrentDataPos, 1 );
		m_CurrentDataPos += 4;
		while( m_CurrentDataPos < _OrigPos + InclusiveSize )
		{
			memcpy( &Tag, m_pFileData + m_CurrentDataPos, 4 );
			m_CurrentDataPos += 4;
			switch( TAG(Tag) )
			{
			case 'KATV':
				{
					unsigned int NumTracks;
					unsigned int Type;
					unsigned int GlobalSequenceID;
					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &Type, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &GlobalSequenceID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						unsigned int Time;
						float Visibility;
						memcpy( &Time, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( &Visibility, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						if( Type > 1 )
						{
							float InTan, OutTan;
							memcpy( &InTan, m_pFileData + m_CurrentDataPos, 4 );
							m_CurrentDataPos += 4;
							memcpy( &OutTan, m_pFileData + m_CurrentDataPos, 4 );
							m_CurrentDataPos += 4;
						}
					}
				}
			default:
				break;
			}
		}
		//printf( "辅助物名:%s\n", NodeName );
	}
}

void CMDXFile::ReadPivotPoint()
{
	unsigned int OrigPos = m_CurrentDataPos;
	tagVERTEX PivotPoint;
	while( m_CurrentDataPos < OrigPos + m_CurrentChunkSize )
	{
		memcpy( PivotPoint.Position, m_pFileData + m_CurrentDataPos, 4 * 3 );
		m_CurrentDataPos += 4 * 3;
		m_PivotPoints.push_back(PivotPoint);
	}
}

void CMDXFile::ReadParticleEmitter()
{
	m_CurrentDataPos += m_CurrentChunkSize;
}

void CMDXFile::ReadParticleEmitter2()
{
	m_CurrentDataPos += m_CurrentChunkSize;
}

void CMDXFile::ReadRibbonEmitter()
{
	m_CurrentDataPos += m_CurrentChunkSize;
}

void CMDXFile::ReadEventObject()
{
	unsigned int OrigPos = m_CurrentDataPos;
	while( m_CurrentDataPos < OrigPos + m_CurrentChunkSize )
	{
		tagBONEMATRIX matrix;
		tagTRANSLATION translation;
		tagROTATION rotation;
		tagSCALING scaling;

		char NodeName[80];
		unsigned int InclusiveSize;
		unsigned int ObjectID;
		unsigned int ParentID;
		unsigned int Flags;
		unsigned int Tag;
		unsigned _OrigPos = m_CurrentDataPos;
		memcpy( &InclusiveSize, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( NodeName, m_pFileData + m_CurrentDataPos, 80 );
		m_CurrentDataPos += 80;
		memcpy( &ObjectID, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &ParentID, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &Flags, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		while( m_CurrentDataPos < _OrigPos + InclusiveSize )
		{
			memcpy( &Tag, m_pFileData + m_CurrentDataPos, 4 );
			m_CurrentDataPos += 4;
			switch( TAG(Tag) )
			{
			case 'KGTR':
				{
					unsigned int NumTracks;
					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.translationType, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.translationGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						memcpy( &translation.dwTime, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( translation.fTrans, m_pFileData + m_CurrentDataPos, 4 * 3 );
						m_CurrentDataPos += 4 * 3;
						if( matrix.translationType > 1 )
						{
							memcpy( translation.fInTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;
							memcpy( translation.fOutTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;							 
						}
						matrix.translation.push_back(translation);
					}
					break;
				}
			case 'KGRT':
				{
					unsigned int NumTracks;
					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.rotationType, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.rotationGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						memcpy( &rotation.dwTime, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( rotation.fRot, m_pFileData + m_CurrentDataPos, 4 * 4 );
						m_CurrentDataPos += 4 * 4;
						if( matrix.rotationType > 1 )
						{
							memcpy( rotation.fInTan, m_pFileData + m_CurrentDataPos, 4 * 4 );
							m_CurrentDataPos += 4 * 4;
							memcpy( rotation.fOutTan, m_pFileData + m_CurrentDataPos, 4 * 4 );
							m_CurrentDataPos += 4 * 4;
						}
						matrix.rotation.push_back(rotation);
					}
					break;
				}
			case 'KGSC':
				{
					unsigned int NumTracks;
					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.scalingType, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.scalingGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						memcpy( &scaling.dwTime, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( scaling.fScale, m_pFileData + m_CurrentDataPos, 4 * 3 );
						m_CurrentDataPos += 4 * 3;
						if( matrix.scalingType > 1 )
						{
							memcpy( scaling.fInTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;
							memcpy( scaling.fOutTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;
						}
						matrix.scaling.push_back(scaling);
					}
					break;
				}
			default:
				break;
			}
		}

		matrix.objectID = ObjectID;
		matrix.parentID = ParentID;
		mModel->mMatrix.push_back(matrix);

		memcpy( &Tag, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		switch( TAG(Tag) )
		{
		case 'KEVT':
			{
				unsigned int NumTracks;
				unsigned int GlobalSequenceID;
				unsigned int Time;
				memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
				m_CurrentDataPos += 4;
				memcpy( &GlobalSequenceID, m_pFileData + m_CurrentDataPos, 4 );
				m_CurrentDataPos += 4;
				for( unsigned int i = 0; i < NumTracks; i++ )
				{
					memcpy( &Time, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
				}
				break;
			}
		default:
			m_CurrentDataPos -= 4;
		}
		printf( "事件对象名:%s\n", NodeName );
	}
}
void CMDXFile::ReadCamera()
{
	m_CurrentDataPos += m_CurrentChunkSize;
}
void CMDXFile::ReadCollisionShape()
{
	unsigned int OrigPos = m_CurrentDataPos;
	while( m_CurrentDataPos < OrigPos + m_CurrentChunkSize )
	{
		tagBONEMATRIX matrix;
		tagTRANSLATION translation;
		tagROTATION rotation;
		tagSCALING scaling;

		char NodeName[80];
		unsigned int InclusiveSize;
		unsigned int ObjectID;
		unsigned int ParentID;
		unsigned int Flags;
		unsigned int Tag;
		unsigned _OrigPos = m_CurrentDataPos;
		memcpy( &InclusiveSize, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( NodeName, m_pFileData + m_CurrentDataPos, 80 );
		m_CurrentDataPos += 80;
		memcpy( &ObjectID, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &ParentID, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		memcpy( &Flags, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		while( m_CurrentDataPos < _OrigPos + InclusiveSize )
		{
			memcpy( &Tag, m_pFileData + m_CurrentDataPos, 4 );
			m_CurrentDataPos += 4;
			switch( TAG(Tag) )
			{
			case 'KGTR':
				{
					unsigned int NumTracks;
					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.translationType, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.translationGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						memcpy( &translation.dwTime, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( translation.fTrans, m_pFileData + m_CurrentDataPos, 4 * 3 );
						m_CurrentDataPos += 4 * 3;
						if( matrix.translationType > 1 )
						{
							memcpy( translation.fInTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;
							memcpy( translation.fOutTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;							 
						}
						matrix.translation.push_back(translation);
					}
					break;
				}
			case 'KGRT':
				{
					unsigned int NumTracks;
					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.rotationType, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.rotationGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						memcpy( &rotation.dwTime, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( rotation.fRot, m_pFileData + m_CurrentDataPos, 4 * 4 );
						m_CurrentDataPos += 4 * 4;
						if( matrix.rotationType > 1 )
						{
							memcpy( rotation.fInTan, m_pFileData + m_CurrentDataPos, 4 * 4 );
							m_CurrentDataPos += 4 * 4;
							memcpy( rotation.fOutTan, m_pFileData + m_CurrentDataPos, 4 * 4 );
							m_CurrentDataPos += 4 * 4;
						}
						matrix.rotation.push_back(rotation);
					}
					break;
				}
			case 'KGSC':
				{
					unsigned int NumTracks;
					memcpy( &NumTracks, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.scalingType, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					memcpy( &matrix.scalingGlobalSeqID, m_pFileData + m_CurrentDataPos, 4 );
					m_CurrentDataPos += 4;
					for( unsigned int i = 0; i < NumTracks; i++ )
					{
						memcpy( &scaling.dwTime, m_pFileData + m_CurrentDataPos, 4 );
						m_CurrentDataPos += 4;
						memcpy( scaling.fScale, m_pFileData + m_CurrentDataPos, 4 * 3 );
						m_CurrentDataPos += 4 * 3;
						if( matrix.scalingType > 1 )
						{
							memcpy( scaling.fInTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;
							memcpy( scaling.fOutTan, m_pFileData + m_CurrentDataPos, 4 * 3 );
							m_CurrentDataPos += 4 * 3;
						}
						matrix.scaling.push_back(scaling);
					}
					break;
				}
			default:
				break;
			}
		}

		matrix.objectID = ObjectID;
		matrix.parentID = ParentID;
		mModel->mMatrix.push_back(matrix);

		unsigned int Type;
		memcpy( &Type, m_pFileData + m_CurrentDataPos, 4 );
		m_CurrentDataPos += 4;
		unsigned int NumTracks;
		if( Type == 0 )
			NumTracks = 2;
		else if( Type == 2)
			NumTracks = 1;
		else 
			NumTracks = 0;
		float Position[3]; 
		for( unsigned int i = 0; i < NumTracks; i++ )
		{
			memcpy( Position, m_pFileData + m_CurrentDataPos, 4 * 3 );
			m_CurrentDataPos += 4 * 3;
		}
		float BoundingRadius;
		if( Type == 2 )
		{
			memcpy( &BoundingRadius, m_pFileData + m_CurrentDataPos, 4 );
			m_CurrentDataPos += 4;
		}
		printf( "碰撞物名:%s\n", NodeName );
	}

}
void CMDXFile::ParseData()
{
	m_CurrentChunkSize = 0;
	m_CurrentDataPos   = 0;
	m_CurrentDataPos  += 4;

	m_GroupGroups.clear();
	m_MatrixGroups.clear();
	m_MatrixIDGroups.clear();
	m_PivotPoints.clear();

	while( m_CurrentDataPos < m_dwFileSize )
	{
		unsigned int Tag = *(unsigned int*)(m_pFileData + m_CurrentDataPos);
		m_CurrentDataPos += 4;
		m_CurrentChunkSize = *(unsigned int*)(m_pFileData + m_CurrentDataPos);
		m_CurrentDataPos += 4;
		switch( TAG(Tag) )
		{
		case 'VERS':
			ReadVersion();
			break;
		case 'MODL':
			ReadModel();
			break;
		case 'SEQS':
			ReadSequence();
			break;
		case 'GLBS':
			ReadGlobalSequence();
			break;
		case 'MTLS':
			ReadMaterial();
			break;
		case 'TEXS':
			ReadTexture();
			break;
		case 'TXAN':
			ReadTextureAnimation();
			break;
		case 'GEOS':
			ReadGeoset();
			break;
		case 'GEOA':
			ReadGeosetAnimation();
			break;
		case 'BONE':
			ReadBone();
			break;
		case 'LITE':
			ReadLight();
			break;
		case 'HELP':
			ReadHelpher();
			break;
		case 'ATCH':
			ReadAttachment();
			break;
		case 'PIVT':
			ReadPivotPoint();
			break;
		case 'PREM':
			ReadParticleEmitter();
			break;
		case 'PRE2':
			ReadParticleEmitter2();
			break;
		case 'RIBB':
			ReadRibbonEmitter();
			break;
		case 'EVTS':
			ReadEventObject();
			break;
		case 'CAMS':
			ReadCamera();
			break;
		case 'CLID':
			ReadCollisionShape();
			break;
		default:
			m_CurrentDataPos += m_CurrentChunkSize;
		}
	}
	
	int numGroups = m_GroupGroups.size();
	mModel->mMatrixGroups.resize( numGroups );
	for( int i = 0; i < numGroups; i++ )
	{
		int lenth = m_GroupGroups[i].size();
		mModel->mMatrixGroups[i].resize( lenth );
		for( int j = 0; j < lenth; j++ )
		{
			unsigned int index = 0;
			for( int m = 0; m < m_GroupGroups[i][j]; m++ )
			{
				index += m_MatrixGroups[i][m];
			}

			for( int k = 0; k < m_MatrixGroups[i][m_GroupGroups[i][j]]; k++ )
			{
				mModel->mMatrixGroups[i][j].push_back( m_MatrixIDGroups[i][index + k] );
			}
		}
	}
	//FILE* file = fopen( "data/log.txt", "w" );
	//for( int i = 0; i < mModel->mMatrixGroups.size(); i++ )
	//{
	//	for( int j = 0; j < mModel->mMatrixGroups[i].size(); j++ )
	//	{
	//		for( int k = 0; k < mModel->mMatrixGroups[i][j].size(); k++ )
	//			fprintf( file, "%d\t", mModel->mMatrixGroups[i][j][k] );
	//		fprintf( file, "\n" );
	//	}
	//}
	//fclose(file);
}

bool CMDXFile::ParseMdxFileFromResource( const char* lpFileName, CMdxModel* model )
{
	m_dwFileSize = 0;
	mModel		 = model;

	HANDLE hFileHandle = NULL;
	BOOL result = SFileOpenFileEx( m_mpqWar3, lpFileName, 0, &hFileHandle);
	if( !result )
		return false;
	m_dwFileSize = SFileGetFileSize(hFileHandle, NULL);
	if( m_pFileData != NULL )
		delete []m_pFileData;
	m_pFileData = new unsigned char[m_dwFileSize];
	DWORD BytesRead = 0;
	SFileReadFile( hFileHandle, m_pFileData, m_dwFileSize, &BytesRead, NULL);

	ParseData();
	SortMatrix();

	SFileCloseFile( hFileHandle );
	return true;
}

bool CMDXFile::ParseMdxFile( const char* lpFileName, CMdxModel* model )
{
	m_dwFileSize = 0;
	mModel		 = model;

	FILE* pFile = fopen( lpFileName, "rb" );
	if( pFile == NULL )
		return false;
	fseek( pFile, 0, SEEK_END );
	m_dwFileSize = ftell( pFile );
	fseek( pFile, -(int)m_dwFileSize, SEEK_CUR );
	if( m_pFileData != NULL )
		delete []m_pFileData;
	m_pFileData = new unsigned char[m_dwFileSize];
	fread( m_pFileData, sizeof(unsigned char), m_dwFileSize, pFile );
	fclose( pFile );

	ParseData();
	SortMatrix();
	return true;
}

void CMDXFile::SortMatrix()
{	
	mModel->mNumMatrix = mModel->mMatrix.size();
	mModel->mMatrixOrder.resize(mModel->mNumMatrix);
	int counter = 0;

	//FILE* fp = fopen( "log.txt", "w" );
	std::map< unsigned int, int> mapRefer;
	for( int j = 0; j < mModel->mNumMatrix; j++ )
	{
		mapRefer[mModel->mMatrix[j].objectID] = 0;
	}

	for( int j = 0; j < mModel->mNumMatrix; j++ )
	{
		tagBONEMATRIX* p = &mModel->mMatrix[j];
		memcpy( p->pivotPoint, m_PivotPoints[p->objectID].Position, 4 * 3 );
		if( mapRefer[p->objectID] == 1 )
			continue;
		if( p->parentID == 0xFFFFFFFF )
		{
			mapRefer[p->objectID] = 1;
			mModel->mMatrixOrder[counter] = j;
			counter++;
			//fprintf( fp, "%d\t<===>\t%d\n", p->objectID, p->parentID );
			continue;
		}

		std::vector<unsigned int> indexVector;
		int currentIndex = j;
		while( counter < mModel->mNumMatrix )
		{
			bool bFound = false;
			if( p->parentID == 0xFFFFFFFF )
			{
				mapRefer[p->objectID] = 1;
				mModel->mMatrixOrder[counter] = currentIndex;
				counter++;
				//fprintf( fp, "%d\t<===>\t%d\n", p->objectID, p->parentID );			
				int num1 = indexVector.size();
				for( int k = num1 - 1; k > -1; k-- )
				{
					for( int m = 0; m < mModel->mNumMatrix; m++ )
					{
						if( mModel->mMatrix[m].objectID == indexVector[k] )
						{
							mapRefer[indexVector[k]] = 1;
							mModel->mMatrixOrder[counter] = m;
							counter++;
							//fprintf( fp, "%d\t<===>\t%d\n", mModel->mMatrix[m].objectID,
							//	mModel->mMatrix[m].parentID );
							break;
						}
					}
				}
				indexVector.clear();
				break;
			}
			for( int i = 0; i < counter; i++ )
			{
				if( mModel->mMatrix[mModel->mMatrixOrder[i]].objectID == p->parentID )
				{
					mapRefer[p->objectID] = 1;
					mModel->mMatrixOrder[counter] = currentIndex;
					counter++;
					//fprintf( fp, "%d\t<===>\t%d\n", p->objectID, p->parentID );
					bFound = true;
					int num2 = indexVector.size();
					for( int k = num2 - 1; k > -1; k-- )
					{
						for( int m = 0; m < mModel->mNumMatrix; m++ )
						{
							if( mModel->mMatrix[m].objectID == indexVector[k] )
							{
								mapRefer[indexVector[k]] = 1;
								mModel->mMatrixOrder[counter] = m;
								counter++;
								//fprintf( fp, "%d\t<===>\t%d\n", mModel->mMatrix[m].objectID,
								//	mModel->mMatrix[m].parentID );					
								break;
							}
						}
					}
					indexVector.clear();
					break;
				}
			}
			if( !bFound )
			{
				for( int k = 0; k < mModel->mNumMatrix; k++ )
				{
					if( mModel->mMatrix[k].objectID == p->parentID )
					{
						indexVector.push_back(p->objectID);
						p = &mModel->mMatrix[k];
						currentIndex = k;
						break;
					}
				}
			}
			else
				break;
		}
	}
	//fclose( fp );
}
bool CMDXFile::ParseTexture( IMAGEINFO* pImageInfo, const char* lpFileName )
{
	if( pImageInfo == NULL )
		return false;

	HANDLE hFileHandle=NULL;
	unsigned char *buffer=NULL;
	UINT filesize = 0;
	BOOL result=FALSE;
	DWORD BytesRead=0;
	result = SFileOpenFileEx( m_mpqWar3, lpFileName, 0, &hFileHandle);
	if(!result)
		return false;
	filesize = SFileGetFileSize( hFileHandle, NULL );
	buffer = new unsigned char[filesize];

	SFileReadFile( hFileHandle, buffer, filesize, &BytesRead, NULL );

	if( !LoadBlpImage( pImageInfo, buffer, 0) )
	{
		if( pImageInfo->imageData != NULL )
		{
			free( pImageInfo->imageData );
			pImageInfo->imageData = NULL;
		}
		return false;
	}

	delete []buffer;
	SFileCloseFile(hFileHandle);
	return true;
}
bool CMDXFile::IsExist( const char* filename )
{
	HANDLE hFileHandle = NULL;
	BOOL result = SFileOpenFileEx( m_mpqWar3, filename, 0, &hFileHandle);
	if( !result )
		return false;
	
	SFileCloseFile( hFileHandle );

	return true;
}