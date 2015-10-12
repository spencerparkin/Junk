// BoundedPointListNode.h

namespace Cornucopia
{
	//=================================================================================
	// This class represents a set of one or more points in the XY-plane.
	// Derivatives of this class might include a class that interprets the
	// vertices as a convex or concave polygon, or a class that interprets
	// and interpolates the points as a Bezier curve.
	class CORNUCOPIA_API BoundedPointListNode : public Node
	{
		DECL_CORNUCOPIA_CLASSINFO( BoundedPointListNode );

	public:

		BoundedPointListNode( void );
		virtual ~BoundedPointListNode( void );

		virtual bool ReadFromTable( lua_State* L, Context& context ) override;
		virtual bool WriteToTable( lua_State* L, Context& context ) const override;

		virtual bool Copy( const Node* node, Context& context, const CopyParameters& copyParameters ) override;

		virtual std::string GetValueToDisplayString( void ) const override;
		virtual ValueStringError GetValueToString( std::string& valueString ) const override;
		virtual ValueStringError SetValueFromString( const std::string& valueString ) override;
		virtual ValueStringError GetValueAsStringHelp( std::string& helpString ) const override;
		
		// If set as unbounded, the user can add points without worry of out-of-bounds rejection.
		bool SetBounded( bool bounded );
		bool IsBounded( void ) const;

		// We support a rectangular sub-region of the XY-plane as bounds for the point-list.
		struct Bounds
		{
			c3ga::vectorE2GA min;
			c3ga::vectorE2GA max;

			bool Contains( const c3ga::vectorE2GA& point ) const;
			bool IsValid( void ) const;
			void MinimallyExpandToIncludePoint( const c3ga::vectorE2GA& point );
			bool operator==( const Bounds& bounds ) const;
		};

		bool FindTightestBounds( Bounds& tightestBounds ) const;

		bool SetBounds( const Bounds& givenBounds );
		const Bounds& GetBounds( void ) const;

		bool AllPointsInBounds( const Bounds& givenBounds ) const;
			
		typedef std::list< c3ga::vectorE2GA > List;

		// Users of the class may want to work exclusively with indices, because
		// iterators can become invalid if the node (and so the list) goes out of scope.
		List::const_iterator IndexToIterator( int index ) const;
		int IndexFromIterator( List::const_iterator iter ) const;

		// A derived class may impose restrictions on the insertion and removal of points, and may even
		// deny such operations altogether if it is meant to manipulate only a certain number of points.
		virtual bool InsertPoint( const c3ga::vectorE2GA& point, List::const_iterator* insertBeforeIter = 0, bool expandIfNeeded = false );
		virtual bool RemovePoint( List::const_iterator& removeIter );
		virtual bool RemoveNearestPoint( const c3ga::vectorE2GA& point, double maxSquareRadius = 0.0 );
		virtual bool SetPoint( const c3ga::vectorE2GA& point, List::const_iterator& setAtIter, bool expandIfNeeded = false );

		List::const_iterator FindNearestPoint( const c3ga::vectorE2GA& point, double maxSquareRadius = -1.0 ) const;

		const List& GetPointList( void ) const;

	protected:

		List* pointList;
		Bounds bounds;
		bool bounded;
	};
}

// BoundedPointListNode.h