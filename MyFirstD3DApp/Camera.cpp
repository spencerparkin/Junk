// Camera.cpp

#include <d3d9.h>
#include "Camera.h"

using namespace DirectX;

Camera::Camera( IDirect3DDevice9 *pDevice )
{
	m_pDevice = pDevice;

	SetSubject( 0.0f, 0.0f, 0.0f );
	SetEyePoint( 0.0f, 0.0f, -10.0f );
	SetUpVector( 0.0f, 1.0f, 0.0f );
}

Camera::~Camera()
{
}

XMFLOAT3 *Camera::GetLookVector( XMFLOAT3 *pVec ) const
{
	*pVec = m_SubjectPt - m_EyePt;
	return pVec;
}

void Camera::SetLookVector( float x, float y, float z )
{
	XMFLOAT3 LookVec = XMFLOAT3( x, y, z );
	m_SubjectPt = m_EyePt + LookVec;
}

inline XMFLOAT3 *Camera::GetSubject( XMFLOAT3 *pVec ) const
{
	*pVec = m_SubjectPt;
	return pVec;
}

inline void Camera::SetSubject( float x, float y, float z )
{
	m_SubjectPt = D3DXVECTOR3( x, y, z );
}

inline XMFLOAT3 *Camera::GetEyePoint( XMFLOAT3 *pVec ) const
{
	*pVec = m_EyePt;
	return pVec;
}

inline void Camera::SetEyePoint( float x, float y, float z )
{
	m_EyePt = XMFLOAT3( x, y, z );
}

inline XMFLOAT3 *Camera::GetUpVector( XMFLOAT3 *pVec ) const
{
	*pVec = m_UpVec;
	return pVec;
}

void Camera::SetUpVector( float x, float y, float z )
{
	m_UpVec = XMFLOAT3( x, y, z );
}

void Camera::CalcViewMatrix( void )
{
	XMMatrixLookAtLH( &m_ViewMtx, &m_EyePt, &m_SubjectPt, &m_UpVec );

	// Do we need to do this every time the matrix changes
	// or can we do it once in the constructor?  The former
	// is true if this just does a copy.  The latter is true
	// if this routine has Direct3D keeping a pointer to the
	// matrix we modify.
	//ASSERT( m_pDevice != 0 );
//	m_pDevice->SetTransform( D3DTS_VIEW, &m_ViewMtx );
}

// endof Camera.cpp