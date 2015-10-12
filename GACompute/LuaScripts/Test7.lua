gac = require 'gacomputelua'

alpha = gac.newscalar( "\\alpha" )
beta = gac.newscalar( "\\beta" )

clear()

r = alpha ^ beta

print( r:latex() .. ' = ' )
r:simplify()
print( r:latex() .. '\n' )

r = alpha .. beta

print( r:latex() .. ' = ' )
r:simplify()
print( r:latex() .. '\n' )
