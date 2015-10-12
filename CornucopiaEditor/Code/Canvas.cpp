// Canvas.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

// TODO: There is a flicker/update bug that occurs when having a canvas window be a child of a scrollable window.
//       I have confirmed that it is fixed by disabling desktop composition in Windows 7.  There is no problem on XP.
//       The online forums show that other people have had this problem too.  There is currently no solution in wxWidgets.

//=================================================================================
BEGIN_EVENT_TABLE( Canvas, wxGLCanvas )

	EVT_ERASE_BACKGROUND( Canvas::OnErase )
	EVT_PAINT( Canvas::OnPaint )
	EVT_SIZE( Canvas::OnResize )
	EVT_MOTION( Canvas::OnMouseMove )

	EVT_LEFT_DOWN( Canvas::OnMouseLeftDown )
	EVT_LEFT_UP( Canvas::OnMouseLeftUp )
	
	EVT_RIGHT_DOWN( Canvas::OnMouseRightDown )
	EVT_RIGHT_UP( Canvas::OnMouseRightUp )
	
	EVT_MOUSEWHEEL( Canvas::OnMouseWheelMove )
	
	EVT_KEY_UP( Canvas::OnKeyUp )
	EVT_KEY_DOWN( Canvas::OnKeyDown )
	EVT_CHAR_HOOK( Canvas::OnCharHook )

END_EVENT_TABLE()

//=================================================================================
Canvas::Canvas( wxWindow* parent, Mode mode ) : wxGLCanvas( parent, wxID_ANY, 0, wxDefaultPosition, wxSize( 400, 400 ), wxBORDER_SUNKEN )
{
	context = 0;

	origin.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	unitXAxis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 );
	unitYAxis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );
	unitZAxis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 );

	axisScale = 1.0;
	gridScale = 1.0;

	hitBuffer = 0;
	hitBufferSize = 0;

	SetMode( mode );
}

//=================================================================================
/*virtual*/ Canvas::~Canvas( void )
{
	if( context )
	{
		delete context;
		context = 0;
	}
}

//=================================================================================
void Canvas::SetMode( Mode mode )
{
	this->mode = mode;

	camera.foviAngle = M_PI / 3.0;

	switch( mode )
	{
		case MODE_VIEW_3D:
		{
			axisScale = 5.0;

			camera.eye.set( c3ga::vectorE3GA::coord_e1_e2_e3, 10.0, 10.0, 10.0 );
			camera.focus.set( origin );
			camera.up.set( unitYAxis );

			break;
		}
		case MODE_VIEW_2D:
		{
			gridScale = 1.0;
			subGridScale = 0.1;
			gridCount = 10;
			subGridCount = 100;

			camera.eye.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 30.0 );
			camera.focus.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
			camera.up.set( unitYAxis );

			break;
		}
	}
}

//=================================================================================
/*virtual*/ void Canvas::OnErase( wxEraseEvent& event )
{
}

//=================================================================================
/*virtual*/ void Canvas::OnPaint( wxPaintEvent& event )
{
	PrepareForRender( GL_RENDER );

	Render( GL_RENDER );

	// TODO: Might consider packaging following code in a routine that the derived class has to call so that they can call it at the desired time.
	if( mode == MODE_VIEW_3D )
		RenderXYZAxes();	// Let axes lines blend with derived class's geometry.
	else if( mode == MODE_VIEW_2D )
	{
		glDisable( GL_LINE_SMOOTH );
		
		double depth = c3ga::norm( camera.eye - camera.focus );

		double alphaMultiplier = ClampedLinearMap( depth, 50.0, 100.0, 1.0, 0.2 );
		glLineWidth( 2.5f );
		RenderXYPlaneGrid( gridScale, gridCount, alphaMultiplier );

		alphaMultiplier = ClampedLinearMap( depth, 2.0, 10.0, 1.0, 0.0 );
		glLineWidth( 1.f );
		RenderXYPlaneGrid( subGridScale, subGridCount, alphaMultiplier );

		glEnable( GL_LINE_SMOOTH );
	}

	FinishRender( GL_RENDER );
}

