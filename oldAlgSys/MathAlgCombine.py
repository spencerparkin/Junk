# MathAlgCombine.py

from MathAlgorithm import MathAlgorithm

class MathAlgCombine(MathAlgorithm):
    def __init__(self):
        super().__init__()

    def Manipulate(self, math_object):
        # Note that our ability to identify like terms here depends upon the math algorithm that sorts various products.
        from MathObject import MathObject
        if math_object._type == MathObject.TYPE_SUM:
            for pair in math_object.GenerateValuePairs():
                if pair[0]._type == MathObject.TYPE_NUMERIC_SCALAR_CONSTANT and pair[1]._type == MathObject.TYPE_NUMERIC_SCALAR_CONSTANT:
                    math_object.DeleteValues([pair[2], pair[3]])
                    sum = pair[0].value + pair[1].value
                    self.value.append(MathObject.TYPE_NUMERIC_SCALAR_CONSTANT, sum)
                    return self
            # I think we need to generate a list of blade/scalar pairs for collection.
            # We could also generate a list of symbolic/numeric pairs for collection.