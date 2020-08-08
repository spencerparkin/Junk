// Mesh.cpp

// TODO: Add subdivision feature?

// TODO: A better mesh class would be able to take a vertex-list as
//       input and then generate a tri-list from it in the form of
//       the convex-hull.  I'm not sure I would want to program this
//       using the left-handed coordinate system and D3DX stuff.

#include <d3d9.h>
#include "Mesh.h"
#include "Data.h"

using namespace DirectX;

DWORD Mesh::VtxFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
D3DFORMAT Mesh::IdxFMT = D3DFMT_INDEX16;

Mesh::Mesh( Type type, IDirect3DDevice9 *pDevice, const char *pTexFilename )
{
	m_pDevice = pDevice;

	m_VtxCount = 0;
	m_IdxCount = 0;

	switch( type )
	{
		case QUAD:				m_VtxCount = QuadVtxListLen;				break;
		case CUBE:				m_VtxCount = CubeVtxListLen;				break;
		case SPHERE:			m_VtxCount = SphereVtxListLen;				break;
		case ICOSAHEDRON:		m_VtxCount = IcosahedronVtxListLen;			break;
		case DODECAHEDRON:		m_VtxCount = DodecahedronVtxListLen;		break;
		case ICOSIDODECAHEDRON:	m_VtxCount = IcosidodecahedronVtxListLen;	break;
	}

	switch( type )
	{
		case QUAD:				m_IdxCount = 3 * QuadIdxListLen;				break;
		case CUBE:				m_IdxCount = 3 * CubeIdxListLen;				break;
		case SPHERE:			m_IdxCount = 3 * SphereIdxListLen;				break;
		case ICOSAHEDRON:		m_IdxCount = 3 * IcosahedronIdxListLen;			break;
		case DODECAHEDRON:		m_IdxCount = 3 * DodecahedronIdxListLen;		break;
		case ICOSIDODECAHEDRON:	m_IdxCount = 3 * IcosidodecahedronIdxListLen;	break;
	}

	//ASSERT( m_IdxCount % 3 == 0 );

	if( FAILED( m_pDevice->CreateVertexBuffer(
					m_VtxCount * sizeof(Vertex),
					0,			// Is a static buffer since D3DUSAGE_DYNAMIC bit not set.
					VtxFVF,
					D3DPOOL_MANAGED,
					&m_pVtxBuf,
					0 ) ) )
	{
		// Assert here.
		m_pVtxBuf = 0;
		return;
	}

	if( FAILED( m_pDevice->CreateIndexBuffer(
					m_IdxCount * sizeof(WORD),		// Should be 16-bits.
					0,			// Is a static buffer since D3DUSAGE_DYNAMIC bit not set.
					IdxFMT,
					D3DPOOL_MANAGED,
					&m_pIdxBuf,
					0 ) ) )
	{
		// Assert here.
		m_pIdxBuf = 0;
		return;
	}
	
	Vertex *vtxList = 0;	
	WORD *idxList = 0;

	switch( type )
	{
		default:
		case QUAD:					vtxList = QuadVtxList;					idxList = (WORD *)QuadIdxList;					break;
		case CUBE:					vtxList = CubeVtxList;					idxList = (WORD *)CubeIdxList;					break;			
		case SPHERE:				vtxList = SphereVtxList;				idxList = (WORD *)SphereIdxList;				break;
		case ICOSAHEDRON:			vtxList = IcosahedronVtxList;			idxList = (WORD *)IcosahedronIdxList;			break;
		case DODECAHEDRON:			vtxList = DodecahedronVtxList;			idxList = (WORD *)DodecahedronIdxList;			break;
		case ICOSIDODECAHEDRON: 	vtxList = IcosidodecahedronVtxList;		idxList = (WORD *)IcosidodecahedronIdxList;		break;		
	}

	// Copy in the vertex data.
	Vertex *pVtx = 0;
	if( D3D_OK == m_pVtxBuf->Lock( 0, 0, (void **)&pVtx, 0 ) )
	{
		for( int k = 0; k < m_VtxCount; k++ )
		{
			pVtx[k] = vtxList[k];

			// The vertex normal is easily computed as
			// the normalized vector corresponding to the
			// vertex coordinates.
			XMFLOAT3 vec = D3DXVECTOR3( pVtx[k].x, pVtx[k].y, pVtx[k].z );
			D3DXVec3Normalize( &vec, &vec );
			pVtx[k].nx = vec.x;
			pVtx[k].ny = vec.y;
			pVtx[k].nz = vec.z;

			// These texture coordinate work for the cube, but the
			// results will be unsightly for other polyhedra.
			switch( k % 4 )
			{
				case 0: pVtx[k].u = 0.0f; pVtx[k].v = 1.0f; break;
				case 1: pVtx[k].u = 0.0f; pVtx[k].v = 0.0f; break;
				case 2: pVtx[k].u = 1.0f; pVtx[k].v = 0.0f; break;
				case 3: pVtx[k].u = 1.0f; pVtx[k].v = 1.0f; break;
			}
		}

		m_pVtxBuf->Unlock();
	}

	// Copy in the index data.
	WORD *pIdx = 0;
	if( D3D_OK == m_pIdxBuf->Lock( 0, 0, (void **)&pIdx, 0 ) )
	{
		for( int k = 0; k < m_IdxCount; k++ )	
			pIdx[k] = idxList[k];
		m_pIdxBuf->Unlock();
	}

	// Set default world transformation.
	SetTranslation( 0.0f, 0.0f, 0.0f );
	SetRotation( 0.0f, 0.0f, 0.0f );
	SetScale( 1.0f, 1.0f, 1.0f );

#if 0
	if( pTexFilename && D3D_OK == D3DXCreateTextureFromFile( m_pDevice, pTexFilename, &m_pTexture ) )
	{
		m_pDevice->SetTexture( 0, m_pTexture );
		m_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		m_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		m_pDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
	}
	else	
		m_pTexture = 0;
#endif
}