//=================================================================================
/*static*/ double Canvas::ClampedLinearMap( double value, double start, double finish, double v0, double v1 )
{
	if( value < start )
		value = start;
	else if( value > finish )
		value = finish;
	return v0 + ( ( value - start ) / ( finish - start ) ) * ( v1 - v0 );
}

//=================================================================================
/*virtual*/ void Canvas::Render( GLenum renderMode )
{
	NodeControlPanel* nodeControlPanel = GetAssociatedPanel();
	if( nodeControlPanel )
		nodeControlPanel->CanvasRender( this, renderMode );
}

//=================================================================================
void Canvas::PerformSelection( wxPoint& mousePos )
{
	PrepareForRender( GL_SELECT, &mousePos );

	Render( GL_SELECT );

	FinishRender( GL_SELECT );
}

//=================================================================================
bool Canvas::BindOpenGLContext( void )
{
	if( !context )
		context = new wxGLContext( this );

	SetCurrent( *context );
	return true;
}

// TODO: There is some sort of bug with picking.  Some pixel of a selectable object can't be clicked on to
//       select the object and some pixel not part of the selectable object can be clicked on to select it.
//       Is the picking matrix wrong?

//=================================================================================
void Canvas::PrepareForRender( GLenum renderMode, wxPoint* mousePos /*= 0*/ )
{
	BindOpenGLContext();

	if( HasFocus() )
		glClearColor( 1.f, 1.f, 1.f, 1.f );
	else
		glClearColor( 0.9f, 0.9f, 0.9f, 1.f );
	glLineWidth( 1.f );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LINE_SMOOTH );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glHint( GL_LINE_SMOOTH_HINT, GL_DONT_CARE );
	glShadeModel( GL_SMOOTH );

	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );

	if( renderMode == GL_SELECT )
	{
		hitBufferSize = 512;
		hitBuffer = new unsigned int[ hitBufferSize ];
		glSelectBuffer( hitBufferSize, hitBuffer );

		glRenderMode( GL_SELECT );
		glInitNames();
	}

	wxSize size = GetSize();
	glViewport( 0, 0, size.x, size.y );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	if( renderMode == GL_SELECT )
	{
		wxPoint localStoragePoint;
		if( !mousePos )
		{
			mousePos = &localStoragePoint;
			wxMouseState mouseState = wxGetMouseState();
			*mousePos = mouseState.GetPosition();
			*mousePos = ScreenToClient( *mousePos );	// This doesn't work.  Why?
		}

		GLint viewport[4];
		glGetIntegerv( GL_VIEWPORT, viewport );
		GLdouble x = mousePos->x;
		GLdouble y = GLdouble( viewport[3] ) - GLdouble( mousePos->y );
		GLdouble width = 2.0;
		GLdouble height = 2.0;
		gluPickMatrix( x, y, width, height, viewport );
	}

	float aspectRatio = float( size.x ) / float( size.y );
	gluPerspective( camera.foviAngle * 180.0 / M_PI, aspectRatio, 0.01f, 1000.f );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	GLfloat lightColor0[] = { 0.5f, 0.5f, 0.5f, 1.f };
	GLfloat lightLocation0[] = { 10.f, 10.f, 10.f, 1.f };
	glLightfv( GL_LIGHT0, GL_DIFFUSE, lightColor0 );
	glLightfv( GL_LIGHT0, GL_POSITION, lightLocation0 );

	gluLookAt( camera.eye.get_e1(), camera.eye.get_e2(), camera.eye.get_e3(),
				camera.focus.get_e1(), camera.focus.get_e2(), camera.focus.get_e3(),
				camera.up.get_e1(), camera.up.get_e2(), camera.up.get_e3() );

	GLfloat ambientColor[] = { 0.2f, 0.2f, 0.2f, 1.f };
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, ambientColor );
	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
}

//=================================================================================
void Canvas::FinishRender( GLenum renderMode )
{
	glFlush();

	if( renderMode == GL_SELECT )
	{
		int hitCount = glRenderMode( GL_RENDER );

		NodeControlPanel* nodeControlPanel = GetAssociatedPanel();
		if( nodeControlPanel )
			nodeControlPanel->CanvasSelect( this, hitBuffer, hitBufferSize, hitCount );

		delete[] hitBuffer;
		hitBuffer = 0;
		hitBufferSize = 0;
	}
	else if( renderMode == GL_RENDER )
	{
		SwapBuffers();

		// Tell windows that we're drawn and that there is no need to have us draw again.
		/*		We don't need this, right?  The purpose was to prevent the canvas from being drawing at some frame-rate.
		HWND wnd = ( HWND )GetHWND();
		::ValidateRect( wnd, NULL );
		*/
	}
}

