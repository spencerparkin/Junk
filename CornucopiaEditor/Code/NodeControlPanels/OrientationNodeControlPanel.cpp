// OrientationNodeControlPanel.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

wxIMPLEMENT_DYNAMIC_CLASS( OrientationNodeControlPanel, NodeControlPanel )

//=================================================================================
OrientationNodeControlPanel::OrientationNodeControlPanel( wxWindow* parent, Cornucopia::Node* node ) : NodeControlPanel( parent, node )
{
	eulerAngleOrder = c3ga::EULER_XYZ;

	canvas = 0;

	xAngleSlider = 0;
	yAngleSlider = 0;
	zAngleSlider = 0;

	xAngleText = 0;
	yAngleText = 0;
	zAngleText = 0;

	eulerAngleOrderComboBox = 0;

	textureNX = GL_INVALID_VALUE;
	texturePX = GL_INVALID_VALUE;

	textureNY = GL_INVALID_VALUE;
	texturePY = GL_INVALID_VALUE;

	textureNZ = GL_INVALID_VALUE;
	texturePZ = GL_INVALID_VALUE;
}

//=================================================================================
/*virtual*/ OrientationNodeControlPanel::~OrientationNodeControlPanel( void )
{
	// The OpenGL context can't be made current while the panel is going down.
	// Will the texture be automatically cleaned up when the OpenGL context goes away?
	/*
	canvas->DestroyTexture( textureNX );
	canvas->DestroyTexture( textureNY );
	canvas->DestroyTexture( textureNZ );

	canvas->DestroyTexture( texturePX );
	canvas->DestroyTexture( texturePY );
	canvas->DestroyTexture( texturePZ );
	*/
}

//=================================================================================
/*virtual*/ bool OrientationNodeControlPanel::PushNodeDataToInterface( void )
{
	PushNodeToSliders();
	UpdateAngleText();
	canvas->Refresh();
	return true;
}

//=================================================================================
/*virtual*/ bool OrientationNodeControlPanel::PullNodeDataFromInterface( void )
{
	PullNodeFromSliders();
	return true;
}

//=================================================================================
void OrientationNodeControlPanel::PushNodeToSliders( void )
{
	Cornucopia::OrientationNode* orientationNode = ( Cornucopia::OrientationNode* )node;

	double xAngle, yAngle, zAngle;
	c3ga::rotorE3GA rotor = orientationNode->GetRotor();	// This should be a unit-rotor.
	if( !c3ga::UnitRotorToEulerAngles( rotor, xAngle, yAngle, zAngle, eulerAngleOrder ) )
	{
		wxASSERT( false );
	}

	xAngle *= 180.0 / M_PI;
	yAngle *= 180.0 / M_PI;
	zAngle *= 180.0 / M_PI;

	xAngleSlider->SetValue( int( xAngle ) );
	yAngleSlider->SetValue( int( yAngle ) );
	zAngleSlider->SetValue( int( zAngle ) );
}

//=================================================================================
void OrientationNodeControlPanel::PullNodeFromSliders( void )
{
	double xAngle = xAngleSlider->GetValue() * M_PI / 180.0;
	double yAngle = yAngleSlider->GetValue() * M_PI / 180.0;
	double zAngle = zAngleSlider->GetValue() * M_PI / 180.0;

	c3ga::rotorE3GA rotor = c3ga::UnitRotorFromEulerAngles( xAngle, yAngle, zAngle, eulerAngleOrder );
	
	Cornucopia::OrientationNode* orientationNode = ( Cornucopia::OrientationNode* )node;
	orientationNode->SetRotor( rotor );
}

//=================================================================================
void OrientationNodeControlPanel::UpdateAngleText( void )
{
	int xAngle = xAngleSlider->GetValue();
	int yAngle = yAngleSlider->GetValue();
	int zAngle = zAngleSlider->GetValue();

	xAngleText->Clear();
	xAngleText->AppendText( wxString::Format( "%d", xAngle ) );
	yAngleText->Clear();
	yAngleText->AppendText( wxString::Format( "%d", yAngle ) );
	zAngleText->Clear();
	zAngleText->AppendText( wxString::Format( "%d", zAngle ) );
}

