# math_sum_manipulator.py

from Math.math_manipulator import Manipulator

class SumManipulator(Manipulator):
    def __init__(self):
        super().__init__()

    def Manipulate(self, number):
        from Math.math_number import Number, NumberType
        if number._type == NumberType.SUM:
            if len(number.value) == 0:
                return True, Number(NumberType.SCALAR, 0.0)
            elif len(number.value) == 1:
                return True, number.value[0]
            else:
                for i, other_number in enumerate(number.value):
                    if other_number._type == NumberType.SCALAR:
                        if other_number.value == 0.0:
                            del number.value[i]
                            return True, number
                if self.ShiftScalars(number):
                    return True, number
                numberA = number.value[0]
                numberB = number.value[1]
                if numberA._type == NumberType.SCALAR and numberB._type == NumberType.SCALAR:
                    del number.value[0]
                    del number.value[0]
                    number.value.insert(0, Number(NumberType.SCALAR, numberA.value + numberB.value))
                    return True, number
            for i in range(len(number.value)):
                termA = number.value[i]
                if self.IsProductOfVectors(termA):
                    for j in range(i+1, len(number.value)):
                        termB = number.value[j]
                        if self.IsProductOfVectors(termB):
                            scalar_countA, vector_countA = self.CountVectorsAndScalars(termA)
                            scalar_countB, vector_countB = self.CountVectorsAndScalars(termB)
                            if vector_countA == vector_countB:
                                if vector_countA == 1 or (vector_countA > 1 and termA._type == termB._type):
                                    scalar_listA, vector_listA = self.MakeScalarAndVectorLists(termA)
                                    scalar_listB, vector_listB = self.MakeScalarAndVectorLists(termB)
                                    # Note that we can do an ordered compare of the vector lists here, because
                                    # we constantly sort outer products using the anti-commutative property.
                                    for k in range(len(vector_listA)):
                                        if vector_listA[k].value != vector_listB[k].value:
                                            break
                                    else:
                                        del number.value[j] # We must delete j before i.
                                        del number.value[i]
                                        sum = Number(NumberType.SUM, [])
                                        sum.value.append(Number(NumberType.GEOMETRIC_PRODUCT, self.CloneList(scalar_listA)))
                                        sum.value.append(Number(NumberType.GEOMETRIC_PRODUCT, self.CloneList(scalar_listB)))
                                        product = Number(termA._type if vector_countA > 1 else NumberType.GEOMETRIC_PRODUCT, [sum])
                                        product.value += self.CloneList(vector_listA)
                                        number.value.append(product)
                                        return True, number
                            elif vector_countA > vector_countB:
                                # The following is for cosmetic and consistency purposes.
                                number.value[i] = termB
                                number.value[j] = termA
                                return True, number
        return False, number