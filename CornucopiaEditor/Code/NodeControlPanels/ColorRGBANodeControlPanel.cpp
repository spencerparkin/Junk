// ColorRGBANodeControlPanel.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
ColorRGBANodeControlPanel::ColorRGBANodeControlPanel( wxWindow* parent, Cornucopia::Node* node ) : NodeControlPanel( parent, node )
{
	colorButton = 0;
	alphaSlider = 0;
}

//=================================================================================
/*virtual*/ ColorRGBANodeControlPanel::~ColorRGBANodeControlPanel( void )
{
}

//=================================================================================
/*virtual*/ bool ColorRGBANodeControlPanel::PushNodeDataToInterface( void )
{
	const Cornucopia::ColorRGBANode* colorRGBNode = ( const Cornucopia::ColorRGBANode* )node;
	Cornucopia::ColorRGBANode::Color nodeColor;
	colorRGBNode->GetColor( nodeColor );

	colorButton->SetForegroundColour( *wxBLACK );
	if( nodeColor.Brightness() < 0.6 )
		colorButton->SetForegroundColour( *wxWHITE );

	wxString colorButtonLabel = wxString::Format( wxT( "(%1.2f,%1.2f,%1.2f)" ), nodeColor.r, nodeColor.g, nodeColor.b );
	colorButton->SetLabel( colorButtonLabel );
	GetNodeColorRGB( color );
	colorButton->SetBackgroundColour( color );
	colorButton->Refresh();

	GetNodeColorAlpha( alpha );
	alphaSlider->SetValue( alpha );

	return true;
}

//=================================================================================
/*virtual*/ bool ColorRGBANodeControlPanel::PullNodeDataFromInterface( void )
{
	alpha = alphaSlider->GetValue();

	SetNodeColorAlpha( alpha );
	SetNodeColorRGB( color );	// Color should already be set properly.

	return true;
}

//=================================================================================
void ColorRGBANodeControlPanel::OnColorButtonPressed( wxCommandEvent& event )
{
	wxColourData colorData;
	colorData.SetCustomColour( 0, color );
	wxColourDialog colorDialog( wxGetApp().GetFrame(), &colorData );
	if( colorDialog.ShowModal() == wxID_OK )
	{
		colorData = colorDialog.GetColourData();
		color = colorData.GetColour();
		
		PullNodeDataFromInterface();
		PushNodeDataToInterface();
		SynchronizeTreePanelControls( true );
	}
}

//=================================================================================
void ColorRGBANodeControlPanel::OnThumbScrub( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( false );
}

//=================================================================================
void ColorRGBANodeControlPanel::OnThumbRelease( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
}

//=================================================================================
void ColorRGBANodeControlPanel::OnPageUp( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
}

//=================================================================================
void ColorRGBANodeControlPanel::OnPageDown( wxScrollEvent& event )
{
	PullNodeDataFromInterface();
	SynchronizeTreePanelControls( true );
}

//=================================================================================
/*virtual*/ void ColorRGBANodeControlPanel::BuildUserInterface( void )
{
	wxPanel* colorPanel = new wxPanel( this );
	wxStaticText* colorLabel = new wxStaticText( colorPanel, wxID_ANY, wxT( "Color:" ), wxDefaultPosition, wxSize( 35, -1 ), wxALIGN_RIGHT );
	colorButton = new wxButton( colorPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	wxBoxSizer* colorPanelSizer = new wxBoxSizer( wxHORIZONTAL );
	colorPanelSizer->Add( colorLabel, 0, wxALL, 0 );
	colorPanelSizer->Add( colorButton, 1, wxEXPAND );
	colorPanel->SetSizer( colorPanelSizer );

	wxPanel* alphaPanel = new wxPanel( this );
	wxStaticText* alphaLabel = new wxStaticText( alphaPanel, wxID_ANY, wxT( "Alpha:" ), wxDefaultPosition, wxSize( 35, -1 ), wxALIGN_RIGHT );
	alphaSlider = new wxSlider( alphaPanel, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	wxBoxSizer* alphaPanelSizer = new wxBoxSizer( wxHORIZONTAL );
	alphaPanelSizer->Add( alphaLabel, 0, wxALL, 0 );
	alphaPanelSizer->Add( alphaSlider, 1, wxEXPAND );
	alphaPanel->SetSizer( alphaPanelSizer );
	
	wxBoxSizer* panelSizer = new wxBoxSizer( wxVERTICAL );
	panelSizer->Add( colorPanel, 0, wxGROW | wxALL, 4 );
	panelSizer->Add( alphaPanel, 0, wxGROW | wxALL, 4 );
	SetSizer( panelSizer );

	colorButton->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &ColorRGBANodeControlPanel::OnColorButtonPressed, this );

	alphaSlider->Bind( wxEVT_SCROLL_THUMBTRACK, &ColorRGBANodeControlPanel::OnThumbScrub, this );
	alphaSlider->Bind( wxEVT_SCROLL_THUMBRELEASE, &ColorRGBANodeControlPanel::OnThumbRelease, this );
	alphaSlider->Bind( wxEVT_SCROLL_PAGEUP, &ColorRGBANodeControlPanel::OnPageUp, this );
	alphaSlider->Bind( wxEVT_SCROLL_PAGEDOWN, &ColorRGBANodeControlPanel::OnPageDown, this );
}

//=================================================================================
void ColorRGBANodeControlPanel::GetNodeColorRGB( wxColour& color ) const
{
	const Cornucopia::ColorRGBANode* colorNode = ( const Cornucopia::ColorRGBANode* )node;
	Cornucopia::ColorRGBANode::Color nodeColor;
	colorNode->GetColor( nodeColor );

	wxUint32 colorBits =
		( wxUint32( nodeColor.r * 255.0 ) << 0 ) |
		( wxUint32( nodeColor.g * 255.0 ) << 8 ) |
		( wxUint32( nodeColor.b * 255.0 ) << 16 );

	color.SetRGB( colorBits );
}

//=================================================================================
void ColorRGBANodeControlPanel::SetNodeColorRGB( const wxColour& color )
{
	Cornucopia::ColorRGBANode* colorRGBNode = ( Cornucopia::ColorRGBANode* )node;
	Cornucopia::ColorRGBANode::Color nodeColor;
	colorRGBNode->GetColor( nodeColor );	// Preserve current alpha.

	nodeColor.r = double( color.Red() ) / 255.0;
	nodeColor.g = double( color.Green() ) / 255.0;
	nodeColor.b = double( color.Blue() ) / 255.0;

	colorRGBNode->SetColor( nodeColor );
}

//=================================================================================
void ColorRGBANodeControlPanel::GetNodeColorAlpha( wxUint32& alpha ) const
{
	const Cornucopia::ColorRGBANode* colorRGBNode = ( const Cornucopia::ColorRGBANode* )node;
	Cornucopia::ColorRGBANode::Color nodeColor;
	colorRGBNode->GetColor( nodeColor );
	alpha = wxUint32( nodeColor.a * 255.0 );
}

//=================================================================================
void ColorRGBANodeControlPanel::SetNodeColorAlpha( wxUint32 alpha )
{
	Cornucopia::ColorRGBANode* colorRGBNode = ( Cornucopia::ColorRGBANode* )node;
	Cornucopia::ColorRGBANode::Color nodeColor;
	colorRGBNode->GetColor( nodeColor );	// Preserve current RGB.
	nodeColor.a = double( alpha ) / 255.0;
	colorRGBNode->SetColor( nodeColor );
}

// ColorRGBANodeControlPanel.cpp