//=================================================================================
/*virtual*/ void OrientationNodeControlPanel::CanvasRender( Canvas* canvas, GLenum renderMode )
{
	GLfloat boxVertex[8][3] =
	{
		{ -1.f, -1.f, -1.f },		// 0
		{ 1.f, -1.f, -1.f },		// 1
		{ -1.f, 1.f, -1.f },		// 2
		{ 1.f, 1.f, -1.f },			// 3
		{ -1.f, -1.f, 1.f },		// 4
		{ 1.f, -1.f, 1.f },			// 5
		{ -1.f, 1.f, 1.f },			// 6
		{ 1.f, 1.f, 1.f },			// 7
	};

	// These should be wound correctly and consistently.
	int boxFace[6][4] =
	{
		{ 0, 4, 6, 2 },		// -X
		{ 5, 1, 3, 7 },		// +X
		{ 0, 1, 5, 4 },		// -Y
		{ 6, 7, 3, 2 },		// +Y
		{ 1, 0, 2, 3 },		// -Z
		{ 4, 5, 7, 6 },		// +Z
	};

	GLfloat texCoord[4][2] =
	{
		{ 0.f, 1.f },
		{ 1.f, 1.f },
		{ 1.f, 0.f },
		{ 0.f, 0.f },
	};

	GLfloat normal[8][3] =
	{
		{ -1.f, 0.f, 0.f },		// -X
		{ 1.f, 0.f, 0.f },		// +X
		{ 0.f, -1.f, 0.f },		// -Y
		{ 0.f, 1.f, 0.f },		// +Y
		{ 0.f, 0.f, -1.f },		// -Z
		{ 0.f, 0.f, 1.f },		// +Z
	};

	GLfloat faceColor[6][3] =
	{
		{ 1.f, 0.5f, 0.5f },		// -X
		{ 1.f, 0.5f, 0.5f },		// +X
		{ 0.5f, 1.f, 0.5f },		// -Y
		{ 0.5f, 1.f, 0.5f },		// +Y
		{ 0.5f, 0.5f, 1.f },		// -Z
		{ 0.5f, 0.5f, 1.f },		// +Z
	};

	int textureName[6] =
	{
		textureNX,
		texturePX,
		textureNY,
		texturePY,
		textureNZ,
		texturePZ,
	};

	// Draw our box in the orientation being authored by this node control panel.
	double scaleFactor = 2.5;
	Cornucopia::OrientationNode* orientationNode = ( Cornucopia::OrientationNode* )node;
	c3ga::rotorE3GA rotor = orientationNode->GetRotor();
	rotor = c3ga::gp( rotor, sqrt( scaleFactor ) );
	glEnable( GL_CULL_FACE );
	glFrontFace( GL_CCW );
	glCullFace( GL_BACK );
	glEnable( GL_TEXTURE_2D );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	for( int face = 0; face < 6; face++ )
	{
		GLfloat* color = faceColor[ face ];
		canvas->Color( color[0], color[1], color[2], 1.f );
		glBindTexture( GL_TEXTURE_2D, textureName[ face ] );
		glBegin( GL_QUADS );
		GLfloat* normalData = normal[ face ];
		c3ga::vectorE3GA normalVec;
		normalVec.set( c3ga::vectorE3GA::coord_e1_e2_e3, normalData[0], normalData[1], normalData[2] );
		normalVec = c3ga::applyVersor( rotor, normalVec );		// This will apply the rotor without scaling the unit-length normal vector.
		glNormal3f( normalVec.get_e1(), normalVec.get_e2(), normalVec.get_e3() );
		int* faceVertices = boxFace[ face ];
		for( int vertex = 0; vertex < 4; vertex++ )
		{
			glTexCoord2fv( texCoord[ vertex ] );
			GLfloat* pointData = boxVertex[ faceVertices[ vertex ] ];
			c3ga::vectorE3GA point;
			point.set( c3ga::vectorE3GA::coord_e1_e2_e3, pointData[0], pointData[1], pointData[2] );
			point = c3ga::applyUnitVersor( rotor, point );		// The versor is not of unit-magnitude, but I want the math this performs.
			glVertex3f( point.get_e1(), point.get_e2(), point.get_e3() );
		}
		glEnd();
	}
	glDisable( GL_TEXTURE_2D );
}

