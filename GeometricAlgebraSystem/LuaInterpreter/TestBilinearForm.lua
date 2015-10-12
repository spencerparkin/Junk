-- TestBilinearForm.lua

package.cpath = package.cpath .. ';C:\\GeometricAlgebraSystem\\Build\\GAExpressionLua\\Debug\\?.dll'

gaexpapi = require 'GAExpressionLua'

dofile( 'C:/GeometricAlgebraSystem/LuaInterpreter/Signature.lua' )

signature_table = e3ga_table
gaexpapi.bilinear_form( signature_func )

local e1 = gaexpapi.new_vector( 'e1' )
local e2 = gaexpapi.new_vector( 'e2' )
local e3 = gaexpapi.new_vector( 'e3' )

local x1 = gaexpapi.new_scalar( 'x1' )
local y1 = gaexpapi.new_scalar( 'y1' )
local z1 = gaexpapi.new_scalar( 'z1' )

local x2 = gaexpapi.new_scalar( 'x2' )
local y2 = gaexpapi.new_scalar( 'y2' )
local z2 = gaexpapi.new_scalar( 'z2' )

local v1 = x1*e1 + y1*e2 + z1*e3
local v2 = x2*e1 + y2*e2 + z2*e3

local dot = v1 .. v2

print( 'dot = ' .. tostring( dot ) )

dot:associate()
dot:distribute()
dot:associate()
dot:compress()

print( 'dot = ' .. tostring( dot ) )

-- TestBilinearForm.lua