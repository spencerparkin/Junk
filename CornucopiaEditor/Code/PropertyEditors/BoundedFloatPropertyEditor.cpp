// BoundedFloatPropertyEditor.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
wxIMPLEMENT_DYNAMIC_CLASS( BoundedFloatPropertyEditor, CompanionedTextPropertyEditor );

//=================================================================================
BoundedFloatPropertyEditor::BoundedFloatPropertyEditor( void )
{
}

//=================================================================================
/*virtual*/ BoundedFloatPropertyEditor::~BoundedFloatPropertyEditor( void )
{
}

//=================================================================================
/*virtual*/ bool BoundedFloatPropertyEditor::OnEvent( wxPropertyGrid* propertyGrid, wxPGProperty* property, wxWindow* control, wxEvent& event ) const
{
	if( CompanionedTextPropertyEditor::OnEvent( propertyGrid, property, control, event ) )
		return true;

	wxTextCtrl* textCtrl = wxDynamicCast( control, wxTextCtrl );
	if( textCtrl )
	{
		if( event.GetEventType() == wxEVT_SET_FOCUS )
		{
			Cornucopia::BoundedFloatNode* boundedFloatNode = GetBoundedFloatNode( property );
			Cornucopia::BoundedFloatNode::Data data;
			boundedFloatNode->Get( data );
			textCtrl->SetValue( wxString::Format( "%1.2f", data.value ) );
			textCtrl->SelectAll();
		}
	}

	return false;
}

//=================================================================================
wxString BoundedFloatPropertyEditor::MakeValueText( wxPGProperty* property ) const
{
	Cornucopia::BoundedFloatNode* boundedFloatNode = GetBoundedFloatNode( property );
	Cornucopia::BoundedFloatNode::Data data;
	boundedFloatNode->Get( data );

	wxString valueText = wxString::Format( "%1.2f <= %1.2f <= %1.2f", data.min, data.value, data.max );
	return valueText;
}

//=================================================================================
/*virtual*/ void BoundedFloatPropertyEditor::DrawValue( wxDC& dc, const wxRect& rect, wxPGProperty* property, const wxString& text ) const
{
	// TODO: Why grey?
	wxString valueText = MakeValueText( property );
	dc.DrawText( valueText, rect.x + wxPG_XBEFORETEXT, rect.y );
}

//=================================================================================
Cornucopia::BoundedFloatNode* BoundedFloatPropertyEditor::GetBoundedFloatNode( wxPGProperty* property ) const
{
	Cornucopia::BoundedFloatNode* boundedFloatNode = 0;
	NodeProperty* nodeProperty = NodeProperty::LameCast( property );
	if( nodeProperty )
	{
		Cornucopia::Node* node = nodeProperty->GetNode();
		boundedFloatNode = node->Cast< Cornucopia::BoundedFloatNode >();
	}
	return boundedFloatNode;
}

//=================================================================================
/*virtual*/ wxControl* BoundedFloatPropertyEditor::CreateCompanionControl( wxWindow* parent ) const
{
	LoneSliderControl* loneSlider = new LoneSliderControl( parent, wxID_ANY, 0, 10000, 0, wxDefaultPosition, wxDefaultSize, 0 );
	return loneSlider;
}

//=================================================================================
/*virtual*/ bool BoundedFloatPropertyEditor::IsCompanionControl( wxWindow* window ) const
{
	if( wxDynamicCast( window, LoneSliderControl ) != 0 )
		return true;
	return false;
}

//=================================================================================
/*virtual*/ bool BoundedFloatPropertyEditor::IsCompanionControlValueChangedEvent( wxEvent& event ) const
{
	if( event.GetEventType() == LONE_SLIDER_EVENT_VALUE_CHANGED )
		return true;
	return false;
}

//=================================================================================
/*virtual*/ bool BoundedFloatPropertyEditor::IsCompanionControlScrubbing( wxEvent& event ) const
{
	if( event.GetEventType() == LONE_SLIDER_EVENT_VALUE_CHANGED )
	{
		SliderEvent* sliderEvent = ( SliderEvent* )&event;
		return sliderEvent->GetScrubbing();
	}
	return false;
}

//=================================================================================
/*virtual*/ void BoundedFloatPropertyEditor::UpdateCompanionControlUsingProperty( wxPGProperty* property, wxControl* control ) const
{
	LoneSliderControl* loneSlider = wxDynamicCast( control, LoneSliderControl );

	Cornucopia::BoundedFloatNode* boundedFloatNode = GetBoundedFloatNode( property );
	Cornucopia::BoundedFloatNode::Data data;
	boundedFloatNode->Get( data );

	double lerp = ( data.value - data.min ) / ( data.max - data.min );
	int sliderMinValue = loneSlider->GetMinValue();
	int sliderMaxValue = loneSlider->GetMaxValue();
	int sliderValue = sliderMinValue + int( lerp * double( sliderMaxValue - sliderMinValue ) );
	loneSlider->SetValue( sliderValue );
}

//=================================================================================
/*virtual*/ void BoundedFloatPropertyEditor::UpdateTextCtrlUsingProperty( wxPGProperty* property, wxTextCtrl* textCtrl ) const
{
	wxString valueText = MakeValueText( property );
	textCtrl->SetValue( valueText );
}

//=================================================================================
/*virtual*/ void BoundedFloatPropertyEditor::UpdatePropertyUsingCompanionControl( wxPGProperty* property, wxControl* control ) const
{
	LoneSliderControl* loneSlider = wxDynamicCast( control, LoneSliderControl );

	Cornucopia::BoundedFloatNode* boundedFloatNode = GetBoundedFloatNode( property );
	Cornucopia::BoundedFloatNode::Data data;
	boundedFloatNode->Get( data );

	int sliderMinValue = loneSlider->GetMinValue();
	int sliderMaxValue = loneSlider->GetMaxValue();
	int sliderValue = loneSlider->GetValue();
	double lerp = double( sliderValue - sliderMinValue ) / double( sliderMaxValue - sliderMinValue );
	data.value = data.min + lerp * ( data.max - data.min );

	boundedFloatNode->Set( data );
}

//=================================================================================
/*virtual*/ void BoundedFloatPropertyEditor::UpdatePropertyUsingTextCtrl( wxPGProperty* property, wxTextCtrl* textCtrl ) const
{
	Cornucopia::BoundedFloatNode* boundedFloatNode = GetBoundedFloatNode( property );
	Cornucopia::BoundedFloatNode::Data data;
	boundedFloatNode->Get( data );

	wxString text = textCtrl->GetValue();
	if( !text.ToDouble( &data.value ) )
		data.value = 0.5 * ( data.min + data.max );

	if( data.value < data.min )
		data.value = data.min;
	if( data.value > data.max )
		data.value = data.max;

	boundedFloatNode->Set( data );
}

// BoundedFloatPropertyEditor.cpp