Mesh::~Mesh()
{
	if( m_pVtxBuf )
		m_pVtxBuf->Release();

	if( m_pIdxBuf )
		m_pIdxBuf->Release();

//	if( m_pTexture )
//		m_pTexture->Release();
}

void Mesh::SetTranslation( float x, float y, float z )
{
	m_Translation = XMFLOAT3( x, y, z );
}

void Mesh::SetRotation( float x, float y, float z )
{
	m_Rotation = XMFLOAT3( x, y, z );
}

void Mesh::SetScale( float x, float y, float z )
{
	m_Scale = XMFLOAT3( x, y, z );
}

void Mesh::GetTranslation( D3DXVECTOR3 *pVec ) const
{
	*pVec = m_Translation;
}

void Mesh::GetRotation( D3DXVECTOR3 *pVec ) const
{
	*pVec = m_Rotation;
}

void Mesh::GetScale( D3DXVECTOR3 *pVec ) const
{
	*pVec = m_Scale;
}

void Mesh::Translate( float dx, float dy, float dz )
{
	m_Translation += XMFLOAT3( dx, dy, dz );
}

void Mesh::Rotate( float dx, float dy, float dz )
{
	m_Rotation += XMFLOAT3( dx, dy, dz );
}

// This should get called after setting up the
// translation, rotation, scale, etc., and before
// the mesh gets rendered.
void Mesh::CalcWorldMatrix( void )
{
	// Compose the world matrix to perform the
	// scaling, rotation, and transformation,
	// to the mesh in that order.

	XMMATRIX tMat, rMatX, rMatY, rMatZ, sMat;

	XMMatrixTranslation( &tMat, m_Translation.x, m_Translation.y, m_Translation.z );
	XMMatrixRotationX( &rMatX, m_Rotation.x );
	XMMatrixRotationY( &rMatY, m_Rotation.y );
	XMMatrixRotationZ( &rMatZ, m_Rotation.z );
	XMMatrixScaling( &sMat, m_Scale.x, m_Scale.y, m_Scale.z );

	// Remember that we're using a left-handed coordinate system.
	m_WorldMtx = sMat * rMatX * rMatY * rMatZ * tMat;	
}

// References have the advantage here that we can
// pass in what appear to be immediate values and
// still do pass-by-reference, which is faster.
// Pass-by-value for structures is slow.
void Mesh::SetMaterial( const D3DXCOLOR& amb,
						const D3DXCOLOR& dif,
						const D3DXCOLOR& spec,
						const D3DXCOLOR& emis,
						float power )
{
	m_Material.Ambient = amb;
	m_Material.Diffuse = dif;
	m_Material.Specular = spec;
	m_Material.Emissive = emis;
	m_Material.Power = power;
}

// I think that pointers are better here because
// we can pass null pointers for values we don't
// want to be returned.  References must always
// get an alias, I think.
void Mesh::GetMaterial( D3DXCOLOR* amb,
						D3DXCOLOR* dif,
						D3DXCOLOR* spec,
						D3DXCOLOR* emis,
						float* power )
{
	if( amb )	*amb = m_Material.Ambient;
	if( dif )	*dif = m_Material.Diffuse;
	if( spec )	*spec = m_Material.Specular;
	if( emis )	*emis = m_Material.Emissive;
	if( power )	*power = m_Material.Power;
}

// Here we assume that we're inside of a begin/end scene block.
// We also assume here that the world matrix has been calculated.
void Mesh::Draw( void )
{
	//ASSERT( m_pDevice );

	HRESULT hr;

	// I believe that some of these set calls won't matter
	// if the mesh is being drawn using an effect with a
	// vertex shader, because the shader will by-pass/replace
	// the transformation stages of the graphics pipeline.

//	hr = m_pDevice->SetTransform( D3DTS_WORLD, &m_WorldMtx );

//	hr = m_pDevice->SetMaterial( &m_Material );

//	if( m_pTexture )
//		hr = m_pDevice->SetTexture( 0, m_pTexture );

	hr = m_pDevice->SetStreamSource( 0, m_pVtxBuf, 0, sizeof(Vertex) );

	hr = m_pDevice->SetFVF( VtxFVF );

	hr = m_pDevice->SetIndices( m_pIdxBuf );

	hr = m_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_VtxCount, 0, m_IdxCount / 3 );
}

// endof Mesh.cpp