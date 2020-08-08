// Camera.h

#if !defined(__Camera_h__)
#define __Camera_h__

#include <DirectXMath.h>

class Camera
{
public:
	
	Camera( IDirect3DDevice9 *pDevice );
	~Camera();

	DirectX::XMFLOAT3 *GetLookVector( DirectX::XMFLOAT3 *pVec ) const;
	void SetLookVector( float x, float y, float z );

	inline DirectX::XMFLOAT3 *GetSubject( DirectX::XMFLOAT3 *pVec ) const;
	inline void SetSubject( float x, float y, float z );

	inline DirectX::XMFLOAT3 *GetEyePoint( DirectX::XMFLOAT3 *pVec ) const;
	inline void SetEyePoint( float x, float y, float z );

	inline DirectX::XMFLOAT3 *GetUpVector( DirectX::XMFLOAT3 *pVec ) const;
	inline void SetUpVector( float x, float y, float z );

	void CalcViewMatrix( void );	

	// Oops, we're returning an l-value.
	DirectX::XMMATRIX& GetViewMatrix( void ) { return m_ViewMtx; }

private:

	IDirect3DDevice9 *m_pDevice;
	DirectX::XMFLOAT3 m_EyePt, m_SubjectPt, m_UpVec;
	DirectX::XMMATRIX m_ViewMtx;			// World space -> View space transformation matrix.
};

#endif //__Camera_h__

// endof Camera.h