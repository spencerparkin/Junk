// VecOrBivecPropertyEditor.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
wxIMPLEMENT_DYNAMIC_CLASS( VecOrBivecPropertyEditor, wxPGEditor );

//=================================================================================
VecOrBivecPropertyEditor::VecOrBivecPropertyEditor( void )
{
}

//=================================================================================
/*virtual*/ VecOrBivecPropertyEditor::~VecOrBivecPropertyEditor( void )
{
}

//=================================================================================
/*virtual*/ wxPGWindowList VecOrBivecPropertyEditor::CreateControls( wxPropertyGrid* propertyGrid, wxPGProperty* property, const wxPoint& pos, const wxSize& size ) const
{
	wxPanel* panel = new wxPanel( propertyGrid, wxID_ANY, pos, size );

	wxTextCtrl* labelText[3];
	wxTextCtrl* componentText[3];

	Cornucopia::VectorE3GANode* vectorNode = 0;
	Cornucopia::BivectorE3GANode* bivectorNode = 0;
	GrabVecOrBivecPointer( property, vectorNode, bivectorNode );

	wxString labelString[3] = { "0", "1", "2" };

	if( vectorNode )
	{
		labelString[0] = "X:";
		labelString[1] = "Y:";
		labelString[2] = "Z:";
	}
	else if( bivectorNode )
	{
		labelString[0] = "YZ:";
		labelString[1] = "ZX:";
		labelString[2] = "XY:";
	}

	labelText[0] = new wxTextCtrl( panel, ID_Label0, labelString[0], wxDefaultPosition, wxSize( 30, -1 ), wxTE_READONLY | wxTE_RIGHT );
	labelText[1] = new wxTextCtrl( panel, ID_Label1, labelString[1], wxDefaultPosition, wxSize( 30, -1 ), wxTE_READONLY | wxTE_RIGHT );
	labelText[2] = new wxTextCtrl( panel, ID_Label2, labelString[2], wxDefaultPosition, wxSize( 30, -1 ), wxTE_READONLY | wxTE_RIGHT );

	componentText[0] = new wxTextCtrl( panel, ID_Component0, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER | wxTE_PROCESS_ENTER );
	componentText[1] = new wxTextCtrl( panel, ID_Component1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER | wxTE_PROCESS_ENTER );
	componentText[2] = new wxTextCtrl( panel, ID_Component2, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER | wxTE_PROCESS_ENTER );

	wxBoxSizer* boxSubSizer[3];
	
	boxSubSizer[0] = new wxBoxSizer( wxHORIZONTAL );
	boxSubSizer[1] = new wxBoxSizer( wxHORIZONTAL );
	boxSubSizer[2] = new wxBoxSizer( wxHORIZONTAL );

	boxSubSizer[0]->Add( labelText[0], 0, wxALL, 0 );
	boxSubSizer[0]->Add( componentText[0], 1, wxGROW | wxALL, 0 );
	boxSubSizer[1]->Add( labelText[1], 0, wxALL, 0 );
	boxSubSizer[1]->Add( componentText[1], 1, wxGROW | wxALL, 0 );
	boxSubSizer[2]->Add( labelText[2], 0, wxALL, 0 );
	boxSubSizer[2]->Add( componentText[2], 1, wxGROW | wxALL, 0 );

	wxBoxSizer* boxSizer = new wxBoxSizer( wxHORIZONTAL );
	boxSizer->Add( boxSubSizer[0], 1, wxGROW | wxALL, 0 );
	boxSizer->Add( boxSubSizer[1], 1, wxGROW | wxALL, 0 );
	boxSizer->Add( boxSubSizer[2], 1, wxGROW | wxALL, 0 );
	panel->SetSizer( boxSizer );
	
	UpdateControl( property, panel );

	panel->Layout();
	return panel;
}

//=================================================================================
/*virtual*/ void VecOrBivecPropertyEditor::UpdateControl( wxPGProperty* property, wxWindow* control ) const
{
	Cornucopia::VectorE3GANode* vectorNode = 0;
	Cornucopia::BivectorE3GANode* bivectorNode = 0;
	GrabVecOrBivecPointer( property, vectorNode, bivectorNode );

	double component[3];

	if( vectorNode )
	{
		c3ga::vectorE3GA vector = vectorNode->GetVector();
		component[0] = vector.get_e1();
		component[1] = vector.get_e2();
		component[2] = vector.get_e3();
	}
	else if( bivectorNode )
	{
		c3ga::bivectorE3GA bivector = bivectorNode->GetBivector();
		component[0] = bivector.get_e2_e3();
		component[1] = bivector.get_e3_e1();
		component[2] = bivector.get_e1_e2();
	}

	wxTextCtrl* componentText[3];

	componentText[0] = wxDynamicCast( control->FindWindow( ID_Component0 ), wxTextCtrl );
	componentText[1] = wxDynamicCast( control->FindWindow( ID_Component1 ), wxTextCtrl );
	componentText[2] = wxDynamicCast( control->FindWindow( ID_Component2 ), wxTextCtrl );

	componentText[0]->SetValue( wxString::Format( "%1.2f", component[0] ) );
	componentText[1]->SetValue( wxString::Format( "%1.2f", component[1] ) );
	componentText[2]->SetValue( wxString::Format( "%1.2f", component[2] ) );
}

