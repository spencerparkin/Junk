-- TestExpansion.lua

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

local r = {}

r[1] = e1..(e1^ni)
r[2] = (e1^ni)..e1
r[3] = (no^ni)..(ni^e1)
r[4] = e1*(e1^ni)
r[5] = (e1^ni)*e2
r[6] = (no^ni)*(e1^e2)
r[7] = (no*ni)^(e1*e2)
r[8] = (e1^e2^e3)*(no^ni - 1)..(no + ni^e1 + e2)

for i = 1, #r do
	print( 'r[' .. tostring(i) .. '] = ' .. tostring(r[i]) )
end

print( '---' )

for i = 1, #r do
	r[i]:expand( gaexpapi.SUM_OF_BLADES )
	print( 'r[' .. tostring(i) .. '] = ' .. tostring(r[i]) )
end

print( '---' )

for i = 1, #r do
	r[i]:expand( gaexpapi.SUM_OF_VERSORS )
	print( 'r[' .. tostring(i) .. '] = ' .. tostring(r[i]) )
end

-- TestExpansion.lua