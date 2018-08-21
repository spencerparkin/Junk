# math_gp_manipulator.py

from Math.math_manipulator import Manipulator

class GeometricProductManipulator(Manipulator):
    def __init__(self):
        super().__init__()
    
    def Manipulate(self, number):
        from Math.math_number import NumberType, Number
        if number._type == NumberType.GEOMETRIC_PRODUCT:
            # Since scalars pool near the front of the product, here we need only look
            # for cases we can expand in every adjacency-pair of the product.
            for i in range(len(number.value)-1):
                numberA = number.value[i]
                numberB = number.value[i+1]
                if (numberA._type == NumberType.VECTOR and numberB._type == NumberType.VECTOR) or \
                        (numberA._type == NumberType.VECTOR and self.IsBlade(numberB)) or \
                        (self.IsBlade(numberA) and numberB._type == NumberType.VECTOR):
                    sum = Number(NumberType.SUM, [])
                    sum.value.append(Number(NumberType.INNER_PRODUCT, [numberA.Clone(), numberB.Clone()]))
                    sum.value.append(Number(NumberType.OUTER_PRODUCT, [numberA.Clone(), numberB.Clone()]))
                    del number.value[i+1]
                    number.value[i] = sum
                    return True, number
                elif self.IsBlade(numberA) and self.IsBlade(numberB):
                    if len(numberA.value) > len(numberB.value):
                        blade = numberA
                    else:
                        blade = numberB
                    scalar_list, vector_list = self.MakeScalarAndVectorLists(blade)
                    if len(vector_list) > 1:
                        vector = vector_list.pop(0)
                        sum = Number(NumberType.SUM, [])
                        sum.value.append(Number(NumberType.GEOMETRIC_PRODUCT, [vector.Clone(), Number(NumberType.GEOMETRIC_PRODUCT, vector_list).Clone()]))
                        sum.value.append(Number(NumberType.INNER_PRODUCT, [Number(NumberType.SCALAR, -1.0), vector.Clone(), Number(NumberType.INNER_PRODUCT, vector_list).Clone()]))
                        if blade == numberA:
                            sum.value[0].value.append(numberB.Clone())
                            sum.value[1].value.append(numberB.Clone())
                        elif blade == numberB:
                            sum.value[0].value.insert(0, numberA.Clone())
                            sum.value[1].value.insert(0, numberA.Clone())
                        del number.value[i+1]
                        number.value[i] = sum
                        number.value = scalar_list + number.value
                        return True, number
        return False, number