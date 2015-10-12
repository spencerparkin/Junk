// FileDropTarget.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
FileDropTarget::FileDropTarget( Frame* frame )
{
	this->frame = frame;
}

//=================================================================================
/*virtual*/ FileDropTarget::~FileDropTarget( void )
{
}

//=================================================================================
/*virtual*/ bool FileDropTarget::OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& fileNames )
{
	frame->HandleDroppedFiles( fileNames );
	return true;
}

// FileDropTarget.cpp