//=================================================================================
/*virtual*/ NodeControlPanel* Canvas::GetAssociatedPanel( void )
{
	NodeControlPanel* nodeControlPanel = 0;
	wxWindow* parent = GetParent();
	while( !nodeControlPanel && parent )
	{
		nodeControlPanel = wxDynamicCast( parent, NodeControlPanel );
		parent = parent->GetParent();
	}
	return nodeControlPanel;
}

//=================================================================================
const Canvas::Camera& Canvas::GetCamera( void )
{
	return camera;
}

//=================================================================================
void Canvas::RenderXYZAxes( void )
{
	Color( 1.f, 0.f, 0.f, 1.f );
	RenderVector( origin, c3ga::gp( unitXAxis, axisScale ), 0.15, 0.5, 15 );
	
	Color( 0.f, 1.f, 0.f, 1.f );
	RenderVector( origin, c3ga::gp( unitYAxis, axisScale ), 0.15, 0.5, 15 );
	
	Color( 0.f, 0.f, 1.f, 1.f );
	RenderVector( origin, c3ga::gp( unitZAxis, axisScale ), 0.15, 0.5, 15 );
}

//=================================================================================
void Canvas::RenderXYPlaneGrid( double scale, int count, double alphaMultiplier )
{
	double xMax = double( count ) * scale;
	double xMin = -xMax;
	double yMax = double( count ) * scale;
	double yMin = -yMax;
	
	// TODO: Should disable anti-aliasing here, because the alpha-blending doesn't work with the crossed lines.

	glBegin( GL_LINES );

	Color( 0.f, 0.f, 0.f, 1.f * alphaMultiplier );
	glVertex2d( xMin, 0.0 );
	glVertex2d( xMax, 0.0 );
	glVertex2d( 0.0, yMin );
	glVertex2d( 0.0, yMax );

	Color( 0.5f, 0.5f, 0.5f, 0.5f * alphaMultiplier );
	for( int index = 1; index <= count; index++ )
	{
		double x = double( index ) * scale;
		double y = double( index ) * scale;

		glVertex2d( xMin, y );
		glVertex2d( xMax, y );
		glVertex2d( xMin, -y );
		glVertex2d( xMax, -y );

		glVertex2d( x, yMin );
		glVertex2d( x, yMax );
		glVertex2d( -x, yMin );
		glVertex2d( -x, yMax );
	}

	glEnd();
}

//=================================================================================
void Canvas::Color( float r, float g, float b, float a )
{
	glColor4f( r, g, b, a );

	GLfloat diffuseColor[] = { r, g, b, a };
	GLfloat specularColor[] = { 0.5f, 0.5f, 0.5f, a };
	GLfloat ambientColor[] = { r, g, b, a };
	GLfloat shininess[] = { 10.f };

	glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuseColor );
	glMaterialfv( GL_FRONT, GL_SPECULAR, specularColor );
	glMaterialfv( GL_FRONT, GL_AMBIENT, ambientColor );
	glMaterialfv( GL_FRONT, GL_SHININESS, shininess );

	glMaterialfv( GL_BACK, GL_DIFFUSE, diffuseColor );
	glMaterialfv( GL_BACK, GL_SPECULAR, specularColor );
	glMaterialfv( GL_BACK, GL_AMBIENT, ambientColor );
	glMaterialfv( GL_BACK, GL_SHININESS, shininess );
}