//=================================================================================
/*virtual*/ void VecOrBivecPropertyEditor::DrawValue( wxDC& dc, const wxRect& rect, wxPGProperty* property, const wxString& text ) const
{
	wxString valueText = "???";

	Cornucopia::VectorE3GANode* vectorNode = 0;
	Cornucopia::BivectorE3GANode* bivectorNode = 0;
	GrabVecOrBivecPointer( property, vectorNode, bivectorNode );

	if( vectorNode )
	{
		c3ga::vectorE3GA vector = vectorNode->GetVector();
		valueText = wxString::Format( "< x, y, z > = < %1.2f, %1.2f, %1.2f >", vector.get_e1(), vector.get_e2(), vector.get_e3() );
	}
	else if( bivectorNode )
	{
		c3ga::bivectorE3GA bivector = bivectorNode->GetBivector();
		valueText = wxString::Format( "< yz, zx, xy > = < %1.2f, %1.2f, %1.2f >", bivector.get_e2_e3(), bivector.get_e3_e1(), bivector.get_e1_e2() );
	}

	dc.DrawText( valueText, rect.x + wxPG_XBEFORETEXT, rect.y );
}

//=================================================================================
/*virtual*/ bool VecOrBivecPropertyEditor::OnEvent( wxPropertyGrid* propertyGrid, wxPGProperty* property, wxWindow* control, wxEvent& event ) const
{
	bool signalValueChanged = false;

	if( event.GetEventType() == wxEVT_TEXT_ENTER )
	{
		Cornucopia::VectorE3GANode* vectorNode = 0;
		Cornucopia::BivectorE3GANode* bivectorNode = 0;
		GrabVecOrBivecPointer( property, vectorNode, bivectorNode );

		c3ga::vectorE3GA vector;
		c3ga::bivectorE3GA bivector;
		double* value = 0;

		if( vectorNode )
		{
			vector = vectorNode->GetVector();
			switch( event.GetId() )
			{
				case ID_Component0: value = &vector.m_e1; break;
				case ID_Component1: value = &vector.m_e2; break;
				case ID_Component2: value = &vector.m_e3; break;
			}
		}
		else if( bivectorNode )
		{
			bivector = bivectorNode->GetBivector();
			switch( event.GetId() )
			{
				case ID_Component0: value = &bivector.m_e2_e3; break;
				case ID_Component1: value = &bivector.m_e3_e1; break;
				case ID_Component2: value = &bivector.m_e1_e2; break;
			}
		}

		if( value )
		{
			wxString valueString;
			wxTextCtrl* textCtrl = wxDynamicCast( control, wxTextCtrl );
			if( textCtrl )
			{
				valueString = textCtrl->GetValue();
				if( !valueString.IsEmpty() && valueString.ToDouble( value ) )
				{
					if( vectorNode )
						vectorNode->SetVector( vector, true );
					else if( bivectorNode )
						bivectorNode->SetBivector( bivector, true );
					signalValueChanged = true;
				}

				UpdateControl( property, control->GetParent() );
			}
		}
	}

	return signalValueChanged;
}

//=================================================================================
/*virtual*/ bool VecOrBivecPropertyEditor::GetValueFromControl( wxVariant& variant, wxPGProperty* property, wxWindow* control ) const
{
	// Fake it.
	return true;
}

//=================================================================================
/*virtual*/ void VecOrBivecPropertyEditor::SetValueToUnspecified( wxPGProperty* property, wxWindow* control ) const
{
}

//=================================================================================
/*virtual*/ void VecOrBivecPropertyEditor::OnFocus( wxPGProperty* property, wxWindow* window ) const
{
}

//=================================================================================
void VecOrBivecPropertyEditor::GrabVecOrBivecPointer( wxPGProperty* property, Cornucopia::VectorE3GANode*& vectorNode, Cornucopia::BivectorE3GANode*& bivectorNode ) const
{
	NodeProperty* nodeProperty = NodeProperty::LameCast( property );
	Cornucopia::Node* node = nodeProperty->GetNode();

	vectorNode = node->Cast< Cornucopia::VectorE3GANode >();
	bivectorNode = node->Cast< Cornucopia::BivectorE3GANode >();
}

// VecOrBivecPropertyEditor.cpp