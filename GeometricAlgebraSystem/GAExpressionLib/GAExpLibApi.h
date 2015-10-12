// GAExpLibApi.h

#pragma once

#if defined GA_EXPRESSION_LIB_API_EXPORT
#	define GA_EXPRESSION_LIB_API		__declspec( dllexport )
#elif defined GA_EXPRESSION_LIB_API_IMPORT
#	define GA_EXPRESSION_LIB_API		__declspec( dllimport )
#else
#	define GA_EXPRESSION_LIB_API
#endif

// GAExpLibApi.h