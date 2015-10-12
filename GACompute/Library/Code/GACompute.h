// GACompute.h

#pragma once

#if defined EXPORT_GACOMPUTE
#	define GACOMPUTE_API		__declspec( dllexport )
#elif defined IMPORT_GACOMPUTE
#	define GACOMPUTE_API		__declspec( dllimport )
#else
#	define GACOMPUTE_API
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <typeinfo>
#include <ctype.h>

#include "List.h"
#include "AutoDelete.h"

namespace GACompute
{
	class Error;
	class BilinearForm;
	class Operand;
	class Operation;
	class Sum;
	class GeometricProduct;
	class InnerProduct;
	class OuterProduct;
	class Vector;
	class NumericScalar;
	class SymbolicScalar;
	class InnerProductScalar;
}

#include "Error.h"
#include "BilinearForm.h"
#include "Operand.h"
#include "Operation.h"
#include "Sum.h"
#include "GeometricProduct.h"
#include "InnerProduct.h"
#include "OuterProduct.h"
#include "Vector.h"
#include "NumericScalar.h"
#include "SymbolicScalar.h"
#include "InnerProductScalar.h"

// GACompute.h