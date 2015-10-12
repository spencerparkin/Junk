// BoundedFloatPairPropertyEditor.cpp

#include "../CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
wxIMPLEMENT_DYNAMIC_CLASS( BoundedFloatPairPropertyEditor, CompanionedTextPropertyEditor );

//=================================================================================
BoundedFloatPairPropertyEditor::BoundedFloatPairPropertyEditor( void )
{
}

//=================================================================================
/*virtual*/ BoundedFloatPairPropertyEditor::~BoundedFloatPairPropertyEditor( void )
{
}

//=================================================================================
/*virtual*/ bool BoundedFloatPairPropertyEditor::OnEvent( wxPropertyGrid* propertyGrid, wxPGProperty* property, wxWindow* control, wxEvent& event ) const
{
	if( CompanionedTextPropertyEditor::OnEvent( propertyGrid, property, control, event ) )
		return true;

	wxTextCtrl* textCtrl = wxDynamicCast( control, wxTextCtrl );
	if( textCtrl )
	{
		if( event.GetEventType() == wxEVT_SET_FOCUS )
		{
			Cornucopia::BoundedFloatPairNode* boundedFloaPairNode = GetBoundedFloatPairNode( property );
			Cornucopia::BoundedFloatPairNode::Data data;
			boundedFloaPairNode->Get( data );
			textCtrl->SetValue( wxString::Format( "%1.2f, %1.2f", data.minValue, data.maxValue ) );
			textCtrl->SelectAll();
		}
	}

	return false;
}

//=================================================================================
wxString BoundedFloatPairPropertyEditor::MakeValueText( wxPGProperty* property ) const
{
	Cornucopia::BoundedFloatPairNode* boundedFloatPairNode = GetBoundedFloatPairNode( property );
	Cornucopia::BoundedFloatPairNode::Data data;
	boundedFloatPairNode->Get( data );

	wxString valueText = wxString::Format( "%1.2f <= %1.2f <= %1.2f <= %1.2f", data.min, data.minValue, data.maxValue, data.max );
	return valueText;
}

//=================================================================================
/*virtual*/ void BoundedFloatPairPropertyEditor::DrawValue( wxDC& dc, const wxRect& rect, wxPGProperty* property, const wxString& text ) const
{
	// TODO: Why grey?
	wxString valueText = MakeValueText( property );
	dc.DrawText( valueText, rect.x + wxPG_XBEFORETEXT, rect.y );
}

//=================================================================================
Cornucopia::BoundedFloatPairNode* BoundedFloatPairPropertyEditor::GetBoundedFloatPairNode( wxPGProperty* property ) const
{
	Cornucopia::BoundedFloatPairNode* boundedFloatPairNode = 0;
	NodeProperty* nodeProperty = NodeProperty::LameCast( property );
	if( nodeProperty )
	{
		Cornucopia::Node* node = nodeProperty->GetNode();
		boundedFloatPairNode = node->Cast< Cornucopia::BoundedFloatPairNode >();
	}
	return boundedFloatPairNode;
}

//=================================================================================
/*virtual*/ wxControl* BoundedFloatPairPropertyEditor::CreateCompanionControl( wxWindow* parent ) const
{
	TwinSliderControl* twinSlider = new TwinSliderControl( parent, wxID_ANY, 0, 10000, 0, 0, wxDefaultPosition, wxDefaultSize );
	twinSlider->SetTickFrequency(1);
	return twinSlider;
}

//=================================================================================
/*virtual*/ bool BoundedFloatPairPropertyEditor::IsCompanionControl( wxWindow* window ) const
{
	if( wxDynamicCast( window, TwinSliderControl ) != 0 )
		return true;
	return false;
}

//=================================================================================
/*virtual*/ bool BoundedFloatPairPropertyEditor::IsCompanionControlValueChangedEvent( wxEvent& event ) const
{
	if( event.GetEventType() == TWIN_SLIDER_EVENT_LEFT_VALUE_CHANGED || event.GetEventType() == TWIN_SLIDER_EVENT_RIGHT_VALUE_CHANGED )
		return true;
	return false;
}

