# math_manipulator.py

class Manipulator(object):
    def __init__(self):
        pass
    
    def Manipulate(self, number):
        return False, number

    def CountVectorsAndScalars(self, number):
        from Math.math_number import NumberType
        scalar_count = 0
        vector_count = 0
        for other_number in number.value:
            if other_number._type == NumberType.SCALAR:
                scalar_count += 1
            elif other_number._type == NumberType.VECTOR:
                vector_count += 1
        return scalar_count, vector_count

    def IsProductOfVectors(self, number, product_type=None):
        from Math.math_number import NumberType
        acceptable_products = [product_type] if product_type is not None else [NumberType.INNER_PRODUCT, NumberType.OUTER_PRODUCT, NumberType.GEOMETRIC_PRODUCT]
        if not any([number._type == _type for _type in acceptable_products]):
            return False
        for other_number in number.value:
            if other_number._type != NumberType.VECTOR and other_number._type != NumberType.SCALAR:
                return False
        return True

    def IsBlade(self, number):
        from Math.math_number import NumberType
        return self.IsProductOfVectors(number, NumberType.OUTER_PRODUCT)
    
    def IsVersor(self, number):
        from Math.math_number import NumberType
        return self.IsProductOfVectors(number, NumberType.GEOMETRIC_PRODUCT)
    
    def IsScalar(self, number):
        from Math.math_number import NumberType
        if number._type == NumberType.SCALAR:
            return True
        elif type(number.value) is list:
            for other_number in number.value:
                if not self.IsScalar(other_number):
                    return False
            return True
        return False
    
    def MakeScalarAndVectorLists(self, number):
        from Math.math_number import NumberType
        scalar_list = [other_number for other_number in number.value if other_number._type == NumberType.SCALAR]
        vector_list = [other_number for other_number in number.value if other_number._type == NumberType.VECTOR]
        return scalar_list, vector_list
    
    def CloneList(self, number_list):
        return [number.Clone() for number in number_list]
    
    def ShiftScalars(self, number):
        from Math.math_number import NumberType
        for i in range(len(number.value)):
            if number.value[i]._type != NumberType.SCALAR:
                break
        for j in range(len(number.value)):
            k = len(number.value) - 1 - j
            if number.value[k]._type == NumberType.SCALAR:
                break
        if 0 <= i < k < len(number.value):
            scalar = number.value[k]
            del number.value[k]
            number.value.insert(i, scalar)
            return True
        return False