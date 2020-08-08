// Mesh.h

#if !defined(__Mesh_h__)
#define __Mesh_h__

#include <DirectXMath.h>

class Mesh
{
public:

	enum Type
	{
		QUAD,
		CUBE,
		SPHERE,
		ICOSAHEDRON,
		DODECAHEDRON,
		ICOSIDODECAHEDRON,
	};

	Mesh( Type type, IDirect3DDevice9 *pDevice, const char *pTexFilename = 0 );
	~Mesh();

	void Draw( void );

	void SetTranslation( float x, float y, float z );
	void SetRotation( float x, float y, float z );
	void SetScale( float x, float y, float z );

	void GetTranslation( DirectX::XMFLOAT3 *pVec ) const;
	void GetRotation( DirectX::XMFLOAT3 *pVec ) const;
	void GetScale( DirectX::XMFLOAT3 *pVec ) const;

	void Translate( float dx, float dy, float dz );
	void Rotate( float dx, float dy, float dz );

	void CalcWorldMatrix( void );

	void SetMaterial( const DirectX::XMFLOAT4& amb,
					const DirectX::XMFLOAT4& dif,
					const DirectX::XMFLOAT4& spec,
					const DirectX::XMFLOAT4& emis,
					float power );
	void GetMaterial( DirectX::XMFLOAT4* amb,
					DirectX::XMFLOAT4* dif,
					DirectX::XMFLOAT4* spec,
					DirectX::XMFLOAT4* emis,
					float* power );

	// Oops, we're returning an l-value.
	DirectX::XMMATRIX& GetWorldMatrix( void ) { return m_WorldMtx; }

private:

	IDirect3DDevice9 *m_pDevice;
	IDirect3DVertexBuffer9 *m_pVtxBuf;		// A list of vertices making up the mesh object.
	IDirect3DIndexBuffer9 *m_pIdxBuf;		// A list of indices into the vertex buffer.
//	IDirect3DTexture9 *m_pTexture;
	DirectX::XMMATRIX m_WorldMtx;					// Model space -> World space transformation matrix.  Gets composed before shipping vertices to pipeline.
	DirectX::XMFLOAT3 m_Translation;
	DirectX::XMFLOAT3 m_Rotation;					// Used as Euler angles.
	DirectX::XMFLOAT3 m_Scale;
	D3DMATERIAL9 m_Material;
	int m_VtxCount, m_IdxCount;

	// TODO: Use quaternions to orient the mesh object.

public:

	static DWORD VtxFVF;
	static D3DFORMAT IdxFMT;

	struct Vertex
	{
		// Without a constructor, this becomes an "aggregate" type,
		// and so initializor lists can be used on it.
		//Vertex( float _x, float _y, float _z ) { x = _x; y = _y; z = _z; }
		float x, y, z;				// Vertex coordinates.
		float nx, ny, nz;			// Vertex normal.
		float u, v;					// Texture coordinates.
	};
};

#endif //__Mesh_h__

// endof Mesh.h