// Error.h

#pragma once

class GACOMPUTE_API GACompute::Error
{
public:

	Error( void );
	virtual ~Error( void );

	void Format( const char* format, ... );

	const char* GetString( void ) const;

	class Handler
	{
	public:
		virtual void Handle( const Error* error ) = 0;
	};

	static void Handle( const Error* error );

	static void SetHandler( Handler* handler, bool deleteExisting = true );
	static Handler* GetHandler( void );

private:

	static Handler* errorHandler;

	char* string;
};

// Error.h