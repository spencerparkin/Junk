gac = require 'gacomputelua'

a = gac.newvector('a')
b = gac.newvector('b')
c = gac.newvector('c')
d = gac.newvector('d')
e = gac.newvector('e')

clear()

r = (-1)*(a^b)*(d^c)

print( r:latex() .. '\n' )