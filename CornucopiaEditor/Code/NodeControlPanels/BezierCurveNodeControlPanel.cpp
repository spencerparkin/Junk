// BezierCurveNodeControlPanel.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

wxIMPLEMENT_DYNAMIC_CLASS( BezierCurveNodeControlPanel, BoundedPointListNodeControlPanel )

//=================================================================================
BezierCurveNodeControlPanel::BezierCurveNodeControlPanel( wxWindow* parent, Cornucopia::Node* node ) : BoundedPointListNodeControlPanel( parent, node )
{
	wxASSERT( 0 != node->Cast< Cornucopia::BezierCurveNode >() );

	drawMode = DRAW_AS_FUNCTION;
	newtonIters = 1;
}

//=================================================================================
/*virtual*/ BezierCurveNodeControlPanel::~BezierCurveNodeControlPanel( void )
{
}

//=================================================================================
/*virtual*/ void BezierCurveNodeControlPanel::BuildUserInterface( void )
{
	BoundedPointListNodeControlPanel::BuildUserInterface();
}

//=================================================================================
/*virtual*/ bool BezierCurveNodeControlPanel::PushNodeDataToInterface( void )
{
	return BoundedPointListNodeControlPanel::PushNodeDataToInterface();
}

//=================================================================================
/*virtual*/ bool BezierCurveNodeControlPanel::PullNodeDataFromInterface( void )
{
	return BoundedPointListNodeControlPanel::PullNodeDataFromInterface();
}

//=================================================================================
/*virtual*/ void BezierCurveNodeControlPanel::CanvasRender( Canvas* canvas, GLenum renderMode )
{
	BoundedPointListNodeControlPanel::CanvasRender( canvas, renderMode );

	if( renderMode == GL_RENDER )
	{
		Cornucopia::BezierCurveNode* bezierCurveNode = ( Cornucopia::BezierCurveNode* )node;
		const Cornucopia::BezierCurveNode::List& pointList = bezierCurveNode->GetPointList();
		if( pointList.size() > 1 )
		{
			glLineWidth( 1.f );
			canvas->Color( 0.f, 0.7f, 0.f, 1.f );

			glDisable( GL_LIGHTING );
			glBegin( GL_LINE_STRIP );

			double x0 = ( *pointList.begin() ).get_e1();
			double x1 = ( *--pointList.end() ).get_e1();

			int resolution = 100;
			for( int k = 0; k < resolution; k++ )
			{
				double t = double( k ) / double ( resolution - 1 );
				if( drawMode == DRAW_AS_PARAMETRIC_FUNCTION )
				{
					c3ga::vectorE2GA point;
					bezierCurveNode->Evaluate( t, point );
					glVertex2f( point.get_e1(), point.get_e2() );
				}
				else if( drawMode == DRAW_AS_FUNCTION )
				{
					double x = ( 1.0 - t ) * x0 + t * x1;
					double y = 0.0;
					bezierCurveNode->Evaluate( x, y, newtonIters );
					glVertex2f( x, y );
				}
			}

			glEnd();
			glEnable( GL_LIGHTING );
		}
	}
}

//=================================================================================
/*virtual*/ wxMenu* BezierCurveNodeControlPanel::CreateMenu( wxEvtHandler* eventHandler )
{
	wxMenu* pointListMenu = BoundedPointListNodeControlPanel::CreateMenu( eventHandler );

	wxMenuItem* incrementDegreeItem = new wxMenuItem( pointListMenu, ID_ContextMenu_IncDegree, wxT( "Increment Degree" ) );
	wxMenuItem* decrementDegreeItem = new wxMenuItem( pointListMenu, ID_ContextMenu_DecDegree, wxT( "Decrement Degree" ) );

	wxMenu* drawModeSubMenu = new wxMenu();
	wxMenuItem* drawAsParaFuncItem = new wxMenuItem( drawModeSubMenu, ID_ContextMenu_DrawAsParametricFunction, wxT( "Draw As Parametric Function" ), wxEmptyString, wxITEM_CHECK );
	wxMenuItem* drawAsFuncItem = new wxMenuItem( drawModeSubMenu, ID_ContextMenu_DrawAsFunction, wxT( "Draw As Function" ), wxEmptyString, wxITEM_CHECK );
	drawModeSubMenu->Append( drawAsParaFuncItem );
	drawModeSubMenu->Append( drawAsFuncItem );

	Cornucopia::BezierCurveNode* bezierCurveNode = ( Cornucopia::BezierCurveNode* )node;
	if( bezierCurveNode->GetDegree() < 0 )
		decrementDegreeItem->Enable( false );

	drawAsParaFuncItem->Check( drawMode == DRAW_AS_PARAMETRIC_FUNCTION ? true : false );
	drawAsFuncItem->Check( drawMode == DRAW_AS_FUNCTION ? true : false );

	pointListMenu->AppendSeparator();
	pointListMenu->Append( incrementDegreeItem );
	pointListMenu->Append( decrementDegreeItem );
	pointListMenu->AppendSeparator();
	pointListMenu->AppendSubMenu( drawModeSubMenu, wxT( "Draw Mode" ) );

	return pointListMenu;
}

//=================================================================================
/*virtual*/ void BezierCurveNodeControlPanel::OnContextMenu( wxCommandEvent& event )
{
	Cornucopia::BezierCurveNode* bezierCurveNode = ( Cornucopia::BezierCurveNode* )node;

	switch( event.GetId() )
	{
		case ID_ContextMenu_IncDegree:
		{
			if( bezierCurveNode->IncrementDegree() )
			{
				SynchronizeTreePanelControls( true );
				Refresh();
			}
			break;
		}
		case ID_ContextMenu_DecDegree:
		{
			if( bezierCurveNode->DecrementDegree() )
			{
				SynchronizeTreePanelControls( true );
				Refresh();
			}
			break;
		}
		case ID_ContextMenu_DrawAsParametricFunction:
		{
			if( drawMode != DRAW_AS_PARAMETRIC_FUNCTION )
			{
				drawMode = DRAW_AS_PARAMETRIC_FUNCTION;
				Refresh();
			}
			break;
		}
		case ID_ContextMenu_DrawAsFunction:
		{
			if( drawMode != DRAW_AS_FUNCTION )
			{
				drawMode = DRAW_AS_FUNCTION;
				Refresh();
			}
			break;
		}
		default:
		{
			BoundedPointListNodeControlPanel::OnContextMenu( event );
			break;
		}
	}
}

// BezierCurveNodeControlPanel.cpp