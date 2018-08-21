# MathAlgElevate.py

from MathAlgorithm import MathAlgorithm

class MathAlgElevate(MathAlgorithm):
    def __init__(self):
        super().__init__()

    def Manipulate(self, math_object):
        from MathObject import MathObject
        if math_object.IsTypeIn([MathObject.TYPE_PRODUCT, MathObject.TYPE_INNER_PRODUCT, MathObject.TYPE_OUTER_PRODUCT]):
            for sub_math_object in math_object.value:
                if sub_math_object.IsTypeIn([MathObject.TYPE_PRODUCT, MathObject.TYPE_INNER_PRODUCT, MathObject.TYPE_OUTER_PRODUCT]):
                    scalar_math_object, i = sub_math_object.FindValueOfType([MathObject.TYPE_NUMERIC_SCALAR_CONSTANT, MathObject.TYPE_SYMBOLIC_SCALAR_CONSTANT])
                    if scalar_math_object is not None:
                        del sub_math_object.value[i]
                        math_object.value.append(scalar_math_object)
                        return self