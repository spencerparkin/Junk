// MyFirstD3DApp.cpp

// TODO: Fix the mesh changing option to work.

#include <windows.h>
#include <d3d9.h>
#include <d3d9types.h>
#include <d3d9caps.h>
#include <d3d9helper.h>
#include <DirectXMath.h>
//#include <d3d9tex.h>
#include "Camera.h"
#include "Mesh.h"
#include "Light.h"

using namespace DirectX;

static HWND hMainWnd = 0;
static IDirect3D9 *d3d9 = 0;
static IDirect3DDevice9 *pDevice = 0;
static int winWidth = 600;
static int winHeight = 600;
static float aspectRatio = float(winWidth) / float(winHeight);
static bool bAutoRotate = false;
Mesh *g_pMesh = 0;
Camera *g_pCamera = 0;
XMMATRIX gProjMtx;
Light *g_pLight = 0;
ID3DXEffect *g_pEffect = 0;
D3DXHANDLE g_hTechnique = 0;
bool g_bUseEffect = true;
IDirect3DTexture9 *pTexture[2] = { 0, 0 };
IDirect3DTexture9 *pColorRamp = 0;
bool g_bAutoMorphFractal = true;
float radius = 0.65f, angle = 0.0f;
float Xmin = -1.5f, Xmax = 1.5f;
float Ymin = -1.5f, Ymax = 1.5f;

// This function assumes that the aspectRatio has been setup.
void CalcProjectionMatrix( void )
{
	D3DXMatrixPerspectiveFovLH( &gProjMtx, D3DX_PI / 3.0f, ::aspectRatio, 0.1f, 1000.0f );	
}

static void Zoom( float zoom_factor )
{
	float Xdelta, Ydelta;
	float Xcenter, Ycenter;

	// Get half the width and height.
	Xdelta = (Xmax - Xmin) * 0.5f;
	Ydelta = (Ymax - Ymin) * 0.5f;

	// Compute the center.
	Xcenter = Xmin + Xdelta;
	Ycenter = Ymin + Ydelta;

	// Adjust the size by the given factor.
	Xdelta *= zoom_factor;
	Ydelta *= zoom_factor;

	// Recompute the domain and range.
	Xmin = Xcenter - Xdelta;
	Xmax = Xcenter + Xdelta;
	Ymin = Ycenter - Ydelta;
	Ymax = Ycenter + Ydelta;
}

