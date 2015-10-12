// BoundedBoxNodeControlPanel.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

wxIMPLEMENT_DYNAMIC_CLASS( BoundedBoxNodeControlPanel, NodeControlPanel )

//=================================================================================
BoundedBoxNodeControlPanel::BoundedBoxNodeControlPanel( wxWindow* parent, Cornucopia::Node* node ) : NodeControlPanel( parent, node )
{
	canvas = 0;

	xTwinSlider = 0;
	yTwinSlider = 0;
	zTwinSlider = 0;

	xMinText = 0;
	yMinText = 0;
	zMinText = 0;

	xMaxText = 0;
	yMaxText = 0;
	zMaxText = 0;

	alphaSlider = 0;
}

//=================================================================================
/*virtual*/ BoundedBoxNodeControlPanel::~BoundedBoxNodeControlPanel( void )
{
}

//=================================================================================
/*virtual*/ void BoundedBoxNodeControlPanel::BuildUserInterface( void )
{
	canvas = new Canvas( this, Canvas::MODE_VIEW_3D );

	xTwinSlider = new TwinSliderControl( this, wxID_ANY, 0, 1, 0, 1, wxDefaultPosition, wxSize( -1, 20 ) );
	yTwinSlider = new TwinSliderControl( this, wxID_ANY, 0, 1, 0, 1, wxDefaultPosition, wxSize( -1, 20 ) );
	zTwinSlider = new TwinSliderControl( this, wxID_ANY, 0, 1, 0, 1, wxDefaultPosition, wxSize( -1, 20 ) );

	xMinText = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40, 20 ), wxTE_READONLY | wxTE_CENTRE );
	yMinText = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40, 20 ), wxTE_READONLY | wxTE_CENTRE );
	zMinText = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40, 20 ), wxTE_READONLY | wxTE_CENTRE );

	xMaxText = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40, 20 ), wxTE_READONLY | wxTE_CENTRE );
	yMaxText = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40, 20 ), wxTE_READONLY | wxTE_CENTRE );
	zMaxText = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40, 20 ), wxTE_READONLY | wxTE_CENTRE );

	alphaSlider = new LoneSliderControl( this, wxID_ANY, 0, 1, 0, wxDefaultPosition, wxSize( -1, 20 ) );

	wxBoxSizer* xBoxSizer = new wxBoxSizer( wxHORIZONTAL );
	xBoxSizer->Add( xMinText, 0, wxALL, 0 );
	xBoxSizer->Add( xTwinSlider, 1, wxGROW | wxALL, 0 );
	xBoxSizer->Add( xMaxText, 0, wxALL, 0 );

	wxBoxSizer* yBoxSizer = new wxBoxSizer( wxHORIZONTAL );
	yBoxSizer->Add( yMinText, 0, wxALL, 0 );
	yBoxSizer->Add( yTwinSlider, 1, wxGROW | wxALL, 0 );
	yBoxSizer->Add( yMaxText, 0, wxALL, 0 );

	wxBoxSizer* zBoxSizer = new wxBoxSizer( wxHORIZONTAL );
	zBoxSizer->Add( zMinText, 0, wxALL, 0 );
	zBoxSizer->Add( zTwinSlider, 1, wxGROW | wxALL, 0 );
	zBoxSizer->Add( zMaxText, 0, wxALL, 0 );

	wxBoxSizer* alphaBoxSizer = new wxBoxSizer( wxHORIZONTAL );
	alphaBoxSizer->Add( alphaSlider, 1, wxGROW | wxALL, 0 );

	wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
	boxSizer->Add( canvas, 1, wxGROW | wxALL, 0 );
	boxSizer->Add( xBoxSizer, 0, wxALL, 0 );
	boxSizer->Add( yBoxSizer, 0, wxALL, 0 );
	boxSizer->Add( zBoxSizer, 0, wxALL, 0 );
	boxSizer->Add( alphaBoxSizer, 0, wxALL, 0 );
	SetSizer( boxSizer );
}

//=================================================================================
/*virtual*/ bool BoundedBoxNodeControlPanel::PushNodeDataToInterface( void )
{
	return true;
}

//=================================================================================
/*virtual*/ bool BoundedBoxNodeControlPanel::PullNodeDataFromInterface( void )
{
	return true;
}

//=================================================================================
/*virtual*/ void BoundedBoxNodeControlPanel::CanvasRender( Canvas* canvas, GLenum renderMode )
{
}

// BoundedBoxNodeControlPanel.cpp