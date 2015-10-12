gac = require 'gacomputelua'

e1 = gac.newvector( "e1" )
e2 = gac.newvector( "e2" )
a = gac.newvector( "a" )

function println( r )
	print( r:latex() .. '\n' )
end

bilinearForm = function( nameA, nameB )
	if ( nameA ~= "e1" and nameA ~= "e2" ) or ( nameB ~= "e1" and nameB ~= "e2" ) then
		return nil
	end
	if nameA == "e1" and nameB == "e1" then
		return 1
	elseif nameA == "e2" and nameB == "e2" then
		return 1
	end
	return 0
end

gac.bilinear_form( bilinearForm )

clear()

r = (2*e1 + 3*e2)..(e1 - 5*e2 + 2*a)
println(r)

r:simplify()
println(r)
