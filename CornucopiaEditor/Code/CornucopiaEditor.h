// CornucopiaEditor.h

#pragma once

// wxWidgets:
#include "wxHeaders.h"

// OpenGL:
#include <GL/glu.h>

// Cornucopia Library:
#include "Cornucopia.h"

// Cornucopia Plugin Library:
#include "Plugin.h"

// Cornucopia Editor Classes:
namespace CornucopiaEditor
{
	template< typename Type > class HandleSystem;

	class ResourceManager;
	class Application;
	class Frame;
	class Tree;
	class NodeProperty;
	class NodeControlPanel;
	class Panel;
	class DockingPanel;
	class ControlTreePanel;
	class PropertyGridPanel;
	class DataViewCtrl;
	class DataViewModel;
	class NoteBook;
	class FileDropTarget;
	class EditorMetaData;
	class UndoRedo;
	class Canvas;
	class DataObject;
	class Upgrader;
	class HelpPanel;
	class PluginManager;
	class PluginPanel;
}

// Widgets:
#include "Widgets/SliderControlBase.h"
#include "Widgets/LoneSliderControl.h"
#include "Widgets/TwinSliderControl.h"

// Cornucopia Editor:
#include "Handle.h"
#include "ResourceManager.h"
#include "Application.h"
#include "Frame.h"
#include "Tree.h"
#include "NodeProperty.h"
#include "NodeControlPanel.h"
#include "Panel.h"
#include "DockingPanel.h"
#include "ControlTreePanel.h"
#include "PropertyGridPanel.h"
#include "DataViewCtrl.h"
#include "DataViewModel.h"
#include "NoteBook.h"
#include "FileDropTarget.h"
#include "EditorMetaData.h"
#include "UndoRedo.h"
#include "Canvas.h"
#include "DataObject.h"
#include "Upgrader.h"
#include "Help.h"
#include "PluginManager.h"
#include "PluginPanel.h"

// Cornucopia Node Control Panels:
#include "NodeControlpanels/BaseNodeControlPanel.h"
#include "NodeControlPanels/BooleanNodeControlPanel.h"
#include "NodeControlPanels/StringNodeControlPanel.h"
#include "NodeControlPanels/BoundedFloatNodeControlPanel.h"
#include "NodeControlPanels/BoundedFloatPairNodeControlPanel.h"
#include "NodeControlpanels/BoundedPointListNodeControlPanel.h"
#include "NodeControlPanels/FileReferenceNodeControlPanel.h"
#include "NodeControlPanels/VecOrBivecNodeControlPanel.h"
#include "NodeControlPanels/ConeNodeControlPanel.h"
#include "NodeControlPanels/ColorRGBANodeControlPanel.h"
#include "NodeControlPanels/SelectionNodeControlPanel.h"
#include "NodeControlPanels/OrientationNodeControlPanel.h"
#include "NodeControlPanels/BezierCurveNodeControlPanel.h"
#include "NodeControlPanels/BoundedIntegerNodeControlPanel.h"
#include "NodeControlPanels/BoundedBoxNodeControlPanel.h"

// Property Editors:
#include "PropertyEditors/CompanionedTextPropertyEditor.h"
#include "PropertyEditors/BoundedFloatPropertyEditor.h"
#include "PropertyEditors/BoundedFloatPairPropertyEditor.h"
#include "PropertyEditors/VecOrBivecPropertyEditor.h"

// Properties:
#include "Properties/CommonProperties.h"
#include "Properties/BoundedFloatProperty.h"
#include "Properties/BoundedFloatPairProperty.h"
#include "Properties/VecOrBivecProperty.h"

// CornucopiaEditor.h