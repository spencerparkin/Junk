# math_assoc_manipulator.py

from Math.math_manipulator import Manipulator

class AssociativePropertyManipulator(Manipulator):
    def __init__(self):
        super().__init__()

    def Manipulate(self, number):
        from Math.math_number import NumberType
        # Note that the inner product is not generally associative.
        type_list = [NumberType.SUM, NumberType.GEOMETRIC_PRODUCT, NumberType.OUTER_PRODUCT]
        if any([number._type == _type for _type in type_list]):
            for i, other_number in enumerate(number.value):
                if other_number._type == number._type:
                    start = number.value[:i]
                    middle = other_number.value
                    end = number.value[i+1:]
                    number.value = start + middle + end
                    return True, number
        return False, number