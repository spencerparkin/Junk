# math_prod_manipulator.py

from Math.math_manipulator import Manipulator

# TODO: Several things appear to be wrong with "a^b^c - reverse(a^b^c)".
#       It may not be tokenizing/parsing correctly, and it's not being evaluated correctly.

class ProductManipulator(Manipulator):
    def __init__(self):
        super().__init__()

    def Manipulate(self, number):
        from Math.math_number import Number, NumberType
        type_list = [
            NumberType.GEOMETRIC_PRODUCT,
            NumberType.OUTER_PRODUCT,
            NumberType.INNER_PRODUCT
        ]
        if any([number._type == _type for _type in type_list]):
            if len(number.value) == 0:
                return True, Number(NumberType.SCALAR, 1.0)
            elif len(number.value) == 1:
                return True, number.value[0]
            else:
                for i, other_number in enumerate(number.value):
                    if other_number._type == NumberType.SCALAR:
                        if other_number.value == 1.0:
                            del number.value[i]
                            return True, number
                        elif other_number.value == 0.0:
                            return True, other_number
                    elif any([other_number._type == _type for _type in type_list]):
                        for j in range(len(other_number.value)):
                            if other_number.value[j]._type == NumberType.SCALAR:
                                number.value.insert(0, other_number.value[j])
                                del other_number.value[j]
                                return True, number
                if self.ShiftScalars(number):
                    return True, number
                numberA = number.value[0]
                numberB = number.value[1]
                if numberA._type == NumberType.SCALAR and numberB._type == NumberType.SCALAR:
                    del number.value[0]
                    del number.value[0]
                    number.value.insert(0, Number(NumberType.SCALAR, numberA.value * numberB.value))
                    return True, number
        return False, number