# MathObject.py

import copy
import datetime

from MathException import MathException

class MathObject(object):
    TYPE_NUMERIC_SCALAR_CONSTANT = 'TYPE_NUMERIC_SCALAR_CONSTANT'
    TYPE_SYMBOLIC_SCALAR_CONSTANT = 'TYPE_SYMBOLIC_SCALAR_CONSTANT'
    TYPE_SYMBOLIC_VECTOR_CONSTANT = 'TYPE_SYMBOLIC_VECTOR_CONSTANT'
    TYPE_SUM = 'TYPE_SUM'
    TYPE_PRODUCT = 'TYPE_PRODUCT'
    TYPE_NEGATE = 'TYPE_NEGATE'
    TYPE_INVERT = 'TYPE_INVERT'
    TYPE_REVERSE = 'TYPE_REVERSE'
    TYPE_OUTER_PRODUCT = 'TYPE_OUTER_PRODUCT'
    TYPE_INNER_PRODUCT = 'TYPE_INNER_PRODUCT'
    TYPE_MATRIX = 'TYPE_MATRIX'
    TYPE_SQUARE_ROOT = 'TYPE_SQUARE_ROOT'
    TYPE_SINE = 'TYPE_SINE'
    TYPE_COSINE = 'TYPE_COSINE'
    TYPE_TANGENT = 'TYPE_TANGENT'
    TYPE_EXPONENT = 'TYPE_EXPONENT'
    TYPE_LOG = 'TYPE_LOG'
    TYPE_FACTORIAL = 'TYPE_FACTORIAL'
    TYPE_GRADE_PART = 'TYPE_GRADE_PART'
    
    def __init__(self, _type, value):
        self._type = _type
        self.value = value
    
    def IsTypeIn(self, type_list):
        return any([self._type == _type for _type in type_list])

    def GenerateValuePairs(self):
        if type(self.value) is list and self._type != self.TYPE_MATRIX:
            for i in range(len(self.value)):
                for j in range(i + 1, len(self.value)):
                    yield self.value[i], self.value[j], i, j

    def DeleteValues(self, offset_list):
        if type(self.value) is list and self._type != self.TYPE_MATRIX:
            offset_list.sort(reversed=True)
            for i in offset_list:
                del self.value[i]

    def FindValueOfType(self, type_list, recursive=False):
        if type(self.value) is list and self._type != self.TYPE_MATRIX:
            for i, math_object in enumerate(self.value):
                if math_object.IsTypeIn(type_list):
                    return math_object, i
            if recursive:
                for math_object in self.value:
                    sub_math_object, i = math_object.FindValueOfType(type_list, True)
                    if sub_math_object is not None:
                        return sub_math_object, i
        return None, None
    
    def FilterValuesOfType(self, type_list):
        if type(self.value) is list and self._type != self.TYPE_MATRIX:
            pass_list = []
            fail_list = []
            for sub_math_object in self.value:
                if sub_math_object.IsTypeIn(type_list):
                    pass_list.append(sub_math_object)
                else:
                    fail_list.append(sub_math_object)
            return pass_list, fail_list
        return None, None
    
    def GatherAllTypes(self, type_list):
        type_list.append(self._type)
        if self._type == self.TYPE_MATRIX:
            pass
        if type(self.value) is list:
            for math_object in self.value:
                math_object.GatherAllTypes(type_list)
    
    def IsScalar(self):
        # In most cases, returning false here only means that we do not know if the math object tree
        # rooted at this node ultimately evaluates to a scalar value.  If, on the other hand, we
        # return true, then we have positively identified a scalar value.
        if self.IsTypeIn([self.TYPE_SYMBOLIC_SCALAR_CONSTANT, self.TYPE_NUMERIC_SCALAR_CONSTANT]):
            return True
        elif self._type == self.TYPE_MATRIX:
            return False
        elif self._type == self.TYPE_INNER_PRODUCT:
            pass_list, fail_list = self.FilterValuesOfType([self.TYPE_SYMBOLIC_VECTOR_CONSTANT])
            if len(pass_list) != 2 and len(pass_list) != 0:
                return False
            for sub_math_object in fail_list:
                if not sub_math_object.IsScalar():
                    return False
            return True
        elif type(self.value) is list:
            for sub_math_object in self.value:
                if not sub_math_object.IsScalar():
                    return False
            return True
        return False
    
    @staticmethod
    def Cast(other):
        if type(other) is str:
            if other[0] == '$':
                return MathObject(MathObject.TYPE_SYMBOLIC_SCALAR_CONSTANT, other[1:])
            else:
                return MathObject(MathObject.TYPE_SYMBOLIC_VECTOR_CONSTANT, other)
        elif type(other) is float:
            return MathObject(MathObject.TYPE_NUMERIC_SCALAR_CONSTANT, other)
        elif type(other) is int:
            return MathObject(MathObject.TYPE_NUMERIC_SCALAR_CONSTANT, float(other))
        elif type(other) is list:
            pass # TODO: Make matrix.
        else:
            raise MathException('Failed to cast "%s" as a MathObject instance.' % str(other))
    
    def Copy(self):
        return copy.deepcopy(self)
    
    def CopyValue(self):
        return copy.deepcopy(self.value)
    
    def Simplify(self):
        # TODO: Import applicable math alg derivatives here and put instances in the list.
        math_algs_list = []
        return self._Process(math_algs_list)
    
    def _Process(self, math_algs_list, timeout_seconds=None):
        # Note that the termination of our algorithm here depends on
        # there being no back-and-forth fighting between two or more
        # math algorithms in the given list.
        start_time = datetime.datetime.now()
        result = self.Copy()
        while True:
            for math_alg in math_algs_list:
                new_result = math_alg.Apply(result)
                if new_result is not None:
                    result = new_result
                    break
            else:
                break
            current_time = datetime.datetime.now()
            delta_time = current_time - start_time
            if timeout_seconds is not None and delta_time.total_seconds() >= timeout_seconds:
                raise MathException('Timed-out while trying to process math object tree.')
        return result
    
    def ApplyAlgorithm(self, math_alg_name, timeout_seconds=None):
        # TODO: Import all math alg derivatives here.
        from MathAlgorithm import MathAlgorithm
        for math_alg_class in MathAlgorithm.__subclasses__:
            if math_alg_class.__name__ == math_alg_name:
                return self._Process([math_alg_class()], timeout_seconds)
        raise MathException('No math algorithms found by the name "%s".' % math_alg_name)
    
    def __str__(self):
        from MathRenderer import MathRenderer
        renderer = MathRenderer()
        return renderer.Render(self)
    
    def __add__(self, other):
        if isinstance(other, MathObject):
            return MathObject(MathObject.TYPE_ADD, [self.Copy(), other.Copy()])
        else:
            return MathObject(MathObject.TYPE_ADD, [self.Copy(), self.Cast(other)])
    
    def __radd__(self, other):
        return MathObject(MathObject.TYPE_ADD, [self.Cast(other), self.Copy()])
    
    def __sub__(self, other):
        if isinstance(other, MathObject):
            return MathObject(MathObject.TYPE_ADD, [self.Copy(), MathObject(MathObject.TYPE_NEGATE, other.Copy())])
        else:
            return MathObject(MathObject.TYPE_ADD, [self.Copy(), MathObject(MathObject.TYPE_NEGATE, self.Cast(other))])
    
    def __rsub__(self, other):
        return MathObject(MathObject.TYPE_ADD, [MathObject(MathObject.TYPE_NEGATE, self.Cast(other)), self.Copy()])
    
    def __mul__(self, other):
        if isinstance(other, MathObject):
            return MathObject(MathObject.TYPE_MULTIPLY, [self.Copy(), other.Copy()])
        else:
            return MathObject(MathObject.TYPE_MULTIPLY, [self.Copy(), self.Cast(other)])
    
    def __rmul__(self, other):
        return MathObject(MathObject.TYPE_MULTIPLY, [self.Cast(other), self.Copy()])
    
    def __truediv__(self, other):
        if isinstance(other, MathObject):
            return MathObject(MathObject.TYPE_MULTIPLY, [self.Copy(), MathObject(MathObject.TYPE_INVERT, other.Copy())])
        else:
            return MathObject(MathObject.TYPE_MULTIPLY, [self.Copy(), MathObject(MathObject.TYPE_INVERT, self.Cast(other))])
    
    def __rtruediv__(self, other):
        return MathObject(MathObject.TYPE_MULTIPLY, [MathObject(MathObject.TYPE_INVERT, self.Cast(other)), self.Copy()])
    
    def __neg__(self, other):
        return MathObject(MathObject.TYPE_NEGATE, self.Copy())
    
    def __invert__(self):
        return MathObject(MathObject.TYPE_INVERT, self.Copy())
    
    def __or__(self, other):
        if isinstance(other, MathObject):
            return MathObject(MathObject.TYPE_INNER_MULTIPLY, [self.Copy(), other.Copy()])
        else:
            return MathObject(MathObject.TYPE_INNER_MULTIPLY, [self.Copy(), self.Cast(other)])
    
    def __ror__(self, other):
        return MathObject(MathObject.TYPE_INNER_MULTIPLY, [self.Cast(other), self.Copy()])
    
    def __xor__(self, other):
        if isinstance(other, MathObject):
            return MathObject(MathObject.TYPE_OUTER_MULTIPLY, [self.Copy(), other.Copy()])
        else:
            return MathObject(MathObject.TYPE_OUTER_MULTIPLY, [self.Copy(), self.Cast(other)])
    
    def __rxor__(self, other):
        return MathObject(MathObject.TYPE_OUTER_MULTIPLY, [self.Cast(other), self.Copy()])