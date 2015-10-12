// UndoRedo.cpp

#include "CornucopiaEditor.h"

using namespace CornucopiaEditor;

//=================================================================================
UndoRedo::SnapShot::SnapShot( Cornucopia::Node* root )
{
	this->root = root;
	next = 0;
	prev = 0;
}

//=================================================================================
UndoRedo::SnapShot::~SnapShot( void )
{
	if( root )
		delete root;
}

//=================================================================================
UndoRedo::UndoRedo( Tree* tree )
{
	Cornucopia::Node* root = tree->GetRoot();
	Cornucopia::Node* clone = 0;
	if( root )
	{
		Cornucopia::Node::CopyParameters copyParameters;
		clone = root->Clone( wxGetApp().GetContext(), copyParameters );
	}

	SnapShot* snapShot = new SnapShot( clone );
	beginning = snapShot;
	end = snapShot;
	current = snapShot;

	count = 1;
}

//=================================================================================
UndoRedo::~UndoRedo( void )
{
	SnapShot* snapShot = beginning;
	while( snapShot )
	{
		SnapShot* doomed = snapShot;
		snapShot = snapShot->next;
		delete doomed;
	}
}

//=================================================================================
bool UndoRedo::TakeSnapShot( Panel* panel )
{
	// When a snap-shot is taken, the user forfeits the ability
	// to redo anything that is currently redo-able.
	while( current != end )
	{
		SnapShot* doomed = end;
		end = end->prev;
		end->next = 0;
		delete doomed;
		count--;
	}

	Tree* tree = panel->GetTree();
	Cornucopia::Node* root = tree->GetRoot();
	Cornucopia::Node* clone = 0;
	if( root )
	{
		Cornucopia::Node::CopyParameters copyParameters;
		clone = root->Clone( wxGetApp().GetContext(), copyParameters );
	}

	current = new SnapShot( clone );
	end->next = current;
	current->prev = end;
	end = current;
	count++;

	if( count > HISTORY_LIMIT )
	{
		SnapShot* doomed = beginning;
		beginning = beginning->next;
		beginning->prev = 0;
		delete doomed;
		count--;
	}

	return true;
}

//=================================================================================
bool UndoRedo::CanUndo( void )
{
	if( current->prev )
		return true;
	return false;
}

//=================================================================================
bool UndoRedo::CanRedo( void )
{
	if( current->next )
		return true;
	return false;
}

//=================================================================================
bool UndoRedo::Undo( Panel* panel )
{
	return MoveCurrent( panel, BACKWARD );
}

//=================================================================================
bool UndoRedo::Redo( Panel* panel )
{
	return MoveCurrent( panel, FORWARD );
}

//=================================================================================
bool UndoRedo::MoveCurrent( Panel* panel, Direction dir )
{
	if( ( dir == FORWARD && !CanRedo() ) || ( dir == BACKWARD && !CanUndo() ) )
		return false;

	Cornucopia::Node* forgottenTree = current->root;

	if( dir == FORWARD )
		current = current->next;
	else if( dir == BACKWARD )
		current = current->prev;

	Tree* tree = panel->GetTree();
	Cornucopia::Node* oldRoot = tree->GetRoot();
	Cornucopia::Node* newRoot = current->root;

	if( newRoot )
	{
		Cornucopia::Node::CopyParameters copyParameters;
		newRoot = newRoot->Clone( wxGetApp().GetContext(), copyParameters );
		if( !newRoot )
			return false;
	}

	panel->PreUndoRedoNotice( newRoot, forgottenTree );

	tree->SetRoot(0);
	tree->SetRoot( newRoot );

	panel->PostUndoRedoNotice( oldRoot, forgottenTree );

	if( oldRoot )
		delete oldRoot;

	return true;
}

// UndoRedo.cpp