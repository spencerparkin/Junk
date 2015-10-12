-- TestAssociation.lua

package.cpath = package.cpath .. ';C:\\GeometricAlgebraSystem\\Build\\GAExpressionLua\\Debug\\?.dll'

gaexpapi = require 'GAExpressionLua'

local a = gaexpapi.new_vector( 'a' )
local b = gaexpapi.new_vector( 'b' )
local c = gaexpapi.new_vector( 'c' )
local d = gaexpapi.new_vector( 'd' )
local e = gaexpapi.new_vector( 'e' )
local f = gaexpapi.new_vector( 'f' )

local r = (a^(b^c))^d

print( tostring(r) .. ' = ' .. tostring( r:associate() ) .. '\n' )

local r = (a*b)*(c*d)*e*(f)

print( tostring(r) .. ' = ' .. tostring( r:associate() ) .. '\n' )

-- TestAssociation.lua