//=================================================================================
/*virtual*/ void OrientationNodeControlPanel::BuildUserInterface( void )
{
	canvas = new OrientationCanvas( this, Canvas::MODE_VIEW_3D );

	wxPanel* xAnglePanel = new wxPanel( this );
	wxStaticText* xAngleLabel = new wxStaticText( xAnglePanel, wxID_ANY, wxT( "X-axis angle:" ), wxDefaultPosition, wxSize( 80, -1 ), wxALIGN_RIGHT );
	xAngleSlider = new wxSlider( xAnglePanel, ID_XAngleSlider, 0, 0, 360, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	xAngleText = new wxTextCtrl( xAnglePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40, -1 ), wxTE_READONLY | wxTE_RIGHT );
	wxBoxSizer* xAnglePanelSizer = new wxBoxSizer( wxHORIZONTAL );
	xAnglePanelSizer->Add( xAngleLabel, 0, wxALL, 0 );
	xAnglePanelSizer->Add( xAngleSlider, 1, wxEXPAND );
	xAnglePanelSizer->Add( xAngleText, 0, wxALL, 0 );
	xAnglePanel->SetSizer( xAnglePanelSizer );

	wxPanel* yAnglePanel = new wxPanel( this );
	wxStaticText* yAngleLabel = new wxStaticText( yAnglePanel, wxID_ANY, wxT( "Y-axis angle:" ), wxDefaultPosition, wxSize( 80, -1 ), wxALIGN_RIGHT );
	yAngleSlider = new wxSlider( yAnglePanel, ID_YAngleSlider, 0, 0, 360, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	yAngleText = new wxTextCtrl( yAnglePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40, -1 ), wxTE_READONLY | wxTE_RIGHT );
	wxBoxSizer* yAnglePanelSizer = new wxBoxSizer( wxHORIZONTAL );
	yAnglePanelSizer->Add( yAngleLabel, 0, wxALL, 0 );
	yAnglePanelSizer->Add( yAngleSlider, 1, wxEXPAND );
	yAnglePanelSizer->Add( yAngleText, 0, wxALL, 0 );
	yAnglePanel->SetSizer( yAnglePanelSizer );

	wxPanel* zAnglePanel = new wxPanel( this );
	wxStaticText* zAngleLabel = new wxStaticText( zAnglePanel, wxID_ANY, wxT( "Z-axis angle:" ), wxDefaultPosition, wxSize( 80, -1 ), wxALIGN_RIGHT );
	zAngleSlider = new wxSlider( zAnglePanel, ID_ZAngleSlider, 0, 0, 360, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	zAngleText = new wxTextCtrl( zAnglePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40, -1 ), wxTE_READONLY | wxTE_RIGHT );
	wxBoxSizer* zAnglePanelSizer = new wxBoxSizer( wxHORIZONTAL );
	zAnglePanelSizer->Add( zAngleLabel, 0, wxALL, 0 );
	zAnglePanelSizer->Add( zAngleSlider, 1, wxEXPAND );
	zAnglePanelSizer->Add( zAngleText, 0, wxALL, 0 );
	zAnglePanel->SetSizer( zAnglePanelSizer );

	wxString eulerAngleOrderChoices[] =
	{
		wxT( "X-Y-Z" ),
		wxT( "X-Z-Y" ),
		wxT( "Y-X-Z" ),
		wxT( "Y-Z-X" ),
		wxT( "Z-X-Y" ),
		wxT( "Z-Y-X" ),
	};

	int choiceCount = sizeof( eulerAngleOrderChoices ) / sizeof( wxString );

	eulerAngleOrderComboBox = new wxComboBox( this, wxID_ANY, eulerAngleOrderChoices[0], wxDefaultPosition, wxDefaultSize, choiceCount, eulerAngleOrderChoices, wxCB_READONLY );

	wxBoxSizer* panelSizer = new wxBoxSizer( wxVERTICAL );
	panelSizer->Add( canvas, 1, wxGROW | wxALL, 5 );
	panelSizer->Add( xAnglePanel, 0, wxGROW | wxALL, 0 );
	panelSizer->Add( yAnglePanel, 0, wxGROW | wxALL, 0 );
	panelSizer->Add( zAnglePanel, 0, wxGROW | wxALL, 0 );
	panelSizer->Add( eulerAngleOrderComboBox, 0, wxGROW | wxUP, 5 );
	SetSizer( panelSizer );

	eulerAngleOrderComboBox->Bind( wxEVT_COMMAND_COMBOBOX_SELECTED, &OrientationNodeControlPanel::OnComboBoxSelect, this );

	xAngleSlider->Bind( wxEVT_SCROLL_THUMBTRACK, &OrientationNodeControlPanel::OnThumbScrub, this );
	xAngleSlider->Bind( wxEVT_SCROLL_THUMBRELEASE, &OrientationNodeControlPanel::OnThumbRelease, this );
	xAngleSlider->Bind( wxEVT_SCROLL_PAGEUP, &OrientationNodeControlPanel::OnPageUp, this );
	xAngleSlider->Bind( wxEVT_SCROLL_PAGEDOWN, &OrientationNodeControlPanel::OnPageDown, this );
	xAngleSlider->Bind( wxEVT_SCROLL_LINEUP, &OrientationNodeControlPanel::OnLineUp, this );
	xAngleSlider->Bind( wxEVT_SCROLL_LINEDOWN, &OrientationNodeControlPanel::OnLineDown, this );

	yAngleSlider->Bind( wxEVT_SCROLL_THUMBTRACK, &OrientationNodeControlPanel::OnThumbScrub, this );
	yAngleSlider->Bind( wxEVT_SCROLL_THUMBRELEASE, &OrientationNodeControlPanel::OnThumbRelease, this );
	yAngleSlider->Bind( wxEVT_SCROLL_PAGEUP, &OrientationNodeControlPanel::OnPageUp, this );
	yAngleSlider->Bind( wxEVT_SCROLL_PAGEDOWN, &OrientationNodeControlPanel::OnPageDown, this );
	yAngleSlider->Bind( wxEVT_SCROLL_LINEUP, &OrientationNodeControlPanel::OnLineUp, this );
	yAngleSlider->Bind( wxEVT_SCROLL_LINEDOWN, &OrientationNodeControlPanel::OnLineDown, this );

	zAngleSlider->Bind( wxEVT_SCROLL_THUMBTRACK, &OrientationNodeControlPanel::OnThumbScrub, this );
	zAngleSlider->Bind( wxEVT_SCROLL_THUMBRELEASE, &OrientationNodeControlPanel::OnThumbRelease, this );
	zAngleSlider->Bind( wxEVT_SCROLL_PAGEUP, &OrientationNodeControlPanel::OnPageUp, this );
	zAngleSlider->Bind( wxEVT_SCROLL_PAGEDOWN, &OrientationNodeControlPanel::OnPageDown, this );
	zAngleSlider->Bind( wxEVT_SCROLL_LINEUP, &OrientationNodeControlPanel::OnLineUp, this );
	zAngleSlider->Bind( wxEVT_SCROLL_LINEDOWN, &OrientationNodeControlPanel::OnLineDown, this );

	textureNX = canvas->CreateTexture( "Art\\nx.png", wxBITMAP_TYPE_PNG );
	texturePX = canvas->CreateTexture( "Art\\px.png", wxBITMAP_TYPE_PNG );

	textureNY = canvas->CreateTexture( "Art\\ny.png", wxBITMAP_TYPE_PNG );
	texturePY = canvas->CreateTexture( "Art\\py.png", wxBITMAP_TYPE_PNG );

	textureNZ = canvas->CreateTexture( "Art\\nz.png", wxBITMAP_TYPE_PNG );
	texturePZ = canvas->CreateTexture( "Art\\pz.png", wxBITMAP_TYPE_PNG );
}

