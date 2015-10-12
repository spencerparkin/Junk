gac = require 'gacomputelua'

a = gac.newvector('a')
b = gac.newvector('b')
c = gac.newvector('c')

r = a - (b + c)

clear()

print( r:latex() .. ' = ' )
r:simplify()
print( r:latex() .. '\n' )