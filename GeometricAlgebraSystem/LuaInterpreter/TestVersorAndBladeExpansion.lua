-- TestVersorAndBladeExpansion.lua

package.cpath = package.cpath .. ';C:\\GeometricAlgebraSystem\\Build\\GAExpressionLua\\Debug\\?.dll'

gaexpapi = require 'GAExpressionLua'

local b1 = gaexpapi.new_vector( 'b1' )
local b2 = gaexpapi.new_vector( 'b2' )
local b3 = gaexpapi.new_vector( 'b3' )
local b4 = gaexpapi.new_vector( 'b4' )
local b5 = gaexpapi.new_vector( 'b5' )

local B2 = b1 ^ b2
local B3 = b1 ^ b2 ^ b3
local B4 = b1 ^ b2 ^ b3 ^ b4
local B5 = b1 ^ b2 ^ b3 ^ b4 ^ b5

function printExpansion( r, target )
	local r_expanded = r:clone()
	r_expanded:expand( target )
	print( tostring(r) .. ' = ' .. tostring( r_expanded ) .. '\n' )
	return r_expanded
end

printExpansion( B2, gaexpapi.SUM_OF_VERSORS )
printExpansion( B3, gaexpapi.SUM_OF_VERSORS )
printExpansion( B4, gaexpapi.SUM_OF_VERSORS )
printExpansion( B5, gaexpapi.SUM_OF_VERSORS )

-- TestVersorAndBladeExpansion.lua