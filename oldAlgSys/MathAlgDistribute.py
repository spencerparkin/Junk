# MathAlgDistribute.py

from MathAlgorithm import MathAlgorithm

class MathAlgDistribute(MathAlgorithm):
    def __init__(self):
        super().__init__()
    
    def Manipulate(self, math_object):
        from MathObject import MathObject
        type_list = [
            MathObject.TYPE_PRODUCT,
            MathObject.TYPE_INNER_PRODUCT,
            MathObject.TYPE_OUTER_PRODUCT,
            MathObject.TYPE_REVERSE
        ]
        if math_object.IsTypeIn(type_list):
            for i, sub_math_object in enumerate(math_object.value):
                if sub_math_object._type == MathObject.TYPE_SUM:
                    # The idea here is to prevent a fight between collection and distribution.
                    # These are inverses of one another in the case that things don't combine.
                    if not math_object.IsScalar() and sub_math_object.IsScalar():
                        continue
                    sum_math_object = MathObject(MathObject.TYPE_SUM, [])
                    for term_math_object in sub_math_object.value:
                        product_math_object = MathObject(MathObject.TYPE_PRODUCT, [])
                        for j in range(0, i):
                            product_math_object.value.append(math_object.value[j].Copy())
                        product_math_object.value.append(term_math_object.Copy())
                        for j in range(i + 1, len(math_object.value)):
                            product_math_object.value.append(math_object.value[j].Copy())
                        sum_math_object.value.append(product_math_object)
                    return sum_math_object