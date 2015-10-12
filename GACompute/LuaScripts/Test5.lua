gac = require 'gacomputelua'

a = gac.newvector('a')
b = gac.newvector('b')
c = gac.newvector('c')
d = gac.newvector('d')
e = gac.newvector('e')

clear()

r = (a^b) .. (c^d^e)

print( r:latex() .. ' = ' )
r:simplify()
print( r:latex() .. '\n' )

r = (c^d^e) .. (a^b)

print( r:latex() .. ' = ' )
r:simplify()
print( r:latex() .. '\n' )