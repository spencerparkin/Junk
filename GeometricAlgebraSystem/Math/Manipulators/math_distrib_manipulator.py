# math_distrib_manipulator.py

from Math.math_manipulator import Manipulator

class DistributivePropertyManipulator(Manipulator):
    def __init__(self):
        super().__init__()

    def Manipulate(self, number):
        from Math.math_number import Number, NumberType
        type_list = [
            NumberType.GEOMETRIC_PRODUCT,
            NumberType.OUTER_PRODUCT,
            NumberType.INNER_PRODUCT,
            NumberType.REVERSE
        ]
        if any([number._type == _type for _type in type_list]):
            for i, numberA in enumerate(number.value):
                if numberA._type == NumberType.SUM and not self.IsScalar(numberA):
                    sum = Number(NumberType.SUM, [])
                    for numberB in numberA.value:
                        product = Number(number._type, [])
                        start = number.value[:i]
                        middle = [numberB]
                        end = number.value[i+1:]
                        product.value = [number.Clone() for number in start + middle + end]
                        sum.value.append(product)
                    return True, sum
        return False, number