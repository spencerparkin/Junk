// ConeNodeControlPanel.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

wxIMPLEMENT_DYNAMIC_CLASS( ConeNodeControlPanel, VecOrBivecNodeControlPanel )

//=================================================================================
ConeNodeControlPanel::ConeNodeControlPanel( wxWindow* parent, Cornucopia::Node* node ) : VecOrBivecNodeControlPanel( parent, node )
{
	coneAngleSlider = 0;
	coneAngleText = 0;
}

//=================================================================================
/*virtual*/ ConeNodeControlPanel::~ConeNodeControlPanel( void )
{
}

//=================================================================================
/*virtual*/ bool ConeNodeControlPanel::PushNodeDataToInterface( void )
{
	VecOrBivecNodeControlPanel::PushNodeDataToInterface();

	Cornucopia::ConeNode* coneNode = ( Cornucopia::ConeNode* )node;

	double coneAngle = coneNode->GetConeAngle();
	double lerp = coneAngle / M_PI;
	double sliderMin = coneAngleSlider->GetMin();
	double sliderMax = coneAngleSlider->GetMax();
	double sliderValue = sliderMin + lerp * ( sliderMax - sliderMin );

	coneAngleSlider->SetValue( int( sliderValue ) );

	coneAngleText->Clear();
	coneAngleText->AppendText( wxString::Format( wxT( "%d" ), coneAngleSlider->GetValue() ) );

	return true;
}

//=================================================================================
/*virtual*/ bool ConeNodeControlPanel::PullNodeDataFromInterface( void )
{
	VecOrBivecNodeControlPanel::PullNodeDataFromInterface();

	double sliderMin = coneAngleSlider->GetMin();
	double sliderMax = coneAngleSlider->GetMax();
	double sliderValue = coneAngleSlider->GetValue();
	double lerp = ( sliderValue - sliderMin ) / ( sliderMax - sliderMin );
	double coneAngle = lerp * M_PI;
	if( coneAngle < 0.0 )
		coneAngle = 0.0;
	if( coneAngle > M_PI )
		coneAngle = M_PI;

	Cornucopia::ConeNode* coneNode = ( Cornucopia::ConeNode* )node;
	coneNode->SetConeAngle( coneAngle );

	coneAngleText->Clear();
	coneAngleText->AppendText( wxString::Format( wxT( "%d" ), coneAngleSlider->GetValue() ) );

	return true;
}

//=================================================================================
/*virtual*/ void ConeNodeControlPanel::BuildUserInterface( void )
{
	VecOrBivecNodeControlPanel::BuildUserInterface();

	wxPanel* coneAnglePanel = new wxPanel( this );
	wxStaticText* coneAngleLabel = new wxStaticText( coneAnglePanel, wxID_ANY, wxT( "Cone Angle:" ), wxDefaultPosition, wxSize( 70, -1 ), wxALIGN_RIGHT );
	coneAngleSlider = new wxSlider( coneAnglePanel, wxID_ANY, 0, 0, 180, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL /*| wxSL_VALUE_LABEL*/ );
	coneAngleText = new wxTextCtrl( coneAnglePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 30, -1 ), wxTE_READONLY | wxTE_RIGHT );
	wxBoxSizer* coneAnglePanelSizer = new wxBoxSizer( wxHORIZONTAL );
	coneAnglePanelSizer->Add( coneAngleLabel, 0, wxALL, 0 );
	coneAnglePanelSizer->Add( coneAngleSlider, 1, wxEXPAND );
	coneAnglePanelSizer->Add( coneAngleText, 0, wxALL, 0 );
	coneAnglePanel->SetSizer( coneAnglePanelSizer );

	wxSizer* panelSizer = GetSizer();
	panelSizer->Add( coneAnglePanel, 0, wxGROW | wxALL, 0 );

	coneAngleSlider->Bind( wxEVT_SCROLL_THUMBTRACK, &ConeNodeControlPanel::OnThumbScrub, this );
	coneAngleSlider->Bind( wxEVT_SCROLL_THUMBRELEASE, &ConeNodeControlPanel::OnThumbRelease, this );
	coneAngleSlider->Bind( wxEVT_SCROLL_PAGEUP, &ConeNodeControlPanel::OnPageUp, this );
	coneAngleSlider->Bind( wxEVT_SCROLL_PAGEDOWN, &ConeNodeControlPanel::OnPageDown, this );
	coneAngleSlider->Bind( wxEVT_SCROLL_LINEUP, &ConeNodeControlPanel::OnLineUp, this );
	coneAngleSlider->Bind( wxEVT_SCROLL_LINEDOWN, &ConeNodeControlPanel::OnLineDown, this );
}

