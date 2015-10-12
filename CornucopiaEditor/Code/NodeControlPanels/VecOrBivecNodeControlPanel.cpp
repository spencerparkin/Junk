// VecOrBivecNodeControlPanel.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

wxIMPLEMENT_DYNAMIC_CLASS( VecOrBivecNodeControlPanel, NodeControlPanel )

// TODO: I think there may still be some synchronization bugs between this panel and the data view control.

//=================================================================================
VecOrBivecNodeControlPanel::VecOrBivecNodeControlPanel( wxWindow* parent, Cornucopia::Node* node ) : NodeControlPanel( parent, node )
{
	canvas = 0;

	longitudeSlider = 0;
	latitudeSlider = 0;
	magnitudeSlider = 0;

	longitudeText = 0;
	latitudeText = 0;
	magnitudeText = 0;

	componentText = 0;
}

//=================================================================================
/*virtual*/ VecOrBivecNodeControlPanel::~VecOrBivecNodeControlPanel( void )
{
}

//=================================================================================
/*virtual*/ bool VecOrBivecNodeControlPanel::PushNodeDataToInterface( void )
{
	Cornucopia::VectorE3GANode* vectorNode = node->Cast< Cornucopia::VectorE3GANode >();
	Cornucopia::BivectorE3GANode* bivectorNode = node->Cast< Cornucopia::BivectorE3GANode >();

	c3ga::vectorE3GA vector;
	if( vectorNode )
		vector = vectorNode->GetVector();
	else if( bivectorNode )
		vector = c3ga::gp( bivectorNode->GetBivector(), c3ga::I3 );	// Undualize.

	double latitudeAngle, longitudeAngle, magnitude;
	SphericalCoordinatesFromVector( latitudeAngle, longitudeAngle, magnitude, vector );

	latitudeSlider->SetValue( int( latitudeAngle ) );
	longitudeSlider->SetValue( int( longitudeAngle ) );

	double lerp = 0.0;
	if( vectorNode )
		lerp = magnitude / vectorNode->GetMaxLength();
	else if( bivectorNode )
		lerp = magnitude / bivectorNode->GetMaxArea();
	
	double sliderMin = magnitudeSlider->GetMin();
	double sliderMax = magnitudeSlider->GetMax();
	double sliderValue = sliderMin + lerp * ( sliderMax - sliderMin );
	magnitudeSlider->SetValue( int( sliderValue ) );

	SyncSliderValueLabelsWithSliders();
	SyncComponentTextWithNode();

	// This will cause the node canvas interface to redraw as a function of the node data.
	canvas->Refresh();
	return true;
}

//=================================================================================
void VecOrBivecNodeControlPanel::SyncSliderValueLabelsWithSliders( void )
{
	latitudeText->Clear();
	latitudeText->AppendText( wxString::Format( wxT( "%d" ), latitudeSlider->GetValue() ) );
	longitudeText->Clear();
	longitudeText->AppendText( wxString::Format( wxT( "%d" ), longitudeSlider->GetValue() ) );
	
	double magnitude = 0;
	Cornucopia::VectorE3GANode* vectorNode = node->Cast< Cornucopia::VectorE3GANode >();
	Cornucopia::BivectorE3GANode* bivectorNode = node->Cast< Cornucopia::BivectorE3GANode >();
	if( vectorNode )
		magnitude = c3ga::norm( vectorNode->GetVector() );
	else if( bivectorNode )
		magnitude = c3ga::norm( bivectorNode->GetBivector() );

	magnitudeText->Clear();
	magnitudeText->AppendText( wxString::Format( wxT( "%1.2f" ), magnitude ) );
}

//=================================================================================
void VecOrBivecNodeControlPanel::SyncComponentTextWithNode( void )
{
	Cornucopia::VectorE3GANode* vectorNode = node->Cast< Cornucopia::VectorE3GANode >();
	Cornucopia::BivectorE3GANode* bivectorNode = node->Cast< Cornucopia::BivectorE3GANode >();

	componentText->Clear();

	if( vectorNode )
	{
		c3ga::vectorE3GA vector = vectorNode->GetVector();
		componentText->AppendText( wxString::Format( wxT( "< x, y, z > = < %1.2f, %1.2f, %1.2f >" ), vector.get_e1(), vector.get_e2(), vector.get_e3() ) );
	}
	else if( bivectorNode )
	{
		c3ga::bivectorE3GA bivector = bivectorNode->GetBivector();
		componentText->AppendText( wxString::Format( wxT( "< yz, zx, xy > = < %1.2f, %1.2f, %1.2f >" ), bivector.get_e2_e3(), bivector.get_e3_e1(), bivector.get_e1_e2() ) );
	}
}

