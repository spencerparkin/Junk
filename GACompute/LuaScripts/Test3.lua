gac = require 'gacomputelua'

a = gac.newvector('a')
b = gac.newvector('b')
c = gac.newvector('c')
d = gac.newvector('d')

clear()

r = a*b*c*d

print( r:latex() .. ' = ' )
r:simplify()
print( r:latex() .. '\n' )

r = a^b^c^d

print( r:latex() .. ' = ' )
r:simplify()
print( r:latex() .. '\n' )