//=================================================================================
void OrientationNodeControlPanel::OnComboBoxSelect( wxCommandEvent& event )
{
	wxString selection = eulerAngleOrderComboBox->GetValue();

	if( selection == wxT( "X-Y-Z" ) )
		eulerAngleOrder = c3ga::EULER_XYZ;
	else if( selection == wxT( "X-Z-Y" ) )
		eulerAngleOrder = c3ga::EULER_XZY;
	else if( selection == wxT( "Y-X-Z" ) )
		eulerAngleOrder = c3ga::EULER_YXZ;
	else if( selection == wxT( "Y-Z-X" ) )
		eulerAngleOrder = c3ga::EULER_YZX;
	else if( selection == wxT( "Z-X-Y" ) )
		eulerAngleOrder = c3ga::EULER_ZXY;
	else if( selection == wxT( "Z-Y-X" ) )
		eulerAngleOrder = c3ga::EULER_ZYX;

	PushNodeDataToInterface();
}

//=================================================================================
void OrientationNodeControlPanel::OnThumbScrub( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( false );
	UpdateAngleText();
	canvas->Refresh();
}

//=================================================================================
void OrientationNodeControlPanel::OnThumbRelease( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
	UpdateAngleText();
	canvas->Refresh();
}

//=================================================================================
void OrientationNodeControlPanel::OnPageUp( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
	UpdateAngleText();
	canvas->Refresh();
}

//=================================================================================
void OrientationNodeControlPanel::OnPageDown( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
	UpdateAngleText();
	canvas->Refresh();
}

//=================================================================================
void OrientationNodeControlPanel::OnLineUp( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
	UpdateAngleText();
	canvas->Refresh();
}