//=================================================================================
// The arrow ratio is the ratio of the arrow head's length with respect to the
// length of the given vector.  The number of arrow segments translates to the
// resolution of the geometry we draw as the arrow's head.
void Canvas::RenderVector( const c3ga::vectorE3GA& location, const c3ga::vectorE3GA& vector, double arrowRatio, double arrowRadiusScalar, int arrowSegments )
{
	double vectorLength = c3ga::norm( vector );
	c3ga::vectorE3GA unitVector = c3ga::gp( vector, 1.0 / vectorLength );
	c3ga::vectorE3GA headLocation = location + vector;

	// Draw arrow head first so that the stem can blend with it.
	if( arrowSegments > 0 )
	{
		// Calculate a rotor that will rotate about the vector for each segment.
		c3ga::rotorE3GA rotor = c3ga::RotorFromAxisAngle( unitVector, M_PI / double( arrowSegments ) );
		
		// Calculate the vector that we'll rotate about the given vector for each segment.
		c3ga::vectorE3GA orthoVector = OrthoVector( unitVector );
		orthoVector = c3ga::gp( orthoVector, vectorLength * arrowRatio * arrowRadiusScalar );

		// Draw the arrow head as a triangle fan.
		glBegin( GL_TRIANGLE_FAN );
		glNormal3d( unitVector.get_e1(), unitVector.get_e2(), unitVector.get_e3() );
		glVertex3d( headLocation.get_e1(), headLocation.get_e2(), headLocation.get_e3() );
		c3ga::vectorE3GA arrowHeadLocation = location + c3ga::gp( unitVector, vectorLength * ( 1.0 - arrowRatio ) );
		c3ga::vectorE3GA segmentLocation, segmentNormal;
		for( int segment = 0; segment <= arrowSegments; segment++ )
		{
			segmentLocation = arrowHeadLocation + orthoVector;
			segmentNormal = c3ga::unit( orthoVector );
			glNormal3d( segmentNormal.get_e1(), segmentNormal.get_e2(), segmentNormal.get_e3() );
			glVertex3d( segmentLocation.get_e1(), segmentLocation.get_e2(), segmentLocation.get_e3() );
			orthoVector = c3ga::applyUnitVersor( rotor, orthoVector );
		}
		glEnd();
	}

	// Draw a line segment as the stem of the vector.
	glBegin( GL_LINES );
	glNormal3d( unitVector.get_e1(), unitVector.get_e2(), unitVector.get_e3() );
	glVertex3d( location.get_e1(), location.get_e2(), location.get_e3() );
	glVertex3d( headLocation.get_e1(), headLocation.get_e2(), headLocation.get_e3() );
	glEnd();
}

//=================================================================================
void Canvas::RenderBivector( const c3ga::vectorE3GA& location, const c3ga::bivectorE3GA& bivector, double arrowScale, int arrowCount, int diskSegments )
{
	double area = c3ga::norm( bivector );
	if( area <= 0.0 )
		return;
	
	double diskRadius = sqrt( area / M_PI );
	c3ga::vectorE3GA normal = c3ga::gp( bivector * c3ga::I3, -1.0 / area );
	c3ga::rotorE3GA rotor = c3ga::RotorFromAxisAngle( normal, M_PI / double( diskSegments ) );
	c3ga::vectorE3GA orthoVector = c3ga::gp( c3ga::OrthoVector( normal ), diskRadius );

	glBegin( GL_TRIANGLE_FAN );
	glNormal3d( normal.get_e1(), normal.get_e2(), normal.get_e3() );
	glVertex3d( location.get_e1(), location.get_e2(), location.get_e3() );
	c3ga::vectorE3GA segmentLocation;
	for( int segment = 0; segment <= diskSegments; segment++ )
	{
		segmentLocation = location + orthoVector;
		glNormal3d( normal.get_e1(), normal.get_e2(), normal.get_e3() );
		glVertex3d( segmentLocation.get_e1(), segmentLocation.get_e2(), segmentLocation.get_e3() );
		orthoVector = c3ga::applyUnitVersor( rotor, orthoVector );
	}
	glEnd();

	rotor = c3ga::RotorFromAxisAngle( normal, M_PI / double( arrowCount ) );
	orthoVector = c3ga::gp( c3ga::OrthoVector( normal ), diskRadius );

	c3ga::vectorE3GA segmentVector;
	for( int segment = 0; segment <= arrowCount; segment++ )
	{
		segmentLocation = location + orthoVector;
		segmentVector = c3ga::gp( c3ga::op( normal, orthoVector ) * c3ga::I3, -arrowScale );
		RenderVector( segmentLocation, segmentVector, 0.15, 0.5, 4 );
		orthoVector = c3ga::applyUnitVersor( rotor, orthoVector );
	}
}

