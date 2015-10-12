// CanvasInterface.cpp

#include "CanvasInterface.h"
#include "Application.h"
#include "Geometry.h"
#include <wx/sizer.h>
#include <GL/gl.h>
#include <GL/glu.h>

namespace Geometer
{
	wxIMPLEMENT_DYNAMIC_CLASS( CanvasInterface, Interface )
}

using namespace Geometer;

// TODO: Expose interface to application's list of geometries.

CanvasInterface::CanvasInterface( void )
{
	canvas = 0;
}

/*virtual*/ CanvasInterface::~CanvasInterface( void )
{
}

/*virtual*/ void CanvasInterface::FillOutRegistryEntry( RegistryEntry& entry )
{
	entry.interfaceMenuItemName = "Canvas";
	entry.interfaceMenuItemHelp = "Toggle the canvas interface.";
}

/*virtual*/ bool CanvasInterface::CreateControls( void )
{
	canvas = new Canvas( this );

	wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
	boxSizer->Add( canvas, 1, wxALL | wxGROW, 0 );
	SetSizer( boxSizer );

	return true;
}

/*virtual*/ bool CanvasInterface::UpdateControls( void )
{
	return true;
}

CanvasInterface::Camera::Camera( void )
{
	fieldOfVisionAngle = 3.1415926536 / 3.0;
	focalDistance = 10.0;
	transform.Identity();
	transform.SetTranslation( 0.0, 0.0, 10.0 );
}

CanvasInterface::Camera::~Camera( void )
{
}

void CanvasInterface::Camera::Pan( const MathLibrary::Vector2D& pan, PanMode mode )
{
	MathLibrary::Vector3D focalPoint = FocalPoint();

	MathLibrary::Vector3D translation = transform.GetXAxis() * pan.GetX() + transform.GetYAxis() * pan.GetY();
	transform.GetTranslation() += translation;

	if( mode == PAN_ABOUT_FFP )
	{
		translation = ( focalPoint - Location() ) * ( 1.0 - focalDistance / ( focalPoint - Location() ).Length() );
		transform.GetTranslation() += translation;

		MathLibrary::Vector3D zAxis = transform.GetZAxis();
		transform.GetZAxis() = Location() - FocalPoint();
		transform.GetZAxis().GetNormalized( zAxis );

		MathLibrary::Scalar angle;
		MathLibrary::Vector3D unitAxis;
		MathLibrary::Vector3D::Rotation( unitAxis, angle, zAxis, transform.GetZAxis() );
		MathLibrary::Vector3D::Rotate( transform.GetXAxis(), transform.GetXAxis(), unitAxis, angle );
		MathLibrary::Vector3D::Rotate( transform.GetYAxis(), transform.GetYAxis(), unitAxis, angle );

		// Account for round-off error.
		transform.GetLinearTransform().Orthonormalize();
	}
}

void CanvasInterface::Camera::Zoom( const MathLibrary::Scalar zoom, ZoomMode mode )
{
	//...
}

void CanvasInterface::Camera::Tilt( const MathLibrary::Scalar tiltAngle, TiltMode mode )
{
	switch( mode )
	{
		case TILT_X_AXIS:
		{
			transform.GetYAxis().Rotate( transform.GetXAxis(), tiltAngle );
			transform.GetZAxis().Rotate( transform.GetXAxis(), tiltAngle );
			break;
		}
		case TILT_Y_AXIS:
		{
			transform.GetXAxis().Rotate( transform.GetYAxis(), tiltAngle );
			transform.GetZAxis().Rotate( transform.GetYAxis(), tiltAngle );
			break;
		}
		case TILT_Z_AXIS:
		{
			transform.GetXAxis().Rotate( transform.GetZAxis(), tiltAngle );
			transform.GetYAxis().Rotate( transform.GetZAxis(), tiltAngle );
			break;
		}
	}

	// Account for round-off error.
	transform.GetLinearTransform().Orthonormalize();
}

const MathLibrary::Vector3D& CanvasInterface::Camera::Location( void ) const
{
	return transform.GetTranslation();
}

MathLibrary::Vector3D CanvasInterface::Camera::FocalPoint( void ) const
{
	return Location() - transform.GetZAxis() * focalDistance;
}

const MathLibrary::Vector3D& CanvasInterface::Camera::UpVector( void ) const
{
	return transform.GetYAxis();
}

