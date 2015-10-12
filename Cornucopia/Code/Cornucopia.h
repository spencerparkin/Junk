// Cornucopia.h

#pragma once

#if defined( CORNUCOPIA_DLL_EXPORTS )
#	define CORNUCOPIA_API __declspec( dllexport )
#elif defined( CORNCUOPIA_DLL_IMPORTS )
#	define CORNUCOPIA_API __declspec( dllimport )
#else
#	define CORNUCOPIA_API
#endif

// OS:
#include <windows.h>		// TODO: Remove in favor of platform-independent stuff.

// I/O:
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <stdarg.h>
#include <sys/stat.h>

// Containers:
#include <string>
#include <map>
#include <list>

// Boost:
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

// Math:
#ifndef M_PI
#	define _USE_MATH_DEFINES
#endif
#include <math.h>
#include "c3ga/c3ga.h"
#include "c3ga/c3ga_util.h"

// Lua:
#include "lua.hpp"

// RTTI:
#include "ClassInfo.h"

// Cornucopia Core:
#include "Path.h"
#include "Error.h"
#include "MetaData.h"
#include "Context.h"
#include "Allocator.h"
#include "Reader.h"
#include "Writer.h"
#include "Node.h"

// Cornucopia Nodes:
#include "Nodes/BooleanNode.h"
#include "Nodes/BoundedFloatNode.h"
#include "Nodes/BoundedFloatPairNode.h"
#include "Nodes/BoundedBoxNode.h"
#include "Nodes/StringNode.h"
#include "Nodes/FileReferenceNode.h"
#include "Nodes/VectorE3GANode.h"
#include "Nodes/BivectorE3GANode.h"
#include "Nodes/ConeNode.h"
#include "Nodes/ColorRGBANode.h"
#include "Nodes/OriginReferenceNode.h"
#include "Nodes/BoundedPointListNode.h"
#include "Nodes/SelectionNode.h"
#include "Nodes/OrientationNode.h"
#include "Nodes/BezierCurveNode.h"
#include "Nodes/BoundedIntegerNode.h"

// Cornucopia.h