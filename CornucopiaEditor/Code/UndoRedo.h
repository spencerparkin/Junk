// UndoRedo.h

namespace CornucopiaEditor
{
	//=================================================================================
	// An instance of this class is owned by each pane (each of which manages
	// an individual file open by the editor) and is responsible for tracking
	// the history of the file over time so that the user can revert back to
	// earlier states of the Cornucopia tree.  Of course, it can only take responsibility
	// for this task as far as the outside world informs it of whenever a change
	// has been made to the state of the tree.  In a better designed program, undo-redo
	// would be built into the framework so that actions could be taken that edit the
	// file and undo-redo would just work without any effort on the part of newly written
	// code that can edit the file.  But this will work.
	class UndoRedo
	{
	public:

		enum { HISTORY_LIMIT = 50 };

		UndoRedo( Tree* tree );
		~UndoRedo( void );

		bool TakeSnapShot( Panel* panel );

		bool CanUndo( void );
		bool CanRedo( void );

		bool Undo( Panel* panel );
		bool Redo( Panel* panel );

	private:

		enum Direction
		{
			FORWARD,
			BACKWARD,
		};

		bool MoveCurrent( Panel* panel, Direction dir );

		class SnapShot
		{
		public:

			SnapShot( Cornucopia::Node* root );
			~SnapShot( void );

			Cornucopia::Node* root;
			SnapShot* next;
			SnapShot* prev;
		};

		SnapShot* beginning;
		SnapShot* end;
		SnapShot* current;
		int count;
	};
}

// UndoRedo.h