//=================================================================================
void Canvas::RenderSphere( const c3ga::vectorE3GA& location, double radius, int subDivisionCount /*= 2*/ )
{
	double gr = ( 1.0 + sqrt( 5.0 ) ) / 2.0;

	c3ga::vectorE3GA vertex[12];

	vertex[0].set( c3ga::vectorE3GA::coord_e1_e2_e3, -1.0, 0.0, gr );
	vertex[1].set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, gr );
	vertex[2].set( c3ga::vectorE3GA::coord_e1_e2_e3, -1.0, 0.0, -gr );
	vertex[3].set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, -gr );

	vertex[4].set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, gr, 1.0 );
	vertex[5].set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, gr, -1.0 );
	vertex[6].set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, -gr, 1.0 );
	vertex[7].set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, -gr, -1.0 );

	vertex[8].set( c3ga::vectorE3GA::coord_e1_e2_e3, gr, 1.0, 0.0 );
	vertex[9].set( c3ga::vectorE3GA::coord_e1_e2_e3, -gr, 1.0, 0.0 );
	vertex[10].set( c3ga::vectorE3GA::coord_e1_e2_e3, gr, -1.0, 0.0 );
	vertex[11].set( c3ga::vectorE3GA::coord_e1_e2_e3, -gr, -1.0, 0.0 );

	for( int index = 0; index < 12; index++ )
		vertex[ index ] = c3ga::unit( vertex[ index ] );

	c3ga::mv dilationVersor = ( c3ga::no - 0.5 * radius * c3ga::ni ) * ( c3ga::no - 0.5 * c3ga::ni );
	c3ga::mv translationVersor = 1.0 - 0.5 * location * c3ga::ni;
	c3ga::mv versor = translationVersor * dilationVersor;
	c3ga::evenVersor evenVersor;
	evenVersor.set( versor );	// This is NOT a unit-versor, but we will apply it as such, because we want to quickly calculate VXV~.

	int face[20][3] =
	{
		{ 1, 4, 0 }, { 4, 9, 0 }, { 4, 5, 9 }, { 8, 5, 4 }, { 1, 8, 4 },
		{ 1, 10, 8 }, { 10, 3, 8 }, { 8, 3, 5 }, { 3, 2, 5 }, { 3, 7, 2 },
		{ 3, 10, 7 }, { 10, 6, 7 }, { 6, 11, 7 }, { 6, 0, 11 }, { 6, 1, 0 },
		{ 10, 1, 6 }, { 11, 0, 9 }, { 2, 11, 9 }, { 5, 2, 9 }, { 11, 2, 7 },
	};

	glBegin( GL_TRIANGLES );
	for( int faceIndex = 0; faceIndex < 20; faceIndex++ )
	{
		int* vertexIndex = face[ faceIndex ];
		RenderSphereTriangle( vertex[ vertexIndex[0] ], vertex[ vertexIndex[1] ], vertex[ vertexIndex[2] ], evenVersor, subDivisionCount );
	}
	glEnd();
}

