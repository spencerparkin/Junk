# MathAlgorithm.py

from MathObject import MathObjectType

class MathAlgorithm(object):
    def __init__(self):
        pass
    
    def Manipulate(self, math_object):
        return None
    
    def Apply(self, math_object):
        new_math_object = self.Manipulate(math_object)
        if new_math_object is not None:
            return new_math_object
        elif math_object._type == MathObjectType.MATRIX:
            for i in range(len(math_object.value)):
                for j in range(len(math_object.value[i])):
                    sub_math_object = math_object.value[i][j]
                    new_math_object = self.Apply(sub_math_object)
                    if new_math_object is not None:
                        math_object.value[i][j] = new_math_object
                        return self
        elif type(math_object.value) is list:
            for i, sub_math_object in enumerate(math_object.value):
                new_math_object = self.Apply(sub_math_object)
                if new_math_object is not None:
                    math_object.value[i] = new_math_object
                    return self