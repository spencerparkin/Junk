# math_ip_manipulator.py

import re
import sympy

from Math.math_manipulator import Manipulator

class InnerProductManipulator(Manipulator):
    def __init__(self):
        super().__init__()
        self.euclid_re = re.compile('e([a-zA-Z0-9]+)')

    def Manipulate(self, number):
        from Math.math_number import NumberType, Number
        if number._type == NumberType.INNER_PRODUCT:
            # Since scalars pool near the front of the product, and since this
            # product is not generally associative, we need only look at the
            # last two numbers in the product.
            if len(number.value) >= 2:
                numberA = number.value[-2]
                numberB = number.value[-1]
                if numberA._type == NumberType.VECTOR and numberB._type == NumberType.VECTOR:
                    scalar = self._BilinearForm(numberA, numberB)
                    if scalar is None:
                        scalar = self._SymbolicallyMultiplyVectors(numberA, numberB)
                    del number.value[-1]
                    number.value[-1] = scalar
                    return True, number
                elif numberA._type == NumberType.VECTOR and self.IsBlade(numberB):
                    geometric_product = self._ExpandVectorWithBlade(numberA, numberB, False)
                    del number.value[-1]
                    number.value[-1] = geometric_product
                    return True, number
                elif self.IsBlade(numberA) and numberB._type == NumberType.VECTOR:
                    geometric_product = self._ExpandVectorWithBlade(numberB, numberA, True)
                    del number.value[-1]
                    number.value[-1] = geometric_product
                    return True, number
                elif self.IsBlade(numberA) and self.IsBlade(numberB):
                    scalar_listA, vector_listA = self.MakeScalarAndVectorLists(numberA)
                    scalar_listB, vector_listB = self.MakeScalarAndVectorLists(numberB)
                    if len(vector_listA) > 1 and len(vector_listB) > 1:
                        if len(vector_listA) < len(vector_listB):
                            vector = vector_listA.pop(-1)
                            inner_product = Number(NumberType.INNER_PRODUCT, [
                                Number(NumberType.OUTER_PRODUCT, self.CloneList(vector_listA)),
                                Number(NumberType.INNER_PRODUCT, [vector.Clone(), numberB.Clone()])
                            ])
                        else:
                            vector = vector_listB.pop(0)
                            inner_product = Number(NumberType.INNER_PRODUCT, [
                                Number(NumberType.INNER_PRODUCT, [numberA.Clone(), vector.Clone()]),
                                Number(NumberType.OUTER_PRODUCT, self.CloneList(vector_listB))
                            ])
                        inner_product.value = self.CloneList(scalar_listA) + self.CloneList(scalar_listB) + inner_product.value
                        del number.value[-1]
                        number.value[-1] = inner_product
                        return True, number
        return False, number
    
    def _ExpandVectorWithBlade(self, vector, blade, flip):
        k = 0 if flip else 1
        from Math.math_number import NumberType, Number
        scalar_list, vector_list = self.MakeScalarAndVectorLists(blade)
        sum = Number(NumberType.SUM, [])
        for i in range(len(vector_list)):
            outer_product = Number(NumberType.OUTER_PRODUCT, [vector_list[j].Clone() for j in range(len(vector_list)) if j != i])
            inner_product = Number(NumberType.INNER_PRODUCT, [vector.Clone(), vector_list[i].Clone()])
            geometric_product = Number(NumberType.GEOMETRIC_PRODUCT, [inner_product, outer_product])
            if i % 2 == k:
                geometric_product.value.insert(0, Number(NumberType.SCALAR, -1.0))
            sum.value.append(geometric_product)
        geometric_product = Number(NumberType.GEOMETRIC_PRODUCT, [scalar.Clone() for scalar in scalar_list])
        geometric_product.value.append(sum)
        return geometric_product
    
    def _BilinearForm(self, vectorA, vectorB):
        from Math.math_number import NumberType, Number
        matchA = re.match(self.euclid_re, vectorA.value)
        matchB = re.match(self.euclid_re, vectorB.value)
        if matchA is not None and matchB is not None:
            euclidA = matchA.group(1)
            euclidB = matchB.group(1)
            if euclidA.isdigit() and euclidB.isdigit():
                if euclidA == euclidB:
                    return Number(NumberType.SCALAR, 1.0)
                else:
                    return Number(NumberType.SCALAR, 0.0)
        elif matchA is not None and (vectorB.value == 'no' or vectorB.value == 'ni'):
            return Number(NumberType.SCALAR, 0.0)
        elif (vectorA.value == 'no' or vectorA.value == 'ni') and matchB is not None:
            return Number(NumberType.SCALAR, 0.0)
        elif vectorA.value == 'no' and vectorB.value == 'ni':
            return Number(NumberType.SCALAR, -1.0)
        elif vectorA.value == 'ni' and vectorB.value == 'no':
            return Number(NumberType.SCALAR, -1.0)
        elif vectorA.value == 'no' and vectorB.value == 'no':
            return Number(NumberType.SCALAR, 0.0)
        elif vectorA.value == 'ni' and vectorB.value == 'ni':
            return Number(NumberType.SCALAR, 0.0)
        return None
    
    def _SymbolicallyMultiplyVectors(self, vectorA, vectorB):
        # We're using the underscore here as another way of writing the inner product.
        from Math.math_number import NumberType, Number
        if vectorA.value < vectorB.value:
            name = vectorA.value + '_' + vectorB.value
        else:
            name = vectorB.value + '_' + vectorA.value
        value = sympy.symbols(name)
        return Number(NumberType.SCALAR, value)