//=================================================================================
/*virtual*/ bool BoundedFloatPairPropertyEditor::IsCompanionControlScrubbing( wxEvent& event ) const
{
	if( event.GetEventType() == TWIN_SLIDER_EVENT_LEFT_VALUE_CHANGED || event.GetEventType() == TWIN_SLIDER_EVENT_RIGHT_VALUE_CHANGED )
	{
		SliderEvent* sliderEvent = ( SliderEvent* )&event;
		return sliderEvent->GetScrubbing();
	}
	return false;
}

//=================================================================================
/*virtual*/ void BoundedFloatPairPropertyEditor::UpdateCompanionControlUsingProperty( wxPGProperty* property, wxControl* control ) const
{
	TwinSliderControl* twinSlider = wxDynamicCast( control, TwinSliderControl );

	Cornucopia::BoundedFloatPairNode* boundedFloatPairNode = GetBoundedFloatPairNode( property );
	Cornucopia::BoundedFloatPairNode::Data data;
	boundedFloatPairNode->Get( data );

	double leftLerp = ( data.minValue - data.min ) / ( data.max - data.min );
	double rightLerp = ( data.maxValue - data.min ) / ( data.max - data.min );

	int sliderMinValue = twinSlider->GetMinValue();
	int sliderMaxValue = twinSlider->GetMaxValue();

	int sliderLeftValue = sliderMinValue + int( leftLerp * double( sliderMaxValue - sliderMinValue ) );
	int sliderRightValue = sliderMinValue + int( rightLerp * double( sliderMaxValue - sliderMinValue ) );

	bool valuesSet = twinSlider->SetValues( sliderLeftValue, sliderRightValue );
	wxASSERT( valuesSet );
}

//=================================================================================
/*virtual*/ void BoundedFloatPairPropertyEditor::UpdateTextCtrlUsingProperty( wxPGProperty* property, wxTextCtrl* textCtrl ) const
{
	wxString valueText = MakeValueText( property );
	textCtrl->SetValue( valueText );
}

//=================================================================================
/*virtual*/ void BoundedFloatPairPropertyEditor::UpdatePropertyUsingCompanionControl( wxPGProperty* property, wxControl* control ) const
{
	TwinSliderControl* twinSlider = wxDynamicCast( control, TwinSliderControl );

	Cornucopia::BoundedFloatPairNode* boundedFloatPairNode = GetBoundedFloatPairNode( property );
	Cornucopia::BoundedFloatPairNode::Data data;
	boundedFloatPairNode->Get( data );

	int sliderMinValue = twinSlider->GetMinValue();
	int sliderMaxValue = twinSlider->GetMaxValue();

	int sliderLeftValue = twinSlider->GetLeftValue();
	int sliderRightValue = twinSlider->GetRightValue();

	double leftLerp = double( sliderLeftValue - sliderMinValue ) / double( sliderMaxValue - sliderMinValue );
	double rightLerp = double( sliderRightValue - sliderMinValue ) / double( sliderMaxValue - sliderMinValue );

	data.minValue = data.min + leftLerp * ( data.max - data.min );
	data.maxValue = data.min + rightLerp * ( data.max - data.min );

	boundedFloatPairNode->Set( data );
}

//=================================================================================
/*virtual*/ void BoundedFloatPairPropertyEditor::UpdatePropertyUsingTextCtrl( wxPGProperty* property, wxTextCtrl* textCtrl ) const
{
	Cornucopia::BoundedFloatPairNode* boundedFloaPairNode = GetBoundedFloatPairNode( property );
	Cornucopia::BoundedFloatPairNode::Data data;
	boundedFloaPairNode->Get( data );

	wxString text = textCtrl->GetValue();
	
	// TODO: Implement this.

	//boundedFloaPairNode->Set( data );
}

// BoundedFloatPairPropertyEditor.cpp