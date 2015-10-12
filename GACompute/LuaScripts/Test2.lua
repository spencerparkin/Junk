gac = require 'gacomputelua'

a = gac.newvector('a')
b = gac.newvector('b')
c = gac.newvector('c')
d = gac.newvector('d')
e = gac.newvector('e')

alpha = gac.newscalar('\\alpha')
beta = gac.newscalar('\\beta')
gamma = gac.newscalar('\\gamma')

r = 4^a^(b*8*(c^9 .. d*alpha)*e^beta..gamma..gamma)

clear()

print( r:latex() .. '\n' )
r:simplify()
print( r:latex() .. '\n' )