//=================================================================================
/*virtual*/ bool VecOrBivecNodeControlPanel::PullNodeDataFromInterface( void )
{
	Cornucopia::VectorE3GANode* vectorNode = node->Cast< Cornucopia::VectorE3GANode >();
	Cornucopia::BivectorE3GANode* bivectorNode = node->Cast< Cornucopia::BivectorE3GANode >();

	double latitudeAngle, longitudeAngle, magnitude;

	latitudeAngle = latitudeSlider->GetValue();
	longitudeAngle = longitudeSlider->GetValue();

	double sliderMin = magnitudeSlider->GetMin();
	double sliderMax = magnitudeSlider->GetMax();
	double sliderValue = magnitudeSlider->GetValue();
	double lerp = ( sliderValue - sliderMin ) / ( sliderMax - sliderMin );

	if( vectorNode )
		magnitude = lerp * vectorNode->GetMaxLength();
	else if( bivectorNode )
		magnitude = lerp * bivectorNode->GetMaxArea();

	c3ga::vectorE3GA vector;
	SphericalCoordinatesToVector( latitudeAngle, longitudeAngle, magnitude, vector );
	
	if( vectorNode )
		vectorNode->GetVector() = vector;
	else if( bivectorNode )
		bivectorNode->GetBivector() = c3ga::gp( vector, c3ga::negate( c3ga::I3 ) );	// Dualize.

	SyncSliderValueLabelsWithSliders();
	SyncComponentTextWithNode();

	return true;
}