//=================================================================================
void ConeNodeControlPanel::DrawCone( Canvas* canvas, int triangleCount )
{
	canvas->Color( 0.5f, 0.5f, 0.5f, 0.2f );

	Cornucopia::ConeNode* coneNode = ( Cornucopia::ConeNode* )node;
	c3ga::vectorE3GA vector = coneNode->GetVector();
	double length = c3ga::norm( vector );
	c3ga::vectorE3GA unitVector = c3ga::gp( vector, 1.0 / length );
	c3ga::vectorE3GA orthoVector = c3ga::OrthoVector( unitVector );

	double halfAngle = M_PI / double( triangleCount );
	c3ga::rotorE3GA rotor = c3ga::RotorFromAxisAngle( unitVector, halfAngle );

	double coneAngle = coneNode->GetConeAngle();
	double baseRadius = length * sin( coneAngle );
	double coneLength = sqrt( length * length - baseRadius * baseRadius );
	if( coneAngle > M_PI / 2.0 )
		coneLength = -coneLength;

	c3ga::vectorE3GA baseCenter = c3ga::gp( unitVector, coneLength );

	glBegin( GL_TRIANGLES );
	
	for( int index = 0; index < triangleCount; index++ )
	{
		c3ga::vectorE3GA baseVertex = baseCenter + c3ga::gp( orthoVector, baseRadius );
		orthoVector = c3ga::applyUnitVersor( rotor, orthoVector );
		c3ga::vectorE3GA nextBaseVertex = baseCenter + c3ga::gp( orthoVector, baseRadius );

		c3ga::vectorE3GA normal = c3ga::unit( c3ga::CrossProduct( nextBaseVertex, baseVertex ) );
		glNormal3d( normal.get_e1(), normal.get_e2(), normal.get_e3() );

		glVertex3d( 0.0, 0.0, 0.0 );
		glVertex3d( nextBaseVertex.get_e1(), nextBaseVertex.get_e2(), nextBaseVertex.get_e3() );
		glVertex3d( baseVertex.get_e1(), baseVertex.get_e2(), baseVertex.get_e3() );
	}

	glEnd();
}

//=================================================================================
/*virtual*/ void ConeNodeControlPanel::CanvasRender( Canvas* canvas, GLenum renderMode )
{
	Cornucopia::ConeNode* coneNode = ( Cornucopia::ConeNode* )node;
		
	if( coneNode->GetConeAngle() < M_PI / 2.0 )
		DrawConeSide( GL_BACK );
	else
		DrawConeSide( GL_FRONT );

	VecOrBivecNodeControlPanel::CanvasRender( canvas, renderMode );

	if( coneNode->GetConeAngle() < M_PI / 2.0 )
		DrawConeSide( GL_FRONT );
	else
		DrawConeSide( GL_BACK );
}

//=================================================================================
void ConeNodeControlPanel::DrawConeSide( int side )
{
	const int triangleCount = 32;
	glEnable( GL_CULL_FACE );
	glFrontFace( GL_CCW );
	if( side == GL_BACK )
		glCullFace( GL_FRONT );
	else if( side == GL_FRONT )
		glCullFace( GL_BACK );
	DrawCone( canvas, triangleCount );
	glDisable( GL_CULL_FACE );
}

// ConeNodeControlPanel.cpp