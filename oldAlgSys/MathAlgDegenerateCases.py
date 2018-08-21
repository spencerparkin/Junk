# MathAlgDegenerateCases.py

from MathAlgorithm import MathAlgorithm

class MathAlgDegenerateCases(MathAlgorithm):
    def __init__(self):
        super().__init__()

    def Manipulate(self, math_object):
        from MathObject import MathObject
        if math_object._type == MathObject.TYPE_SUM:
            if len(math_object.value) == 0:
                return MathObject(MathObject.TYPE_NUMERIC_SCALAR_CONSTANT, 0.0)
            scalar_math_object, i = math_object.FindValueOfType(MathObject.TYPE_NUMERIC_SCALAR_CONSTANT)
            if scalar_math_object.value == 0.0:
                del math_object.value[i]
                return self
        elif math_object.IsTypeIn([MathObject.TYPE_PRODUCT, MathObject.TYPE_INNER_PRODUCT, MathObject.TYPE_OUTER_PRODUCT]):
            if len(math_object.value) == 0:
                return MathObject(MathObject.TYPE_NUMERIC_SCALAR_CONSTANT, 1.0)
            scalar_math_object, i = math_object.FindValueOfType(MathObject.TYPE_NUMERIC_SCALAR_CONSTANT)
            if scalar_math_object.value == 1.0:
                del math_object.value[i]
                return self
            elif scalar_math_object.value == 0.0:
                return MathObject(MathObject.TYPE_NUMERIC_SCALAR_CONSTANT, 0.0)