//=================================================================================
/*virtual*/ void VecOrBivecNodeControlPanel::BuildUserInterface( void )
{
	canvas = new Canvas( this, Canvas::MODE_VIEW_3D );

	wxPanel* latitudePanel = new wxPanel( this );
	wxStaticText* latitudeLabel = new wxStaticText( latitudePanel, wxID_ANY, wxT( "Latitude:" ), wxDefaultPosition, wxSize( 70, -1 ), wxALIGN_RIGHT );
	latitudeSlider = new wxSlider( latitudePanel, ID_LatitudeSlider, 0, 0, 180, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL /*| wxSL_VALUE_LABEL*/ );
	latitudeText = new wxTextCtrl( latitudePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40, -1 ), wxTE_READONLY | wxTE_RIGHT );
	wxBoxSizer* latitudePanelSizer = new wxBoxSizer( wxHORIZONTAL );
	latitudePanelSizer->Add( latitudeLabel, 0, wxALL, 0 );
	latitudePanelSizer->Add( latitudeSlider, 1, wxEXPAND );
	latitudePanelSizer->Add( latitudeText, 0, wxALL, 0 );
	latitudePanel->SetSizer( latitudePanelSizer );

	wxPanel* longitudePanel = new wxPanel( this );
	wxStaticText* longitudeLabel = new wxStaticText( longitudePanel, wxID_ANY, wxT( "Longitude:" ), wxDefaultPosition, wxSize( 70, -1 ), wxALIGN_RIGHT );
	longitudeSlider = new wxSlider( longitudePanel, ID_LongitudeSlider, 0, 0, 360, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL /*| wxSL_VALUE_LABEL*/ );
	longitudeText = new wxTextCtrl( longitudePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40, -1 ), wxTE_READONLY | wxTE_RIGHT );
	wxBoxSizer* longitudePanelSizer = new wxBoxSizer( wxHORIZONTAL );
	longitudePanelSizer->Add( longitudeLabel, 0, wxALL, 0 );
	longitudePanelSizer->Add( longitudeSlider, 1, wxEXPAND );
	longitudePanelSizer->Add( longitudeText, 0, wxALL, 0 );
	longitudePanel->SetSizer( longitudePanelSizer );

	wxPanel* magnitudePanel = new wxPanel( this );
	wxStaticText* magnitudeLabel = new wxStaticText( magnitudePanel, wxID_ANY, wxT( "Magnitude:" ), wxDefaultPosition, wxSize( 70, -1 ), wxALIGN_RIGHT );
	magnitudeSlider = new wxSlider( magnitudePanel, ID_MagnitudeSlider, 0, 0, 1000, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	magnitudeText = new wxTextCtrl( magnitudePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40, -1 ), wxTE_READONLY | wxTE_RIGHT );
	wxBoxSizer* magnitudePanelSizer = new wxBoxSizer( wxHORIZONTAL );
	magnitudePanelSizer->Add( magnitudeLabel, 0, wxALL, 0 );
	magnitudePanelSizer->Add( magnitudeSlider, 1, wxEXPAND );
	magnitudePanelSizer->Add( magnitudeText, 0, wxALL, 0 );
	magnitudePanel->SetSizer( magnitudePanelSizer );

	componentText = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTRE );

	wxBoxSizer* panelSizer = new wxBoxSizer( wxVERTICAL );
	panelSizer->Add( canvas, 1, wxGROW | wxALL, 5 );
	panelSizer->Add( longitudePanel, 0, wxGROW | wxALL, 0 );
	panelSizer->Add( latitudePanel, 0, wxGROW | wxALL, 0 );
	panelSizer->Add( magnitudePanel, 0, wxGROW | wxALL, 0 );
	panelSizer->Add( componentText, 0, wxGROW | wxUP, 5 );
	SetSizer( panelSizer );

	latitudeSlider->Bind( wxEVT_SCROLL_THUMBTRACK, &VecOrBivecNodeControlPanel::OnThumbScrub, this );
	latitudeSlider->Bind( wxEVT_SCROLL_THUMBRELEASE, &VecOrBivecNodeControlPanel::OnThumbRelease, this );
	latitudeSlider->Bind( wxEVT_SCROLL_PAGEUP, &VecOrBivecNodeControlPanel::OnPageUp, this );
	latitudeSlider->Bind( wxEVT_SCROLL_PAGEDOWN, &VecOrBivecNodeControlPanel::OnPageDown, this );
	latitudeSlider->Bind( wxEVT_SCROLL_LINEUP, &VecOrBivecNodeControlPanel::OnLineUp, this );
	latitudeSlider->Bind( wxEVT_SCROLL_LINEDOWN, &VecOrBivecNodeControlPanel::OnLineDown, this );

	longitudeSlider->Bind( wxEVT_SCROLL_THUMBTRACK, &VecOrBivecNodeControlPanel::OnThumbScrub, this );
	longitudeSlider->Bind( wxEVT_SCROLL_THUMBRELEASE, &VecOrBivecNodeControlPanel::OnThumbRelease, this );
	longitudeSlider->Bind( wxEVT_SCROLL_PAGEUP, &VecOrBivecNodeControlPanel::OnPageUp, this );
	longitudeSlider->Bind( wxEVT_SCROLL_PAGEDOWN, &VecOrBivecNodeControlPanel::OnPageDown, this );
	longitudeSlider->Bind( wxEVT_SCROLL_LINEUP, &VecOrBivecNodeControlPanel::OnLineUp, this );
	longitudeSlider->Bind( wxEVT_SCROLL_LINEDOWN, &VecOrBivecNodeControlPanel::OnLineDown, this );

	magnitudeSlider->Bind( wxEVT_SCROLL_THUMBTRACK, &VecOrBivecNodeControlPanel::OnThumbScrub, this );
	magnitudeSlider->Bind( wxEVT_SCROLL_THUMBRELEASE, &VecOrBivecNodeControlPanel::OnThumbRelease, this );
	magnitudeSlider->Bind( wxEVT_SCROLL_PAGEUP, &VecOrBivecNodeControlPanel::OnPageUp, this );
	magnitudeSlider->Bind( wxEVT_SCROLL_PAGEDOWN, &VecOrBivecNodeControlPanel::OnPageDown, this );
	magnitudeSlider->Bind( wxEVT_SCROLL_LINEUP, &VecOrBivecNodeControlPanel::OnLineUp, this );
	magnitudeSlider->Bind( wxEVT_SCROLL_LINEDOWN, &VecOrBivecNodeControlPanel::OnLineDown, this );
}

