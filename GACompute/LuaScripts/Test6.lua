gac = require 'gacomputelua'

function print_line( r )
	print( r:latex() .. '\n' )
end

a = gac.newvector('a')
b = gac.newvector('b')
c = gac.newvector('c')
d = gac.newvector('d')

clear()

r = (a^b)*(c^d)

clear()
print_line(r)
r:simplify()
print_line(r)
