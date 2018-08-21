# MathAlgAssociate.py

from MathAlgorithm import MathAlgorithm

class MathAlgAssociate(MathAlgorithm):
    def __init__(self):
        super().__init__()

    def Manipulate(self, math_object):
        from MathObject import MathObject
        if math_object.IsTypeIn([MathObject.TYPE_SUM, MathObject.TYPE_PRODUCT, MathObject.TYPE_OUTER_PRODUCT]):
            for i, sub_math_object_a in enumerate(math_object.value):
                if sub_math_object_a._type == math_object._type:
                    del math_object.value[i]
                    for j, sub_math_object_b in enumerate(sub_math_object_a.value):
                        math_object.insert(i + j, sub_math_object_b)
                    return self