//=================================================================================
void VecOrBivecNodeControlPanel::OnThumbScrub( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( false );
	canvas->Refresh();
}

//=================================================================================
void VecOrBivecNodeControlPanel::OnThumbRelease( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
	canvas->Refresh();
}

//=================================================================================
void VecOrBivecNodeControlPanel::OnPageUp( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
	canvas->Refresh();
}

//=================================================================================
void VecOrBivecNodeControlPanel::OnPageDown( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
	canvas->Refresh();
}

//=================================================================================
void VecOrBivecNodeControlPanel::OnLineUp( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
	canvas->Refresh();
}

//=================================================================================
void VecOrBivecNodeControlPanel::OnLineDown( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
	canvas->Refresh();
}

//=================================================================================
/*virtual*/ void VecOrBivecNodeControlPanel::CanvasRender( Canvas* canvas, GLenum renderMode )
{
	canvas->Color( 0.5f, 0.5f, 0.5f, 1.f );
	Cornucopia::VectorE3GANode* vectorNode = node->Cast< Cornucopia::VectorE3GANode >();
	Cornucopia::BivectorE3GANode* bivectorNode = node->Cast< Cornucopia::BivectorE3GANode >();
	if( vectorNode )
		canvas->RenderVector( canvas->origin, vectorNode->GetVector(), 0.15, 0.5, 15 );
	else if( bivectorNode )
		canvas->RenderBivector( canvas->origin, bivectorNode->GetBivector(), 1.0, 8, 32 );
}

//=================================================================================
// Given the vector, calculate the corresponding polar coordinates in degrees.
// Longitude is always in [0,360] while lattitude in [0,180].
/*static*/ void VecOrBivecNodeControlPanel::SphericalCoordinatesFromVector( double& latitudeAngle, double& longitudeAngle, double& magnitude, const c3ga::vectorE3GA& vector )
{
	// I'm sure there's a faster way to do this, but this works.
	magnitude = c3ga::norm( vector );
	c3ga::bivectorE3GA xzPlane( c3ga::bivectorE3GA::coord_e1e2_e2e3_e3e1, 0.0, 0.0, 1.0 );
	c3ga::bivectorE3GA xzPlaneInv( c3ga::bivectorE3GA::coord_e1e2_e2e3_e3e1, 0.0, 0.0, -1.0 );
	c3ga::vectorE3GA projectedVector;
	projectedVector.set( c3ga::negate( c3ga::rc( xzPlane, c3ga::lc( vector, xzPlaneInv ) ) ) );
	c3ga::vectorE3GA rejectedVector = vector - projectedVector;
	if( rejectedVector.get_e2() < 0.0 )
		rejectedVector.set_e2( -rejectedVector.get_e2() );	// The latitude angle is always measured against the +Y axis.
	longitudeAngle = atan2( projectedVector.get_e3(), projectedVector.get_e1() );
	if( longitudeAngle < 0.0 )
		longitudeAngle += 2.0 * M_PI;
	double cosine = c3ga::lc( c3ga::unit( rejectedVector ), c3ga::gp( vector, 1.0 / magnitude ) );
	latitudeAngle = acos( cosine );
	longitudeAngle *= 180.0 / M_PI;
	latitudeAngle *= 180.0 / M_PI;
}

//=================================================================================
// Given polar coordinates in degrees, calculate the corresponding vector.
/*static*/ void VecOrBivecNodeControlPanel::SphericalCoordinatesToVector( double latitudeAngle, double longitudeAngle, double magnitude, c3ga::vectorE3GA& vector )
{
	latitudeAngle *= M_PI / 180.0;
	longitudeAngle *= M_PI / 180.0;
	c3ga::vectorE3GA projectedVector( c3ga::vectorE3GA::coord_e1_e2_e3, cos( longitudeAngle ), 0.0, sin( longitudeAngle ) );
	projectedVector = c3ga::gp( projectedVector, magnitude * sin( latitudeAngle ) );
	c3ga::vectorE3GA rejectedVector( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, magnitude * cos( latitudeAngle ), 0.0 );
	vector = projectedVector + rejectedVector;
}

// VecOrBivecNodeControlPanel.cpp