//=================================================================================
void OrientationNodeControlPanel::OnLineDown( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
	UpdateAngleText();
	canvas->Refresh();
}

//=================================================================================
OrientationNodeControlPanel::OrientationCanvas::OrientationCanvas( wxWindow* parent, Canvas::Mode mode ) : Canvas( parent, mode )
{
	rotationScrubInProgress = false;
}

//=================================================================================
/*virtual*/ OrientationNodeControlPanel::OrientationCanvas::~OrientationCanvas( void )
{
}

//=================================================================================
/*virtual*/ void OrientationNodeControlPanel::OrientationCanvas::OnMouseMove( wxMouseEvent& event )
{
	if( !event.ControlDown() )
		Canvas::OnMouseMove( event );
	else if( event.LeftIsDown() )
	{
		wxPoint mouseDelta;
		MouseDrag( event, &mouseDelta );

		const Camera& camera = GetCamera();
		c3ga::vectorE3GA yAxis = c3ga::unit( camera.up );
		c3ga::vectorE3GA xAxis = c3ga::unit( c3ga::CrossProduct( camera.focus - camera.eye, camera.up ) );
		c3ga::vectorE3GA zAxis = c3ga::CrossProduct( xAxis, yAxis );
		
		double sensativityFactor = 0.005;
		c3ga::rotorE3GA rotation;
		if( event.ShiftDown() )
		{
			double zAxisHalfAngle = -double( mouseDelta.y ) * sensativityFactor;
			rotation = c3ga::RotorFromAxisAngle( zAxis, zAxisHalfAngle );
		}
		else
		{
			double xAxisHalfAngle = double( mouseDelta.y ) * sensativityFactor;
			double yAxisHalfAngle = double( mouseDelta.x ) * sensativityFactor;
			rotation = c3ga::RotorFromAxisAngle( yAxis, yAxisHalfAngle ) * c3ga::RotorFromAxisAngle( xAxis, xAxisHalfAngle );
		}

		NodeControlPanel* nodeControlPanel = GetAssociatedPanel();
		if( nodeControlPanel )
		{
			OrientationNodeControlPanel* orientationNodeControlPanel = wxDynamicCast( nodeControlPanel, OrientationNodeControlPanel );
			if( orientationNodeControlPanel )
			{
				rotationScrubInProgress = true;
				orientationNodeControlPanel->ApplyRotation( rotation );
				orientationNodeControlPanel->OnMouseRotationScrubHappening();
				Refresh();
			}
		}
	}
}

//=================================================================================
void OrientationNodeControlPanel::OnMouseRotationScrubHappening( void )
{
	PushNodeToSliders();
	UpdateAngleText();
	SynchronizeTreePanelControls( false );
}

//=================================================================================
void OrientationNodeControlPanel::ApplyRotation( const c3ga::rotorE3GA& rotation )
{
	Cornucopia::OrientationNode* orientationNode = ( Cornucopia::OrientationNode* )node;
	c3ga::rotorE3GA rotor = orientationNode->GetRotor();
	rotor = rotation * rotor;
	rotor = c3ga::unit( rotor );	// Re-normalize to account for accumulated round-off error.
	orientationNode->SetRotor( rotor );
}

//=================================================================================
/*virtual*/ void OrientationNodeControlPanel::OrientationCanvas::OnMouseLeftDown( wxMouseEvent& event )
{
	Canvas::OnMouseLeftDown( event );
}

//=================================================================================
/*virtual*/ void OrientationNodeControlPanel::OrientationCanvas::OnMouseLeftUp( wxMouseEvent& event )
{
	Canvas::OnMouseLeftUp( event );

	if( rotationScrubInProgress )
	{
		NodeControlPanel* nodeControlPanel = GetAssociatedPanel();
		if( nodeControlPanel )
		{
			OrientationNodeControlPanel* orientationNodeControlPanel = wxDynamicCast( nodeControlPanel, OrientationNodeControlPanel );
			if( orientationNodeControlPanel )
				orientationNodeControlPanel->OnMouseRotationScrubFinished();
		}

		rotationScrubInProgress = false;
	}
}

//=================================================================================
void OrientationNodeControlPanel::OnMouseRotationScrubFinished( void )
{
	PushNodeToSliders();
	SynchronizeTreePanelControls( true );
}

//=================================================================================
/*virtual*/ void OrientationNodeControlPanel::OrientationCanvas::OnMouseRightDown( wxMouseEvent& event )
{
	Canvas::OnMouseRightDown( event );
}

// OrientationNodeControlPanel.cpp