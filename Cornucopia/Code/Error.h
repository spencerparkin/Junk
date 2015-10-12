// Error.h

namespace Cornucopia
{
	class Error
	{
	public:

		Error( const char* errorMessageFormat, ... );
		Error( const char* errorMessageFormat, va_list args );
		virtual ~Error( void );

		const std::string& ErrorMessage( void ) const;

	private:

		void FormatErrorMessage( const char* errorMessageFormat, va_list args );

		std::string errorMessage;
	};
}

// Error.h