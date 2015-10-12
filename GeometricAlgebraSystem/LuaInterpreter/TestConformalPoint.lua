-- TestConformalPoint.lua

package.cpath = package.cpath .. ';C:\\GeometricAlgebraSystem\\Build\\GAExpressionLua\\Debug\\?.dll'

gaexpapi = require 'GAExpressionLua'

dofile( 'C:/GeometricAlgebraSystem/LuaInterpreter/Signature.lua' )

signature_table = c3ga_table
gaexpapi.bilinear_form( signature_func )

local no = gaexpapi.new_vector( 'no' )
local e1 = gaexpapi.new_vector( 'e1' )
local e2 = gaexpapi.new_vector( 'e2' )
local e3 = gaexpapi.new_vector( 'e3' )
local ni = gaexpapi.new_vector( 'ni' )

local v1 = gaexpapi.new_vector( 'v1' )
local v2 = gaexpapi.new_vector( 'v2' )

euclidean_vector_map[ 'v1' ] = true
euclidean_vector_map[ 'v2' ] = true

local p1 = no + v1 + 0.5 * v1 * v1 * ni
local p2 = no + v2 + 0.5 * v2 * v2 * ni

local result = p1 .. p2

print( 'result = ' .. tostring( result ) )

result:expand( gaexpapi.SUM_OF_BLADES )

print( 'result = ' .. tostring( result ) )

-- TestConformalPoint.lua