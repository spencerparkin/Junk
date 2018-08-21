# math_rev_manipulator.py

from Math.math_exception import MathException
from Math.math_manipulator import Manipulator

class ReverseManipulator(Manipulator):
    def __init__(self):
        super().__init__()

    def Manipulate(self, number):
        from Math.math_number import NumberType, Number
        if number._type == NumberType.REVERSE:
            if len(number.value) != 1:
                raise MathException('Reverse function expects exactly one argument.')
            arg_number = number.value[0]
            if self.IsBlade(arg_number) or self.IsVersor(arg_number):
                rev_number = Number(arg_number._type, [])
                for other_number in reversed(arg_number.value):
                    rev_number.value.append(other_number)
                return True, rev_number
            elif arg_number._type == NumberType.GEOMETRIC_PRODUCT:
                rev_number = Number(NumberType.GEOMETRIC_PRODUCT, [])
                for other_number in reversed(arg_number.value):
                    rev_number.value.append(Number(NumberType.REVERSE, [other_number]))
                return True, rev_number
            elif arg_number._type == NumberType.VECTOR or arg_number._type == NumberType.SCALAR:
                return True, arg_number
        return False, number