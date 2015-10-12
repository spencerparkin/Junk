// CanvasInterface.h

#ifndef __CanvasInterface_h__
#define __CanvasInterface_h__

#include <wx/glcanvas.h>

#include "Interface.h"
#include "MathLibrary.h"

namespace Geometer
{
	class CanvasInterface;
}

class Geometer::CanvasInterface : public Geometer::Interface
{
public:

	wxDECLARE_DYNAMIC_CLASS( CanvasInterface );

	CanvasInterface( void );
	virtual ~CanvasInterface( void );

	virtual void FillOutRegistryEntry( RegistryEntry& entry ) override;
	virtual bool CreateControls( void ) override;
	virtual bool UpdateControls( void ) override;

	class Camera
	{
	public:

		Camera( void );
		~Camera( void );

		// IFP -- Infinite Focal Point
		// FFP -- Finite Focal Point
		enum PanMode { PAN_ABOUT_IFP, PAN_ABOUT_FFP };
		enum ZoomMode { ZOOM_TOWARD_IFP, ZOOM_TOWARD_FFP };
		enum TiltMode { TILT_X_AXIS, TILT_Y_AXIS, TILT_Z_AXIS };

		void Pan( const MathLibrary::Vector2D& pan, PanMode mode );
		void Zoom( const MathLibrary::Scalar zoom, ZoomMode mode );
		void Tilt( const MathLibrary::Scalar tiltAngle, TiltMode mode );

		const MathLibrary::Vector3D& Location( void ) const;
		MathLibrary::Vector3D FocalPoint( void ) const;
		const MathLibrary::Vector3D& UpVector( void ) const;

		MathLibrary::Scalar GetFieldOfVisionAngle( void ) const;
		void SetFieldOfVisitionAngle( MathLibrary::Scalar fieldOfVisionAngle );

	private:

		MathLibrary::Scalar fieldOfVisionAngle;
		MathLibrary::Scalar focalDistance;
		MathLibrary::AffineTransform3D transform;
	};

	class Canvas : public wxGLCanvas
	{
	public:

		Canvas( wxWindow* parent );
		virtual ~Canvas( void );

	private:

		void BindContext( void );

		void Render( GLenum mode );
		void RenderAxes( void );

		void OnPaint( wxPaintEvent& event );
		void OnSize( wxSizeEvent& event );
		void OnMouseLeftDown( wxMouseEvent& event );
		void OnMouseMotion( wxMouseEvent& event );

		Camera* camera;
		wxGLContext* context;
		wxPoint mousePos;
	};

private:

	Canvas* canvas;
};

#endif //__CanvasInterface_h__

// CanvasInterface.h