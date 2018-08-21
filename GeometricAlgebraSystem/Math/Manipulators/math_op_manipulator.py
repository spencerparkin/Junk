# math_op_manipulator.py

from Math.math_manipulator import Manipulator

class OuterProductManipulator(Manipulator):
    def __init__(self):
        super().__init__()

    def Manipulate(self, number):
        from Math.math_number import NumberType, Number
        if self.IsBlade(number):
            # This is the only case of linear dependence we detect.
            for i in range(len(number.value)):
                numberA = number.value[i]
                if numberA._type == NumberType.VECTOR:
                    for j in range(i + 1, len(number.value)):
                        numberB = number.value[j]
                        if numberB._type == NumberType.VECTOR:
                            if numberA.value == numberB.value:
                                return True, Number(NumberType.SCALAR, 0.0)
            adjacent_swap_count = 0
            while True:
                if self._PerformAdjacentVectorSwap(number):
                    adjacent_swap_count += 1
                else:
                    break
            if adjacent_swap_count > 0:
                if adjacent_swap_count % 2 == 1:
                    number.value.insert(0, Number(NumberType.SCALAR, -1.0))
                return True, number
        return False, number
    
    def _PerformAdjacentVectorSwap(self, number):
        from Math.math_number import NumberType
        for i in range(len(number.value)):
            numberA = number.value[i]
            if numberA._type == NumberType.VECTOR:
                for j in range(i+1, len(number.value)):
                    numberB = number.value[j]
                    if numberB._type == NumberType.VECTOR:
                        if numberA.value > numberB.value:
                            number.value[i] = numberB
                            number.value[j] = numberA
                            return True
                        else:
                            # We must break here, because we only want to perform
                            # swaps between adjacent vectors in the product.
                            break
        return False