static void Pan( float x_factor, float y_factor )
{
	// Get the width and height.
	float Xdelta = Xmax - Xmin;
	float Ydelta = Ymax - Ymin;

	// Compute the shift factor.
	Xdelta *= x_factor;
	Ydelta *= y_factor;

	// Shift the domain and range.
	Xmin += Xdelta;
	Xmax += Xdelta;
	Ymin += Ydelta;
	Ymax += Ydelta;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	static int mouseX, mouseY;
	static int mouseDX, mouseDY;

	switch( Msg )
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return FALSE;
		}
		case WM_LBUTTONDOWN:
		{
			mouseX = (int)LOWORD( lParam );
			mouseY = (int)HIWORD( lParam );
			break;
		}
		case WM_MOUSEMOVE:
		{
			if( !bAutoRotate && g_pMesh && (wParam & MK_LBUTTON) )
			{
				mouseDX = (int)LOWORD( lParam ) - mouseX;
				mouseDY = (int)HIWORD( lParam ) - mouseY;
				mouseX = (int)LOWORD( lParam );
				mouseY = (int)HIWORD( lParam );	

				g_pMesh->Rotate( float(-mouseDY) * D3DX_PI / 720.0f,
								 float(-mouseDX) * D3DX_PI / 720.0f, 0.0f );
				g_pMesh->CalcWorldMatrix();
			}
			break;
		}
		case WM_KEYDOWN:
		{			
			switch( wParam )
			{
				case VK_ESCAPE:
				{
					DestroyWindow( hMainWnd );
					break;
				}
				case VK_F1:
				case VK_F2:
				case VK_F3:
				case VK_F4:
				case VK_F5:		// BUG: This doesn't work!  Why?
				{
					// What if the mesh is in use?!
					if( g_pMesh )
						delete g_pMesh;
					g_pMesh = 0;					
					break;
				}
				case VK_F6:
				{
					bAutoRotate = bAutoRotate ? false : true;
					break;
				}
				case VK_SPACE:
				{
					g_bAutoMorphFractal = g_bAutoMorphFractal ? false : true;
					break;
				}
				case VK_PRIOR:		Zoom( 0.8f ); break;
				case VK_NEXT:		Zoom( 1.2f ); break;
				case VK_UP:			Pan( 0.0f, 0.2f ); break;
				case VK_DOWN:		Pan( 0.0f, -0.2f ); break;
				case VK_LEFT:		Pan( -0.2f, 0.0f ); break;
				case VK_RIGHT:		Pan( 0.2f, 0.0f ); break;
				case VK_HOME:
				{
					Xmin = Ymin = -1.5f;
					Xmax = Ymax = 1.5f;
					break;
				}
			}
			Mesh::Type type;				
			switch( wParam )
			{
				default:
				case VK_F1: type = Mesh::CUBE; break;
				case VK_F2: type = Mesh::SPHERE; break;
				case VK_F3: type = Mesh::ICOSAHEDRON; break;
				case VK_F4: type = Mesh::DODECAHEDRON; break;
				case VK_F5: type = Mesh::ICOSIDODECAHEDRON; break;
			}
			switch( wParam )
			{
				case VK_F1:
				case VK_F2:
				case VK_F3:
				case VK_F4:
				case VK_F5:
				{
					g_pMesh = new Mesh( type, pDevice );
					break;
				}
			}
			return FALSE;
		}		
		case WM_SIZE:
		{
			// Note that we should get this message upon initialization
			// of the window so that our project matrix gets setup to begin with.
			winWidth = LOWORD( lParam );
			winHeight = HIWORD( lParam );
			aspectRatio = float(winWidth) / float(winHeight);
			CalcProjectionMatrix();
			if( pDevice )
			{
				// Do we really need to do this whenever the matrix changes?
//				pDevice->SetTransform( D3DTS_PROJECTION, &gProjMtx );

				// The render target dimensions are fixed once the
				// device as been created, I think, and this can be
				// used to specify the viewport as the entire render
				// target surface or some subset of it.  So I guess,
				// when the window resizes, the render target is
				// stretched to fit the client area.
//				D3DVIEWPORT9 viewport = { 0, 0, winWidth, winHeight, 0, 1 };
//				pDevice->SetViewport( &viewport );
			}
			return FALSE;
		}
	}

	return DefWindowProc( hWnd, Msg, wParam, lParam );
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASS wc;

	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInst;
	wc.hIcon			= LoadIcon(0, IDI_APPLICATION);
	wc.hCursor			= LoadCursor(0, IDI_APPLICATION);
	wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName		= 0;
	wc.lpszClassName	= "MyApp";

	if( !RegisterClass( &wc ) )
	{
		MessageBox( 0, "Failed to register window class!", 0, 0 );
		return 0;
	}

	hMainWnd = CreateWindow(
					wc.lpszClassName,
					"My freakin' awsome Direct3D App of total awsomeness!",
					WS_OVERLAPPEDWINDOW,
					CW_USEDEFAULT,
					CW_USEDEFAULT,
					winWidth,
					winHeight,
					0,
					0,
					hInst,
					0);

	if( !hMainWnd )
	{
		MessageBox( 0, "Failed to create window!", 0, 0 );
		return 0;
	}

	// Acquare an IDirect3D9 interface.  This is a COM
	// object whose interface can be used to determine
	// information about the graphics hardware.
	d3d9 = Direct3DCreate9( D3D_SDK_VERSION );
	if( !d3d9 )
	{
		MessageBox( 0, "Failed to acquire IDirect3D9 interface!", 0, 0 );
		return 0;
	}

	// Determine the capabilities of the man video card.
	D3DCAPS9 caps;	// A pDevice capabilities structure.
	D3DDEVTYPE deviceType =	D3DDEVTYPE_HAL;
	d3d9->GetDeviceCaps(
					D3DADAPTER_DEFAULT,		// Denotes the primary display adapter.
					deviceType,				// Can be D3DDEVTYPE_HAL or D3DDEVTYPE_REF for hardware or software usage, respectively.
					&caps);					// This gets filled out by the function.

	// Does the pDevice support hardware vertex processing?
	int vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;

	// Can we do vertex shaders?
	int b = 0;

	// Can we do pixel shaders?

	// Fill out a structure that's used to specify the
	// parameters of the IDirect3DDevice9 object we'll create.
	D3DPRESENT_PARAMETERS pres_parms;
	ZeroMemory( &pres_parms, sizeof(pres_parms) );
	pres_parms.BackBufferWidth					= 512;
	pres_parms.BackBufferHeight				= 512;
	pres_parms.BackBufferFormat				= D3DFMT_A8R8G8B8;		// 32-bit pixel format, with 8-bits per component.
	pres_parms.BackBufferCount					= 1;
	pres_parms.MultiSampleType					= D3DMULTISAMPLE_NONE;
	pres_parms.MultiSampleQuality				= 0;
	pres_parms.SwapEffect						= D3DSWAPEFFECT_DISCARD;
	pres_parms.hDeviceWindow					= hMainWnd;
	pres_parms.Windowed							= TRUE;
	pres_parms.EnableAutoDepthStencil		= TRUE;				// If true, this bogs the program down!
	pres_parms.AutoDepthStencilFormat		= D3DFMT_D16;
	pres_parms.Flags								= 0;
	pres_parms.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
	pres_parms.PresentationInterval			= D3DPRESENT_INTERVAL_IMMEDIATE;

	// Try to create the IDirect3DDevice9 object whose
	// interface we'll use to render awesome crap.  I
	// think it's like a drawing context and it also
	// keeps render state for us.
	HRESULT hr = d3d9->CreateDevice(
					D3DADAPTER_DEFAULT,
					deviceType,
					hMainWnd,
					vp,
					&pres_parms,
					&pDevice);	

	if( FAILED(hr) )
	{
		MessageBox( 0, "Failed to acquire IDirect3DDevice9 interface!", 0, 0 );
		return 0;
	}

	// Try to create an effect.
	ID3DXBuffer *pErrorBuf = 0;
	hr = D3DXCreateEffectFromFile(
					pDevice,
					"Effect.fx",
					0,
					0,
					D3DXSHADER_DEBUG,
					0,
					&g_pEffect,
					&pErrorBuf );

	// Did it fail to compile?
	if ( pErrorBuf )
	{
		// Yes.  Explain why.
		MessageBox( 0, LPCSTR( pErrorBuf->GetBufferPointer() ), 0, 0 );
		pErrorBuf->Release();
		return 0;
	}

	// Did we fail to create the effect?
	if ( hr != D3D_OK )
	{
		MessageBox( 0, "Failed to create effect!", 0, 0 );
		return 0;
	}

	// Get a handle on one of the supported techniques of the effect.
	g_hTechnique = g_pEffect->GetTechniqueByName( "MyTech2" );
	if ( !g_hTechnique )
	{
		MessageBox( 0, "Failed to get handle to technique!", 0, 0 );
		return 0;
	}

	// Use this technique.
	hr = g_pEffect->SetTechnique( g_hTechnique );

	// Okay, now show the window.
	ShowWindow( hMainWnd, nShowCmd );
	UpdateWindow( hMainWnd );

	// Setup the camera object.
	g_pCamera = new Camera( pDevice );
	g_pCamera->SetEyePoint( 0.0f, 0.0f, -5.0f );
	g_pCamera->SetLookVector( 0.0f, 0.0f, 1.0f );
	g_pCamera->SetUpVector( 0.0f, 1.0f, 0.0f );
	g_pCamera->CalcViewMatrix();		// Only needs to be done after the camera moves.

	// Create a mesh object to draw.
	g_pMesh = new Mesh( Mesh::QUAD, pDevice, "texture.JPG" );
	g_pMesh->CalcWorldMatrix();
	g_pMesh->SetMaterial(
				D3DXCOLOR( 0.5f, 0.5f, 0.5f, 0.0f ),		// Ambient light reflected.
				D3DXCOLOR( 0.7f, 0.7f, 0.7f, 0.0f ),		// Diffuse light reflected.
				D3DXCOLOR( 0.2f, 0.2f, 0.2f, 0.0f ),		// Specular light reflected.
				D3DXCOLOR( 0.0f, 0.0f, 0.0f, 0.0f ),		// Light emitted.  (Material is not a light source.)
				50.0f );			// Sharpness of specular highlight.  (Like shininess.)

	// TODO: Create a menu that lets you change render state.
	pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
