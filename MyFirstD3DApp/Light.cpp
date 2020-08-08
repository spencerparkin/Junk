// Light.cpp

#include <d3d9.h>
#include "Light.h"

Light::Light( IDirect3DDevice9 *pDevice, D3DLIGHTTYPE type, int index )
{
	ZeroMemory( (void *)this, sizeof(Light) );

	m_pDevice = pDevice;	
	m_Parms.Type = type;
	m_Index = index;

	// Do this initially so that state member
	// functions work.  I still am not quite
	// sure what I'm doing.
	HRESULT hr = m_pDevice->SetLight( m_Index, &m_Parms );
	//ASSERT( hr == D3D_OK );
}

Light::~Light()
{	
}

bool Light::SetPosition( const XMFLOAT3& pos )
{
	// Directional lights are at infinity.
	if( m_Parms.Type == D3DLIGHT_DIRECTIONAL )
		return false;
	m_Parms.Position = pos;
	return true;
}

bool Light::SetDirection( const XMFLOAT3& dir )
{
	// Point-lights don't have direction.
	// They shine in all directions.
	if( m_Parms.Type == D3DLIGHT_POINT )
		return false;
	m_Parms.Direction = dir;
	return true;
}

bool Light::SetFalloff( float falloff )
{
	// Fall-off is only used by spot-lights.
	if( m_Parms.Type != D3DLIGHT_SPOT )
		return false;
	m_Parms.Falloff = falloff;
	return true;
}

void Light::SetColor( const XMFLOAT4& amb,
				const XMFLOAT4& dif,
				const XMFLOAT4& spec )
{
	m_Parms.Ambient = amb;
	m_Parms.Diffuse = dif;
	m_Parms.Specular = spec;
}

bool Light::GetPosition( XMFLOAT3 *pos ) const
{
	// Directional lights are considered to be
	// infinitely far away.
	if( m_Parms.Type == D3DLIGHT_DIRECTIONAL )
		return false;
	if( *pos ) *pos = m_Parms.Position;
	return true;
}

bool Light::GetDirection( XMFLOAT3 *dir ) const
{
	// Point-lights don't have directions.
	if( m_Parms.Type == D3DLIGHT_POINT )
		return false;
	if( *dir ) *dir = m_Parms.Direction;
	return true;
}

bool Light::GetFalloff( float *falloff ) const
{
	// Fall-off is only used by spot-lights.
	if( m_Parms.Type != D3DLIGHT_SPOT )
		return false;
	if( falloff ) *falloff = m_Parms.Falloff;
	return true;
}

void Light::GetColor( XMFLOAT4* amb,
				XMFLOAT4* dif,
				XMFLOAT4* spec ) const
{
	if( amb )	*amb = m_Parms.Ambient;
	if( dif )	*dif = m_Parms.Diffuse;
	if( spec )	*spec = m_Parms.Specular;
}

// These needs to get called after the
// lighting parameters have been setup.
void Light::Update( void )
{
	HRESULT hr = m_pDevice->SetLight( m_Index, &m_Parms );
	//ASSERT( hr == D3D_OK );
}

void Light::Enable( void )
{
	HRESULT hr = m_pDevice->LightEnable( m_Index, TRUE );
	//ASSERT( hr == D3D_OK );
}

void Light::Disable( void )
{
	HRESULT hr = m_pDevice->LightEnable( m_Index, FALSE );
	//ASSERT( hr == D3D_OK );
}

bool Light::IsEnabled( void )
{
	BOOL bIsEnabled;
	m_pDevice->GetLightEnable( m_Index, &bIsEnabled );
	return bIsEnabled ? true : false;
}

// endof Light.cpp