-- TestInnerProductIdentities.lua

package.cpath = package.cpath .. ';C:\\GeometricAlgebraSystem\\Build\\GAExpressionLua\\Debug\\?.dll'

gaexpapi = require 'GAExpressionLua'

local v1 = gaexpapi.new_vector( 'v1' )
local v2 = gaexpapi.new_vector( 'v2' )
local v3 = gaexpapi.new_vector( 'v3' )
local v4 = gaexpapi.new_vector( 'v4' )
local v5 = gaexpapi.new_vector( 'v5' )

local a = gaexpapi.new_vector( 'a' )

local r1 = a .. v1
local r2 = a .. ( v1 * v2 )
local r3 = a .. ( v1 * v2 * v3 )
local r4 = a .. ( v1 * v2 * v3 * v4 )
local r5 = a .. ( v1 * v2 * v3 * v4 * v5 )

local s1 = a .. v1
local s2 = a .. ( v1 ^ v2 )
local s3 = a .. ( v1 ^ v2 ^ v3 )
local s4 = a .. ( v1 ^ v2 ^ v3 ^ v4 )
local s5 = a .. ( v1 ^ v2 ^ v3 ^ v4 ^ v5 )

function printExpansion( r, target )
	local r_expanded = r:clone()
	r_expanded:expand( target )
	print( tostring(r) .. ' = ' .. tostring( r_expanded ) .. '\n' )
	return r_expanded
end

printExpansion( r1, gaexpapi.SUM_OF_VERSORS )
printExpansion( r2, gaexpapi.SUM_OF_VERSORS )
printExpansion( r3, gaexpapi.SUM_OF_VERSORS )
printExpansion( r4, gaexpapi.SUM_OF_VERSORS )
printExpansion( r5, gaexpapi.SUM_OF_VERSORS )

--printExpansion( s1, gaexpapi.SUM_OF_BLADES )
--printExpansion( s2, gaexpapi.SUM_OF_BLADES )
--printExpansion( s3, gaexpapi.SUM_OF_BLADES )
--printExpansion( s4, gaexpapi.SUM_OF_BLADES )
--printExpansion( s5, gaexpapi.SUM_OF_BLADES )

-- STPTODO: If we can find an identity for a .. ( v1 * ... * vN ), then we can improve the expander algorithm with sum of versors target.

-- TestInnerProductIdentities.lua