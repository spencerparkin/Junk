// BezierCurveNode.h

namespace Cornucopia
{
	//=================================================================================
	class CORNUCOPIA_API BezierCurveNode : public BoundedPointListNode
	{
		DECL_CORNUCOPIA_CLASSINFO( BezierCurveNode );

	public:

		BezierCurveNode( void );
		virtual ~BezierCurveNode( void );

		virtual bool ReadFromTable( lua_State* L, Context& context ) override;
		virtual bool WriteToTable( lua_State* L, Context& context ) const override;

		virtual bool Copy( const Node* node, Context& context, const CopyParameters& copyParameters ) override;

		virtual std::string GetValueToDisplayString( void ) const override;
		virtual ValueStringError GetValueToString( std::string& valueString ) const override;
		virtual ValueStringError SetValueFromString( const std::string& valueString ) override;
		virtual ValueStringError GetValueAsStringHelp( std::string& helpString ) const override;

		virtual bool InsertPoint( const c3ga::vectorE2GA& point, List::const_iterator* insertBeforeIter = 0, bool expandIfNeeded = false ) override;
		virtual bool RemovePoint( List::const_iterator& removeIter ) override;
		virtual bool RemoveNearestPoint( const c3ga::vectorE2GA& point, double maxSquareRadius = 0.0 ) override;
		virtual bool SetPoint( const c3ga::vectorE2GA& point, List::const_iterator& setAtIter, bool expandIfNeeded = false ) override;

		bool IncrementDegree( void );		// Non-destructive.  Preserves parameterization and entire curve shape.
		bool DecrementDegree( void );		// Destructive!  Preserves neither!

		int GetDegree( void ) const;

		bool Evaluate( double t, c3ga::vectorE2GA& point ) const;
		bool EvaluateDerivative( double t, c3ga::vectorE2GA& point ) const;

		// Assuming that the curve control points are such that the Bezier forms a well
		// defined function over the X-domain, calculate a y-value using the given x-value.
		// Newton iteration is used to calculate the inverse Bezier function.
		bool Evaluate( double x, double& y, int newtonIters ) const;

		enum
		{
			CONSTRAIN_AS_FUNCTION									= 1,
			CONSTRAIN_FIRST_AND_LAST_POINTS_TO_V_BOUNDARIES			= 2,
			CONSTRAIN_FIRST_AND_LAST_POINTS_TO_H_BOUNDARIES			= 4,
		};

		int GetConstraintFlags( void ) const;
		void SetConstraintFlags( int constraintFlags );

	protected:
		
		int constraintFlags;

		void ApplyConstraints( void );

		typedef double ( BezierCurveNode::* CoeficientFunction )( int ) const;

		double CoefFuncForX( int index ) const;
		double CoefFuncForY( int index ) const;

		mutable std::vector< c3ga::vectorE2GA >* pointArrayCache;
		mutable bool pointArrayCacheValid;
		void RepopulatePointArrayCacheIfNeeded( void ) const;

		double DeCasteljuaEvaluate( int a, int b, double t, CoeficientFunction coefFunc ) const;
		double DeCasteljuaEvaluateDerivative( int a, int b, double t, CoeficientFunction coefFunc ) const;
	};
}

// BezierCurveNode.h