//	pDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
	pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	pDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );		// Sometimes normals get unnormalized after transformations.
	pDevice->SetRenderState( D3DRS_SPECULARENABLE, TRUE );			// Do specular highlights.

	// Create a light object.
	g_pLight = new Light( pDevice, D3DLIGHT_DIRECTIONAL, 0 );
	g_pLight->SetDirection( D3DXVECTOR3( 1.0f, 0.0f, 0.0f ) );
	g_pLight->SetColor(
						D3DXCOLOR( 1.0f, 1.0f, 1.0f, 0.0f ),		// Ambient light emitted.
						D3DXCOLOR( 1.0f, 1.0f, 1.0f, 0.0f ),		// Diffuse light emitted.
						D3DXCOLOR( 1.0f, 1.0f, 1.0f, 0.0f ) );		// Specular light emitted.
	g_pLight->Update();			// Tell the device that the parameters have changed!
	g_pLight->Enable();			// Tell the device to use the light in the pipeline.

	D3DXHANDLE hParm;
	
	if ( D3D_OK != D3DXCreateTextureFromFile( pDevice, "texture0.jpg", &pTexture[0] ) )
	{
		MessageBox( 0, "Failed to create texture zero!" , 0, 0 );
		return 0;
	}

	if ( D3D_OK != D3DXCreateTextureFromFile( pDevice, "texture1.jpg", &pTexture[1] ) )
	{
		MessageBox( 0, "Failed to create texture one!", 0, 0 );
		return 0;
	}

	hParm = g_pEffect->GetParameterByName( 0, LPCSTR( "Tex0" ) );
	hr = g_pEffect->SetTexture( hParm, pTexture[0] );
	hParm = g_pEffect->GetParameterByName( 0, LPCSTR( "Tex1" ) );
	hr = g_pEffect->SetTexture( hParm, pTexture[1] );

	// Compile the texture shader.
	ID3DXBuffer *pTextureShaderBuf = 0;	
	ID3DXConstantTable *pConstantTable = 0;		// Not used by texture shaders.  Instead, the texture shader interface is used to do the same thing.
	if ( D3D_OK != D3DXCompileShaderFromFile(
								"ColorRamp.txt",
								0,
								0,
								"TextureColorRampFill",
								"tx_1_0",
								0,
								&pTextureShaderBuf,
								&pErrorBuf,
								&pConstantTable ) )	// Is there a pDevice->CreateShaderFromFile?
	{
		MessageBox( 0, LPCSTR( pErrorBuf->GetBufferPointer() ), 0, 0 );
		return 0;
	}

	// Create the texture shader.
	ID3DXTextureShader *pTextureShader = 0;
	if ( D3D_OK != D3DXCreateTextureShader( (DWORD *)pTextureShaderBuf->GetBufferPointer(), &pTextureShader ) )
	{
		MessageBox( 0, "Failed to create the texture shader!", 0, 0 );
		return 0;
	}

	// Initialize the constant variables in the shader.
	hr = pTextureShader->SetDefaults();

	// This may not return the format we requested.
	if ( D3D_OK != D3DXCreateTexture( pDevice, 32, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pColorRamp ) )
	{
		MessageBox( 0, "Failed to create color-ramp texture!", 0, 0 );
		return 0;
	}

	// Populate it here using D3DXFillTextureTX with a texture shader.
	if ( D3D_OK != D3DXFillTextureTX( pColorRamp, pTextureShader ) )
	{
		MessageBox( 0, "Failed to populate texture!", 0, 0 );
		return 0;
	}

	// Now that the texture has been created and populated, free what we don't need anymore.
	pTextureShaderBuf->Release();	
	pTextureShader->Release();

	// Let the effect know about the color-ramp texture.
	hParm = g_pEffect->GetParameterByName( 0, LPCSTR( "Tex1" ) );
	hr = g_pEffect->SetTexture( hParm, pColorRamp );

	MSG msg;
	ZeroMemory( &msg, sizeof(MSG) );

	while( msg.message != WM_QUIT )
	{
		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			// When we're not doing Window's bidding, let's
			// go render some awesome crap using Direct3D!

			// But first, in preparation for rendering this
			// frame, clear the back-buffer, depth-buffer,
			// and the stencil buffer.
			pDevice->Clear(
								0,				// Number of rectangles in given array.
								0,				// Array of sub-rectangles of the render-target surface to clear.
								D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,		// Oring of flags to control what gets cleared.
								D3DCOLOR_XRGB(0, 0, 0),		// Pixel color (black) to clear the back-surface with.
								1.0f,			// Depth value to clear the Z-buffer with.
								0 );			// Value to clear the stencil-buffer with.

			// What's this for?  All drawing must be inside a or the begin/end scene calls.
			// Is this done on a per-mesh basis or on a per-frame basis?
			if( SUCCEEDED( pDevice->BeginScene() ) )
			{
				if( g_pMesh )
				{
					if ( g_bUseEffect )
					{
						D3DXMATRIX mtx;
						mtx = g_pMesh->GetWorldMatrix() * g_pCamera->GetViewMatrix();
						hParm = g_pEffect->GetParameterByName( 0, LPCSTR( "norm_transform_mtx" ) );
						hr = g_pEffect->SetMatrix( hParm, &mtx );
						mtx = mtx * gProjMtx;
						hParm = g_pEffect->GetParameterByName( 0, LPCSTR( "pos_transform_mtx" ) );
						hr = g_pEffect->SetMatrix( hParm, &mtx );

						hParm = g_pEffect->GetParameterByName( 0, LPCSTR( "Xmin" ) );
						hr = g_pEffect->SetFloat( hParm, Xmin );
						hParm = g_pEffect->GetParameterByName( 0, LPCSTR( "Xmax" ) );
						hr = g_pEffect->SetFloat( hParm, Xmax );
						hParm = g_pEffect->GetParameterByName( 0, LPCSTR( "Ymin" ) );
						hr = g_pEffect->SetFloat( hParm, Ymin );
						hParm = g_pEffect->GetParameterByName( 0, LPCSTR( "Ymax" ) );
						hr = g_pEffect->SetFloat( hParm, Ymax );

						float C[2] = { radius * cosf(angle), radius * sinf(angle) };
						hParm = g_pEffect->GetParameterByName( 0, LPCSTR( "C" ) );
						hr = g_pEffect->SetFloatArray( hParm, C, 2 );
						if( g_bAutoMorphFractal )
						{
					//		radius += 0.00001f;
							angle += D3DX_PI / 128.0f;
						}
						
						hParm = g_pEffect->GetParameterByName( 0, LPCSTR( "fade_factor" ) );
						hr = g_pEffect->SetFloat( hParm, (1.0f + cosf(angle * 0.1f)) * 0.5f );

						UINT nNumPasses = 0;
						hr = g_pEffect->Begin( &nNumPasses, 0 );

						for( UINT k = 0; k < nNumPasses; k++ )
						{
							hr = g_pEffect->BeginPass( k );
							g_pMesh->Draw();
							hr = g_pEffect->EndPass();
						}

						hr = g_pEffect->End();
					}
					else
						g_pMesh->Draw();

					if( bAutoRotate )
					{
						g_pMesh->Rotate( D3DX_PI / 512.0f, D3DX_PI / 512.0f, 0.0f );
						g_pMesh->CalcWorldMatrix();
					}
				}

				// What's this for?
				pDevice->EndScene();
			}

			// Present the back-buffer.  This should swap the roles
			// played by the front and back buffers, but the process
			// may be more elaborate via the use of swap-chains and
			// page-flippting, whatever those are.
			pDevice->Present(
							0,			// ?
							0,			// ?
							0,			// ?
							0);		// ?
		}
	}

	// Free object we created on the free-store.
	if( g_pCamera )
		delete g_pCamera;
	if( g_pMesh )
		delete g_pMesh;
	if( g_pLight )
		delete g_pLight;

	// Before exiting, don't forget to release our COM objects!
	// Notice that we do this after we delete all the objects
	// we created that depend on the device.	
	g_pEffect->Release();
	pDevice->Release();
	d3d9->Release();

	// Be nice and do this.
	UnregisterClass( wc.lpszClassName, hInst );

	// Always return the wParam of the WM_QUIT message back to Windows.
	return msg.wParam;
}

// endof MyFirstD3DApp.cpp