MathLibrary::Scalar CanvasInterface::Camera::GetFieldOfVisionAngle( void ) const
{
	return fieldOfVisionAngle;
}

void CanvasInterface::Camera::SetFieldOfVisitionAngle( MathLibrary::Scalar fieldOfVisionAngle )
{
	this->fieldOfVisionAngle = fieldOfVisionAngle;
}

CanvasInterface::Canvas::Canvas( wxWindow* parent ) : wxGLCanvas( parent, wxID_ANY, NULL )
{
	context = 0;

	camera = new Camera();

	Bind( wxEVT_PAINT, &Canvas::OnPaint, this );
	Bind( wxEVT_SIZE, &Canvas::OnSize, this );
	Bind( wxEVT_LEFT_DOWN, &Canvas::OnMouseLeftDown, this );
	Bind( wxEVT_MOTION, &Canvas::OnMouseMotion, this );
}

/*virtual*/ CanvasInterface::Canvas::~Canvas( void )
{
	delete camera;
}

void CanvasInterface::Canvas::BindContext( void )
{
	if( !context )
		context = new wxGLContext( this );

	SetCurrent( *context );
}

void CanvasInterface::Canvas::Render( GLenum mode )
{
	BindContext();

	glClearColor( 0.f, 0.f, 0.f, 1.f );
	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	GLint viewport[4];
	glGetIntegerv( GL_VIEWPORT, viewport );
	double aspectRatio = double( viewport[2] ) / double( viewport[3] );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	if( mode == GL_SELECT )
		gluPickMatrix( GLdouble( mousePos.x ), GLdouble( viewport[3] - mousePos.y - 1 ), 2.0, 2.0, viewport );

	gluPerspective( camera->GetFieldOfVisionAngle() * 180.0 / 3.1415926536, aspectRatio, 0.1, 1000.0 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	MathLibrary::Vector3D eye = camera->Location();
	MathLibrary::Vector3D focalPoint = camera->FocalPoint();
	MathLibrary::Vector3D upVector = camera->UpVector();

	gluLookAt( eye.GetX(), eye.GetY(), eye.GetZ(),
				focalPoint.GetX(), focalPoint.GetY(), focalPoint.GetZ(),
				upVector.GetX(), upVector.GetY(), upVector.GetZ() );

	if( mode == GL_RENDER )
		RenderAxes();

	const GeometryMap& geometryMap = wxGetApp().GetGeometryMap();
	GeometryMap::const_iterator iter = geometryMap.begin();
	while( iter != geometryMap.end() )
	{
		const Geometry* geometry = iter->second;

		if( mode == GL_SELECT )
			glPushName( geometry->SelectionID() );

		geometry->Render( mode );

		if( mode == GL_SELECT )
			glPopName();

		iter++;
	}

	glFlush();

	if( mode == GL_RENDER )
		SwapBuffers();
	else if( mode == GL_SELECT )
	{
		//...
	}
}

void CanvasInterface::Canvas::RenderAxes( void )
{
	glBegin( GL_LINES );

	GLfloat axisLength = 10.f;

	glColor3f( 1.f, 0.f, 0.f );
	glVertex3f( 0.f, 0.f, 0.f );
	glVertex3f( axisLength, 0.f, 0.f );

	glColor3f( 0.f, 1.f, 0.f );
	glVertex3f( 0.f, 0.f, 0.f );
	glVertex3f( 0.f, axisLength, 0.f );

	glColor3f( 0.f, 0.f, 1.f );
	glVertex3f( 0.f, 0.f, 0.f );
	glVertex3f( 0.f, 0.f, axisLength );

	glEnd();
}

void CanvasInterface::Canvas::OnPaint( wxPaintEvent& event )
{
	Render( GL_RENDER );
}

void CanvasInterface::Canvas::OnSize( wxSizeEvent& event )
{
	BindContext();

	wxSize size = event.GetSize();
	glViewport( 0, 0, size.GetWidth(), size.GetHeight() );

	Refresh();
}

void CanvasInterface::Canvas::OnMouseLeftDown( wxMouseEvent& event )
{
	mousePos = event.GetPosition();

	//if( event.ControlDown() ) {...
	//Render( GL_SELECT );
	//Refresh();
}

void CanvasInterface::Canvas::OnMouseMotion( wxMouseEvent& event )
{
}

// CanvasInterface.cpp