// CompanionedTextPropertyEditor.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
wxIMPLEMENT_ABSTRACT_CLASS( CompanionedTextPropertyEditor, wxPGEditor );

//=================================================================================
CompanionedTextPropertyEditor::CompanionedTextPropertyEditor( void )
{
}

//=================================================================================
/*virtual*/ CompanionedTextPropertyEditor::~CompanionedTextPropertyEditor( void )
{
}

//=================================================================================
/*virtual*/ wxPGWindowList CompanionedTextPropertyEditor::CreateControls( wxPropertyGrid* propertyGrid, wxPGProperty* property, const wxPoint& pos, const wxSize& size ) const
{
	wxPanel* panel = new wxPanel( propertyGrid, wxID_ANY, pos, size );

	wxTextCtrl* textCtrl = new wxTextCtrl( panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 200, size.GetHeight() ), wxTE_CENTRE | wxTE_PROCESS_ENTER );
	wxControl* control = CreateCompanionControl( panel );

	wxBoxSizer* boxSizer = new wxBoxSizer( wxHORIZONTAL );
	boxSizer->Add( textCtrl, 0, wxALL, 0 );
	boxSizer->Add( control, 1, wxGROW | wxALL, 0 );
	panel->SetSizer( boxSizer );

	UpdateControl( property, textCtrl );
	UpdateControl( property, control );

	property->SetAttribute( wxT( "TextCtrl" ), wxVariant( ( void* )textCtrl ) );
	property->SetAttribute( wxT( "CompanionControl" ), wxVariant( ( void* )control ) );

	panel->Layout();
	return panel;
}

//=================================================================================
/*virtual*/ void CompanionedTextPropertyEditor::UpdateControl( wxPGProperty* property, wxWindow* control ) const
{
	wxPanel* panel = wxDynamicCast( control, wxPanel );
	wxTextCtrl* textCtrl = wxDynamicCast( control, wxTextCtrl );
	bool isCompanion = IsCompanionControl( control );
	
	if( panel )
	{
		wxWindowList& windowList = panel->GetChildren();
		for( wxWindowList::iterator iter = windowList.begin(); iter != windowList.end(); iter++ )
		{
			wxWindow* child = *iter;
			UpdateControl( property, child );
		}
	}
	else if( isCompanion )
		UpdateCompanionControlUsingProperty( property, wxDynamicCast( control, wxControl ) );
	else if( textCtrl )
		UpdateTextCtrlUsingProperty( property, textCtrl );
}

//=================================================================================
void CompanionedTextPropertyEditor::UpdateProperty( wxPGProperty* property, wxWindow* control ) const
{
	wxPanel* panel = wxDynamicCast( control, wxPanel );
	wxTextCtrl* textCtrl = wxDynamicCast( control, wxTextCtrl );
	bool isCompanion = IsCompanionControl( control );

	if( panel )
	{
		wxWindowList& windowList = panel->GetChildren();
		for( wxWindowList::iterator iter = windowList.begin(); iter != windowList.end(); iter++ )
		{
			wxWindow* child = *iter;
			UpdateProperty( property, child );
		}
	}
	else if( isCompanion )
		UpdatePropertyUsingCompanionControl( property, wxDynamicCast( control, wxControl ) );
	else if( textCtrl )
		UpdatePropertyUsingTextCtrl( property, textCtrl );
}

//=================================================================================
/*virtual*/ void CompanionedTextPropertyEditor::DrawValue( wxDC& dc, const wxRect& rect, wxPGProperty* property, const wxString& text ) const
{
	return wxPGEditor::DrawValue( dc, rect, property, text );
}

//=================================================================================
// Our return value here indicates whether the property value was modified.
/*virtual*/ bool CompanionedTextPropertyEditor::OnEvent( wxPropertyGrid* propertyGrid, wxPGProperty* property, wxWindow* control, wxEvent& event ) const
{
	bool scrubbing = false;
	bool signalValueChanged = false;

	bool isCompanion = IsCompanionControl( control );
	if( isCompanion )
	{
		if( IsCompanionControlValueChangedEvent( event ) )
		{
			wxVariant variant = property->GetAttribute( wxT( "TextCtrl" ) );
			wxTextCtrl* textCtrl = ( wxTextCtrl* )variant.GetVoidPtr();

			UpdatePropertyUsingCompanionControl( property, wxDynamicCast( control, wxControl ) );
			UpdateTextCtrlUsingProperty( property, textCtrl );

			// When we're scrubbing, the value is changing, but we don't want to signal
			// a value change, which would causes us to advance the undo/redo system, etc.
			scrubbing = IsCompanionControlScrubbing( event );
			signalValueChanged = !scrubbing;
		}
	}

	wxTextCtrl* textCtrl = wxDynamicCast( control, wxTextCtrl );
	if( textCtrl )
	{
		if( event.GetEventType() == wxEVT_TEXT_ENTER )
		{
			wxVariant variant = property->GetAttribute( wxT( "CompanionControl" ) );
			wxControl* control = ( wxControl* )variant.GetVoidPtr();

			UpdatePropertyUsingTextCtrl( property, textCtrl );
			UpdateCompanionControlUsingProperty( property, control );

			signalValueChanged = true;
		}
	}

	// When a value change is signaled, a tweak event will be sent,
	// but we also want to send such events when we're scrubbing.
	if( scrubbing )
	{
		Panel* panel = wxDynamicCast( propertyGrid->GetParent(), Panel );
		if( panel )
		{
			NodeProperty* nodeProperty = NodeProperty::LameCast( property );
			Cornucopia::Node* node = nodeProperty->GetNode();
			panel->SendLiveTweakEvent( node, true );
		}
	}

	// By returning true here, a message will be sent to the property grid telling it that a value has changed.
	// That handler will update the undo/redo system, among other things.
	return signalValueChanged;
}

//=================================================================================
/*virtual*/ bool CompanionedTextPropertyEditor::GetValueFromControl( wxVariant& variant, wxPGProperty* property, wxWindow* control ) const
{
	return true;
}

//=================================================================================
/*virtual*/ void CompanionedTextPropertyEditor::SetValueToUnspecified( wxPGProperty* property, wxWindow* control ) const
{
	//UpdateControl( property, control );
}

//=================================================================================
/*virtual*/ void CompanionedTextPropertyEditor::OnFocus( wxPGProperty* property, wxWindow* window ) const
{
	wxPGEditor::OnFocus( property, window );
}

// SliderPropertyEditor.cpp