//=================================================================================
void Canvas::RenderSphereTriangle( const c3ga::vectorE3GA& point0, const c3ga::vectorE3GA& point1, const c3ga::vectorE3GA& point2, const c3ga::evenVersor& evenVersor, int subDivisionCount )
{
	if( subDivisionCount == 0 )
	{
		c3ga::vectorE3GA normal0, normal1, normal2;

		// The vertices of the unit sphere at origin or also the vertex normals.
		normal0 = point0;
		normal1 = point1;
		normal2 = point2;

		c3ga::normalizedPoint cp0 = c3ga::normalizedPoint( c3ga::normalizedPoint::coord_e1_e2_e3_ni, point0.get_e1(), point0.get_e2(), point0.get_e3(), c3ga::norm2( point0 ) );
		c3ga::normalizedPoint cp1 = c3ga::normalizedPoint( c3ga::normalizedPoint::coord_e1_e2_e3_ni, point1.get_e1(), point1.get_e2(), point1.get_e3(), c3ga::norm2( point1 ) );
		c3ga::normalizedPoint cp2 = c3ga::normalizedPoint( c3ga::normalizedPoint::coord_e1_e2_e3_ni, point2.get_e1(), point2.get_e2(), point2.get_e3(), c3ga::norm2( point2 ) );
		
		// Each of these dual spheres has a radius of zero, so they're really points in space.
		// Notice that while the given versor is NOT a unit-versor, I am applying it here as
		// such, because doing so gives us the normalized dual sphere in the end.
		c3ga::dualSphere ds0 = c3ga::applyUnitVersor( evenVersor, cp0 );
		c3ga::dualSphere ds1 = c3ga::applyUnitVersor( evenVersor, cp1 );
		c3ga::dualSphere ds2 = c3ga::applyUnitVersor( evenVersor, cp2 );

		glNormal3d( normal0.get_e1(), normal0.get_e2(), normal0.get_e3() );
		glVertex3d( ds0.get_e1(), ds0.get_e2(), ds0.get_e3() );

		glNormal3d( normal1.get_e1(), normal1.get_e2(), normal1.get_e3() );
		glVertex3d( ds1.get_e1(), ds1.get_e2(), ds1.get_e3() );

		glNormal3d( normal2.get_e1(), normal2.get_e2(), normal2.get_e3() );
		glVertex3d( ds2.get_e1(), ds2.get_e2(), ds2.get_e3() );
	}
	else
	{
		c3ga::vectorE3GA point01, point12, point20;

		point01 = c3ga::unit( c3ga::Lerp( point0, point1, 0.5 ) );
		point12 = c3ga::unit( c3ga::Lerp( point1, point2, 0.5 ) );
		point20 = c3ga::unit( c3ga::Lerp( point2, point0, 0.5 ) );

		subDivisionCount--;
		RenderSphereTriangle( point0, point01, point20, evenVersor, subDivisionCount );
		RenderSphereTriangle( point1, point12, point01, evenVersor, subDivisionCount );
		RenderSphereTriangle( point2, point20, point12, evenVersor, subDivisionCount );
		RenderSphereTriangle( point01, point12, point20, evenVersor, subDivisionCount );
	}
}

//=================================================================================
/*virtual*/ void Canvas::OnResize( wxSizeEvent& event )
{
	// Compensate for the live-resize failure of the AUI manager class.
	Refresh();
}

//=================================================================================
void Canvas::MouseDrag( wxMouseEvent& event, wxPoint* mouseDelta /*= 0*/ )
{
	wxPoint curMousePos = event.GetPosition();
	if( mouseDelta )
		*mouseDelta = curMousePos - lastMousePos;
	lastMousePos = curMousePos;
}

