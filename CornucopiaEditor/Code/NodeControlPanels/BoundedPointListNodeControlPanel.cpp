// BoundedPointListNodeControlPanel.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

wxIMPLEMENT_DYNAMIC_CLASS( BoundedPointListNodeControlPanel, NodeControlPanel )

// TODO: Double clicking the canvas should auto-zoom/pan the camera to frame everything nicely.

//=================================================================================
BoundedPointListNodeControlPanel::BoundedPointListNodeControlPanel( wxWindow* parent, Cornucopia::Node* node ) : NodeControlPanel( parent, node )
{
	wxASSERT( 0 != node->Cast< Cornucopia::BoundedPointListNode >() );
	canvas = 0;
	xCoordTextCtrl = 0;
	yCoordTextCtrl = 0;
	indexTextCtrl = 0;
	selectedPointIndex = -1;
}

//=================================================================================
/*virtual*/ BoundedPointListNodeControlPanel::~BoundedPointListNodeControlPanel( void )
{
}

//=================================================================================
/*virtual*/ void BoundedPointListNodeControlPanel::BuildUserInterface( void )
{
	canvas = new PointListCanvas( this, Canvas::MODE_VIEW_2D );

	wxPanel* pointPanel = new wxPanel( this );
	wxPanel* xCoordPanel = new wxPanel( pointPanel );
	wxPanel* yCoordPanel = new wxPanel( pointPanel );
	wxPanel* indexPanel = new wxPanel( pointPanel );
	wxBoxSizer* pointPanelSizer = new wxBoxSizer( wxHORIZONTAL );
	pointPanelSizer->Add( xCoordPanel, 1, wxGROW );
	pointPanelSizer->Add( yCoordPanel, 1, wxGROW );
	pointPanelSizer->Add( indexPanel, 1, wxGROW );
	pointPanel->SetSizer( pointPanelSizer );

	wxStaticText* xCoordLabel = new wxStaticText( xCoordPanel, wxID_ANY, wxT( "X:" ), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	xCoordTextCtrl = new wxTextCtrl( xCoordPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxTE_LEFT );
	wxBoxSizer* xCoordPanelSizer = new wxBoxSizer( wxHORIZONTAL );
	xCoordPanelSizer->Add( xCoordLabel, 0, wxLEFT | wxRIGHT | wxDOWN, 5 );
	xCoordPanelSizer->Add( xCoordTextCtrl, 1, wxGROW | wxLEFT | wxRIGHT | wxDOWN, 5 );
	xCoordPanel->SetSizer( xCoordPanelSizer );

	wxStaticText* yCoordLabel = new wxStaticText( yCoordPanel, wxID_ANY, wxT( "Y:" ), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	yCoordTextCtrl = new wxTextCtrl( yCoordPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxTE_LEFT );
	wxBoxSizer* yCoordPanelSizer = new wxBoxSizer( wxHORIZONTAL );
	yCoordPanelSizer->Add( yCoordLabel, 0, wxLEFT | wxRIGHT | wxDOWN, 5 );
	yCoordPanelSizer->Add( yCoordTextCtrl, 1, wxGROW | wxLEFT | wxRIGHT | wxDOWN, 5 );
	yCoordPanel->SetSizer( yCoordPanelSizer );

	wxStaticText* indexLabel = new wxStaticText( indexPanel, wxID_ANY, wxT( "Idx:" ), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	indexTextCtrl = new wxTextCtrl( indexPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_LEFT );
	wxBoxSizer* indexPanelSizer = new wxBoxSizer( wxHORIZONTAL );
	indexPanelSizer->Add( indexLabel, 0, wxLEFT | wxRIGHT | wxDOWN, 5 );
	indexPanelSizer->Add( indexTextCtrl, 1, wxGROW | wxLEFT | wxRIGHT | wxDOWN, 5 );
	indexPanel->SetSizer( indexPanelSizer );

	wxBoxSizer* panelSizer = new wxBoxSizer( wxVERTICAL );
	panelSizer->Add( canvas, 1, wxGROW | wxALL, 5 );
	panelSizer->Add( pointPanel, 0, wxGROW );
	SetSizer( panelSizer );

	xCoordTextCtrl->Bind( wxEVT_COMMAND_TEXT_ENTER, &BoundedPointListNodeControlPanel::OnXCoordEnter, this );
	yCoordTextCtrl->Bind( wxEVT_COMMAND_TEXT_ENTER, &BoundedPointListNodeControlPanel::OnYCoordEnter, this );
}

//=================================================================================
/*virtual*/ void BoundedPointListNodeControlPanel::OnXCoordEnter( wxCommandEvent& event )
{
	ProcessCoordinateEnter( xCoordTextCtrl );
}

//=================================================================================
/*virtual*/ void BoundedPointListNodeControlPanel::OnYCoordEnter( wxCommandEvent& event )
{
	ProcessCoordinateEnter( yCoordTextCtrl );
}

//=================================================================================
void BoundedPointListNodeControlPanel::ProcessCoordinateEnter( wxTextCtrl* coordTextCtrl )
{
	do
	{
		Cornucopia::BoundedPointListNode* boundedPointListNode = ( Cornucopia::BoundedPointListNode* )node;
		if( selectedPointIndex < 0 || selectedPointIndex >= ( signed )boundedPointListNode->GetPointList().size() )
			break;

		wxString valueString = coordTextCtrl->GetValue();
		double value = 0.0;
		if( !valueString.ToDouble( &value ) )
		{
			wxMessageBox( wxString::Format( "Please enter a valid number into the text control.  \"%s\" was not recognized.", valueString.c_str() ), wxT( "Error" ), wxCENTRE | wxOK );
			break;
		}
	
		Cornucopia::BoundedPointListNode::List::const_iterator setAtIter = boundedPointListNode->IndexToIterator( selectedPointIndex );
		c3ga::vectorE2GA point = *setAtIter;
		if( coordTextCtrl == xCoordTextCtrl )
			point.set_e1( value );
		else
			point.set_e2( value );

		if( !boundedPointListNode->SetPoint( point, setAtIter ) )
			break;
		
		SynchronizeTreePanelControls( true );
		canvas->Refresh();
		return;
	}
	while( false );

	UpdateCoordinateControls();
}

//=================================================================================
/*virtual*/ bool BoundedPointListNodeControlPanel::PushNodeDataToInterface( void )
{
	UpdateCoordinateControls();

	// The canvas draws as a function of the node, so it just needs to be refreshed here.
	canvas->Refresh();

	return true;
}

//=================================================================================
/*virtual*/ bool BoundedPointListNodeControlPanel::PullNodeDataFromInterface( void )
{
	// Nothing to do here.
	return true;
}

//=================================================================================
void BoundedPointListNodeControlPanel::UpdateCoordinateControls( void )
{
	xCoordTextCtrl->Clear();
	yCoordTextCtrl->Clear();

	c3ga::vectorE2GA point;
	if( GetSelectedPoint( point ) )
	{
		xCoordTextCtrl->AppendText( wxString::Format( wxT( "%f" ), point.get_e1() ) );
		yCoordTextCtrl->AppendText( wxString::Format( wxT( "%f" ), point.get_e2() ) );
	}

	indexTextCtrl->Clear();
	if( selectedPointIndex < 0 )
		indexTextCtrl->AppendText( wxT( "No selection." ) );
	else
		indexTextCtrl->AppendText( wxString::Format( wxT( "%d" ), selectedPointIndex ) );
}

//=================================================================================
/*virtual*/ void BoundedPointListNodeControlPanel::CanvasRender( Canvas* canvas, GLenum renderMode )
{
	Cornucopia::BoundedPointListNode* boundedPointListNode = ( Cornucopia::BoundedPointListNode* )node;

	Cornucopia::BoundedPointListNode::Bounds bounds = boundedPointListNode->GetBounds();

	if( boundedPointListNode->IsBounded() && renderMode == GL_RENDER )
	{
		canvas->Color( 1.f, 0.f, 0.f, 1.f );
		glDisable( GL_LIGHTING );
		glDisable( GL_LINE_SMOOTH );
		glLineWidth( 1.5f );
		glBegin( GL_LINE_LOOP );
		glVertex3d( bounds.min.get_e1(), bounds.min.get_e2(), 0.0 );
		glVertex3d( bounds.max.get_e1(), bounds.min.get_e2(), 0.0 );
		glVertex3d( bounds.max.get_e1(), bounds.max.get_e2(), 0.0 );
		glVertex3d( bounds.min.get_e1(), bounds.max.get_e2(), 0.0 );
		glEnd();
		glEnable( GL_LIGHTING );
		glEnable( GL_LINE_SMOOTH );
	}

	const Canvas::Camera& camera = canvas->GetCamera();
	double focalDistance = c3ga::norm( camera.eye - camera.focus );
	double pointRadius = focalDistance / 80.0;

	if( renderMode == GL_SELECT )
		glPushName( -1 );

	int index = 0;
	const Cornucopia::BoundedPointListNode::List& pointList = boundedPointListNode->GetPointList();
	for( Cornucopia::BoundedPointListNode::List::const_iterator iter = pointList.begin(); iter != pointList.end(); iter++ )
	{
		if( selectedPointIndex == index )
			canvas->Color( 0.f, 1.f, 0.f, 1.f );
		else
			canvas->Color( 1.f, 0.f, 0.f, 1.f );

		if( renderMode == GL_SELECT )
			glLoadName( index );

		c3ga::vectorE2GA point = *iter;
		c3ga::vectorE3GA location;
		location.set( c3ga::vectorE3GA::coord_e1_e2_e3, point.get_e1(), point.get_e2(), 0.0 );
		canvas->RenderSphere( location, pointRadius );

		index++;
	}
}

//=================================================================================
/*virtual*/ void BoundedPointListNodeControlPanel::CanvasSelect( Canvas* canvas, unsigned int* hitBuffer, int hitBufferSize, int hitCount )
{
	// This condition would indicate a hit-buffer overflow.
	if( hitCount == -1 )
		return;

	// Find the hit record that has the index for the selection.
	int index = -1;
	float smallestZ = 999999.f;
	unsigned int* hitRecord = hitBuffer;
	for( int hitIndex = 0; hitIndex < hitCount; hitIndex++ )
	{
		int indexCount = hitRecord[0];	// Should always be 1 for our purposes.
		float minZ = float( hitRecord[1] ) / float( 0x7FFFFFFF );
		if( minZ < smallestZ && indexCount > 0 )
		{
			smallestZ = minZ;
			index = hitRecord[3];
		}
		hitRecord += 3 + indexCount;
	}

	// Make the selection!  -1 is okay, and indicates that nothing is selected.
	selectedPointIndex = index;

	// Update our UI.
	UpdateCoordinateControls();
	Refresh();
}

//=================================================================================
/*virtual*/ wxMenu* BoundedPointListNodeControlPanel::CreateMenu( wxEvtHandler* eventHandler )
{
	wxString nodeName = ( const char* )node->GetName().c_str();
	wxMenu* pointListMenu = new wxMenu( nodeName );

	wxMenuItem* addPointMenuItem = new wxMenuItem( pointListMenu, ID_ContextMenu_AddPoint, wxT( "Add Point" ) );
	wxMenuItem* removePointMenuItem = new wxMenuItem( pointListMenu, ID_ContextMenu_RemovePoint, wxT( "Remove Point" ) );
	wxMenuItem* boundedItem = new wxMenuItem( pointListMenu, ID_ContextMenu_Bounded, wxT( "Bounded" ), wxEmptyString, wxITEM_CHECK );

	pointListMenu->Append( addPointMenuItem );
	pointListMenu->Append( removePointMenuItem );
	pointListMenu->AppendSeparator();
	pointListMenu->Append( boundedItem );

	Cornucopia::BoundedPointListNode* boundedPointListNode = ( Cornucopia::BoundedPointListNode* )node;
	boundedItem->Check( boundedPointListNode->IsBounded() );

	if( !eventHandler )
		eventHandler = pointListMenu->GetEventHandler();
	eventHandler->Bind( wxEVT_COMMAND_MENU_SELECTED, &BoundedPointListNodeControlPanel::OnContextMenu, this );

	return pointListMenu;
}

//=================================================================================
/*virtual*/ void BoundedPointListNodeControlPanel::OnContextMenu( wxCommandEvent& event )
{
	Cornucopia::BoundedPointListNode* boundedPointListNode = ( Cornucopia::BoundedPointListNode* )node;

	switch( event.GetId() )
	{
		case ID_ContextMenu_AddPoint:
		{
			Cornucopia::BoundedPointListNode::Bounds bounds = boundedPointListNode->GetBounds();
			c3ga::vectorE2GA point = c3ga::Lerp( bounds.min, bounds.max, 0.5 );
			boundedPointListNode->InsertPoint( point );

			SynchronizeTreePanelControls( true );
			Refresh();

			break;
		}
		case ID_ContextMenu_RemovePoint:
		{
			if( selectedPointIndex < 0 )
			{
				wxMessageBox( wxT( "You must first select the point to be removed by right-clicking on it with the CTRL key down." ), wxT( "Error" ), wxCENTRE | wxOK, wxGetApp().GetFrame() );
				return;
			}

			Cornucopia::BoundedPointListNode::List::const_iterator selectedPointIter = boundedPointListNode->IndexToIterator( selectedPointIndex );
			boundedPointListNode->RemovePoint( selectedPointIter );
			selectedPointIndex = -1;

			SynchronizeTreePanelControls( true );
			Refresh();

			break;
		}
		case ID_ContextMenu_Bounded:
		{
			if( boundedPointListNode->IsBounded() )
				boundedPointListNode->SetBounded( false );
			else if( !boundedPointListNode->SetBounded( true ) )
			{
				Cornucopia::BoundedPointListNode::Bounds tightestBounds;
				boundedPointListNode->FindTightestBounds( tightestBounds );
				boundedPointListNode->SetBounds( tightestBounds );
				boundedPointListNode->SetBounded( true );
			}

			SynchronizeTreePanelControls( true );
			Refresh();

			break;
		}
		default:
		{
			// Let the data-view control handle the event.
			event.Skip();
		}
	}
}

//=================================================================================
BoundedPointListNodeControlPanel::PointListCanvas::PointListCanvas( wxWindow* parent, Canvas::Mode mode ) : Canvas( parent, mode )
{
	pointScrubInProgress = false;
}

//=================================================================================
/*virtual*/ BoundedPointListNodeControlPanel::PointListCanvas::~PointListCanvas( void )
{
}

//=================================================================================
/*virtual*/ void BoundedPointListNodeControlPanel::PointListCanvas::OnMouseMove( wxMouseEvent& event )
{
	if( !event.ControlDown() )
		Canvas::OnMouseMove( event );
	else if( event.LeftIsDown() )
	{
		wxPoint mouseDelta;
		MouseDrag( event, &mouseDelta );

		c3ga::vectorE2GA delta;

		const Camera& camera = GetCamera();
		double focalDistance = c3ga::norm( camera.eye - camera.focus );
		double scaleFactor = 0.005;
		delta.set_e1( double( mouseDelta.x ) * scaleFactor * focalDistance );
		delta.set_e2( double( -mouseDelta.y ) * scaleFactor * focalDistance );

		NodeControlPanel* nodeControlPanel = GetAssociatedPanel();
		if( nodeControlPanel )
		{
			BoundedPointListNodeControlPanel* boundedPointListNodeControlPanel = wxDynamicCast( nodeControlPanel, BoundedPointListNodeControlPanel );
			if( boundedPointListNodeControlPanel && boundedPointListNodeControlPanel->ApplyDeltaToSelectedPoint( delta ) )
			{
				pointScrubInProgress = true;
				boundedPointListNodeControlPanel->OnScrubHappening();
				Refresh();
			}
		}
	}
}

//=================================================================================
/*virtual*/ bool BoundedPointListNodeControlPanel::ApplyDeltaToSelectedPoint( const c3ga::vectorE2GA& delta )
{
	Cornucopia::BoundedPointListNode* boundedPointListNode = ( Cornucopia::BoundedPointListNode* )node;
	if( selectedPointIndex >= 0 && selectedPointIndex < ( signed )boundedPointListNode->GetPointList().size() )
	{
		Cornucopia::BoundedPointListNode::List::const_iterator selectedPointIter = boundedPointListNode->IndexToIterator( selectedPointIndex );
		c3ga::vectorE2GA point = *selectedPointIter;
		point = point + delta;
		if( boundedPointListNode->SetPoint( point, selectedPointIter, false ) )
			return true;
	}
	return false;
}

//=================================================================================
/*virtual*/ void BoundedPointListNodeControlPanel::OnScrubHappening( void )
{
	UpdateCoordinateControls();

	// This is also where we might tell a live-authoring plugin that something has just happened.
}

//=================================================================================
/*virtual*/ void BoundedPointListNodeControlPanel::PointListCanvas::OnMouseLeftDown( wxMouseEvent& event )
{
	Canvas::OnMouseLeftDown( event );
}

//=================================================================================
/*virtual*/ void BoundedPointListNodeControlPanel::PointListCanvas::OnMouseLeftUp( wxMouseEvent& event )
{
	if( pointScrubInProgress )
	{
		NodeControlPanel* nodeControlPanel = GetAssociatedPanel();
		if( nodeControlPanel )
		{
			BoundedPointListNodeControlPanel* boundedPointListNodeControlPanel = wxDynamicCast( nodeControlPanel, BoundedPointListNodeControlPanel );
			if( boundedPointListNodeControlPanel )
				boundedPointListNodeControlPanel->OnScrubFinished();
		}

		pointScrubInProgress = false;
	}
}

//=================================================================================
/*virtual*/ void BoundedPointListNodeControlPanel::OnScrubFinished( void )
{
	UpdateCoordinateControls();
	SynchronizeTreePanelControls( true );
}

//=================================================================================
/*virtual*/ void BoundedPointListNodeControlPanel::PointListCanvas::OnMouseRightDown( wxMouseEvent& event )
{
	Canvas::OnMouseRightDown( event );

	if( event.ControlDown() )
		PerformSelection( event.GetPosition() );
	else
	{
		NodeControlPanel* nodeControlPanel = GetAssociatedPanel();
		wxMenu* contextMenu = nodeControlPanel->CreateMenu( 0 );
		if( contextMenu )
		{
			PopupMenu( contextMenu );
			delete contextMenu;
		}
	}
}

//=================================================================================
bool BoundedPointListNodeControlPanel::GetSelectedPoint( c3ga::vectorE2GA& point )
{
	Cornucopia::BoundedPointListNode* boundedPointListNode = ( Cornucopia::BoundedPointListNode* )node;
	if( selectedPointIndex >= 0 && selectedPointIndex < ( signed )boundedPointListNode->GetPointList().size() )
	{
		Cornucopia::BoundedPointListNode::List::const_iterator selectedPointIter = boundedPointListNode->IndexToIterator( selectedPointIndex );
		point = *selectedPointIter;
		return true;
	}

	return false;
}

//=================================================================================
/*virtual*/ void BoundedPointListNodeControlPanel::PointListCanvas::OnCharHook( wxKeyEvent& event )
{
	switch( event.GetKeyCode() )
	{
		case WXK_RETURN:
		{
			NodeControlPanel* nodeControlPanel = GetAssociatedPanel();
			Cornucopia::BoundedPointListNode* boundedPointListNode = ( Cornucopia::BoundedPointListNode* )nodeControlPanel->GetNode();
			CenterCameraOnPoints( boundedPointListNode );
			break;
		}
	}
}

//=================================================================================
void BoundedPointListNodeControlPanel::PointListCanvas::CenterCameraOnPoints( Cornucopia::BoundedPointListNode* pointListNode )
{
	Cornucopia::BoundedPointListNode::Bounds bounds;
	if( pointListNode->IsBounded() )
		bounds = pointListNode->GetBounds();
	else
	{
		const Cornucopia::BoundedPointListNode::List& pointList = pointListNode->GetPointList();
		if( pointList.size() < 1 )
			return;

		if( !pointListNode->FindTightestBounds( bounds ) )
			return;
	}

	// This doesn't account for the window's dimensions, but it works well enough.
	c3ga::vectorE2GA center = c3ga::Lerp( bounds.min, bounds.max, 0.5 );
	c3ga::vectorE2GA extent = bounds.max - bounds.min;
	double largestExtent = extent.largestCoordinate();
	largestExtent *= 1.1;		// Make it slightly larger than it is.
	double focalDistance = largestExtent / ( 2.0 * tan( camera.foviAngle * 0.5 ) );
	if( focalDistance < 1.0 )
		focalDistance = 1.0;	// Don't let it be too short.

	camera.eye.set( c3ga::vectorE3GA::coord_e1_e2_e3, center.get_e1(), center.get_e2(), focalDistance );
	camera.focus.set( c3ga::vectorE3GA::coord_e1_e2_e3, center.get_e1(), center.get_e2(), 0.0 );

	Refresh();
}

// BoundedPointListNodeControlPanel.h