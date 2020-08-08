// Light.h

#if !defined(__Light_h__)
#define __Light_h__

#include <DirectXMath.h>

class Light
{
public:

	Light( IDirect3DDevice9 *pDevice, D3DLIGHTTYPE type, int index );
	~Light();

	bool SetPosition( const DirectX::XMFLOAT3& pos );
	bool SetDirection( const DirectX::XMFLOAT3& dir );
	bool SetFalloff( float falloff );
	void SetColor( const DirectX::XMFLOAT3& amb,
					const DirectX::XMFLOAT3& dif,
					const DirectX::XMFLOAT3& spec );

	bool GetPosition( DirectX::XMFLOAT3 *pos ) const;
	bool GetDirection( DirectX::XMFLOAT3 *dir ) const;
	bool GetFalloff( float *falloff ) const;
	void GetColor( DirectX::XMFLOAT4* amb,
					DirectX::XMFLOAT4* dif,
					DirectX::XMFLOAT4* spec ) const;

	void Update( void );
	void Enable( void );
	void Disable( void );
	bool IsEnabled( void );

private:

	D3DLIGHT9 m_Parms;
	DWORD m_Index;
	IDirect3DDevice9 *m_pDevice;
};

#endif //__Light_h__

// endof Light.h