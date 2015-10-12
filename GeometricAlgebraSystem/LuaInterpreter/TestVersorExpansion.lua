-- TestVersorExpansion.lua

package.cpath = package.cpath .. ';C:\\GeometricAlgebraSystem\\Build\\GAExpressionLua\\Debug\\?.dll'

gaexpapi = require 'GAExpressionLua'

local v1 = gaexpapi.new_vector( 'v1' )
local v2 = gaexpapi.new_vector( 'v2' )
local v3 = gaexpapi.new_vector( 'v3' )
local v4 = gaexpapi.new_vector( 'v4' )
local v5 = gaexpapi.new_vector( 'v5' )
local v6 = gaexpapi.new_vector( 'v6' )

local r1 = v1
local r2 = v1 * v2
local r3 = v1 * v2 * v3
local r4 = v1 * v2 * v3 * v4
local r5 = v1 * v2 * v3 * v4 * v5
local r6 = v1 * v2 * v3 * v4 * v5 * v6

function printExpansion( r, target )
	local r_expanded = r:clone()
	r_expanded:expand( target )
	print( tostring(r) .. ' = ' .. tostring( r_expanded ) .. '\n' )
	return r_expanded
end

printExpansion( r1, gaexpapi.SUM_OF_BLADES )
printExpansion( r2, gaexpapi.SUM_OF_BLADES )
printExpansion( r3, gaexpapi.SUM_OF_BLADES )
printExpansion( r4, gaexpapi.SUM_OF_BLADES )
printExpansion( r5, gaexpapi.SUM_OF_BLADES )
printExpansion( r6, gaexpapi.SUM_OF_BLADES )

-- TestVersorExpansion.lua