//=================================================================================
/*virtual*/ void Canvas::OnMouseMove( wxMouseEvent& event )
{
	// We actually don't need this if-statement for our algorithm
	// to be correct here, but there's no need to do any work if
	// no button is currently down.
	if( !event.LeftIsDown() )
		return;

	wxPoint mouseDelta;
	MouseDrag( event, &mouseDelta );

	c3ga::vectorE3GA oldLookVector = camera.focus - camera.eye;
	double oldFocalDistance = c3ga::norm( oldLookVector );
	bool maintainFocalDistance = false;
	c3ga::vectorE3GA eyeDelta( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	c3ga::vectorE3GA focusDelta( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );

	// Determine how we'll move the focus and/or the eye.
	if( event.LeftIsDown() )
	{
		// The alt-key in any more helps us zoom in and out.
		if( event.AltDown() )
		{
			double zoomFactor = double( mouseDelta.y ) * 0.01;
			eyeDelta = c3ga::gp( oldLookVector, zoomFactor );
		}
		else
		{
			// Calculate the panning axes based upon where the camera is currently looking.
			c3ga::vectorE3GA panXAxis = c3ga::unit( c3ga::CrossProduct( oldLookVector, camera.up ) );
			c3ga::vectorE3GA panYAxis = c3ga::unit( camera.up );

			// How much would we pan in each dimension?
			double panFactor = 0.2;
			if( mode == MODE_VIEW_2D )
				panFactor *= oldFocalDistance / 30.0;
			double panInX = double( mouseDelta.x ) * -panFactor;
			double panInY = double( mouseDelta.y ) * panFactor;

			// Defer to the mode to figure out how to move the camera and/or eye.
			switch( mode )
			{
				case MODE_VIEW_3D:
				{
					// Move just the eye and maintain the focal distance.
					eyeDelta = c3ga::gp( panXAxis, panInX ) + c3ga::gp( panYAxis, panInY );
					maintainFocalDistance = true;
					break;
				}
				case MODE_VIEW_2D:
				{
					// Move the eye and focal point together.
					eyeDelta = focusDelta = c3ga::gp( panXAxis, panInX ) + c3ga::gp( panYAxis, panInY );
					break;
				}
			}
		}
	}

	// Move the eye and focal points.
	camera.eye += eyeDelta;
	camera.focus += focusDelta;

	// Bring the eye toward or away from the focus point to maintain the focal distance.
	c3ga::vectorE3GA newLookVector = camera.focus - camera.eye;
	if( maintainFocalDistance )
	{
		double newFocalDistance = c3ga::norm( newLookVector );
		newLookVector = c3ga::gp( newLookVector, oldFocalDistance / newFocalDistance );
		camera.eye = camera.focus - newLookVector;
	}

	// Recalculate the camera's up vector based upon the Y-axis.
	camera.up = c3ga::lc( newLookVector, c3ga::op( newLookVector, unitYAxis ) );
	camera.up = c3ga::unit( camera.up );

	// Request a redraw of the canvas.
	Refresh();
}

//=================================================================================
/*virtual*/ void Canvas::OnMouseLeftDown( wxMouseEvent& event )
{
	MouseDrag( event );

	NodeControlPanel* nodeControlPanel = GetAssociatedPanel();
	nodeControlPanel->Highlight();

	SetFocus();
	Refresh();
}

//=================================================================================
/*virtual*/ void Canvas::OnMouseLeftUp( wxMouseEvent& event )
{
}

//=================================================================================
/*virtual*/ void Canvas::OnMouseRightDown( wxMouseEvent& event )
{
	SetFocus();
	Refresh();
}

//=================================================================================
/*virtual*/ void Canvas::OnMouseRightUp( wxMouseEvent& event )
{
}

//=================================================================================
/*virtual*/ void Canvas::OnMouseWheelMove( wxMouseEvent& event )
{
}

//=================================================================================
/*virtual*/ void Canvas::OnKeyUp( wxKeyEvent& event )
{
}

//=================================================================================
/*virtual*/ void Canvas::OnKeyDown( wxKeyEvent& event )
{
}

//=================================================================================
/*virtual*/ void Canvas::OnCharHook( wxKeyEvent& event )
{
}

//=================================================================================
GLuint Canvas::CreateTexture( const wxString& textureFile, wxBitmapType type /*= wxBITMAP_TYPE_ANY*/ )
{
	// Try to load the texture data.
	wxImage image;
	if( !wxGetApp().GetResourceManager()->LoadImage( image, textureFile, type ) )
		return GL_INVALID_VALUE;

	// Make sure that our OpenGL context is bound.
	if( !BindOpenGLContext() )
		return GL_INVALID_VALUE;

	// Try to create an OpenGL texture.
	GLuint textureName;
	glGenTextures( 1, &textureName );
	if( textureName != GL_INVALID_VALUE )
	{
		// The texture dimensions are set on initial binding.
		glBindTexture( GL_TEXTURE_2D, textureName );

		// Setup wrapping mode and filters.
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

		// Texel rows in our texture image data are 1-byte aligned.
		glPixelStorei( GL_PACK_ALIGNMENT, 1 );

		// Feed the texture data to OpenGL.  It will create its own storage for the texture data.
		unsigned char* data = image.GetData();
		GLenum format = image.HasAlpha() ? GL_RGBA : GL_RGB;
		GLsizei width = image.GetWidth();
		GLsizei height = image.GetHeight();
		//glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data );
		gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA, width, height, format, GL_UNSIGNED_BYTE, data );
	}

	// Return the texture to the caller.
	return textureName;
}

//=================================================================================
void Canvas::DestroyTexture( GLuint textureName )
{
	if( textureName != GL_INVALID_VALUE && BindOpenGLContext() )
		glDeleteTextures( 1, &textureName );
}

// Canvas.cpp