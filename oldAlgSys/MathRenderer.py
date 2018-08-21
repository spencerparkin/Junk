# MathRenderer.py

import re

from MathObject import MathObject

class MathRenderer(object):
    def __init__(self):
        pass
    
    def Render(self, math_object):
        pass

class MathLatexRenderer(MathRenderer):
    def __init__(self):
        pass
    
    def Render(self, math_object):
        pass
    
class MathPythonCodeRenderer(MathRenderer):
    def __init__(self):
        super().__init__()
    
    def Render(self, math_object):
        pass

'''
    @staticmethod
    def _FormatLatexName(name, wrapper=None):
        match = re.match(r'([a-zA-Z0-9])_([a-zA-Z0-9])', name)
        if match is not None:
            if wrapper is not None:
                latex_name = r'\%s{%s}_{%s}' % (wrapper, match.group(1), match.group(2))
            else:
                latex_name = '%s_{%s}' % (match.group(1), match.group(2))
        else:
            if wrapper is not None:
                latex_name = r'\%s{%s}' % (wrapper, name)
            else:
                latex_name = name
        return latex_name

    @staticmethod
    def _ParanWrap(rendered_text, format):
        if format == MathObject.RENDER_FORMAT_NORMAL:
            return '(' + rendered_text + ')'
        elif format == MathObject.RENDER_FORMAT_LATEX:
            return r'\left(' + rendered_text + r'\right'

    def Render(self, format=RENDER_FORMAT_NORMAL):
        if self._type == MathObjectType.NUMERIC_SCALAR_CONSTANT:
            if format == self.RENDER_FORMAT_NORMAL:
                return '%f' % self.value
            elif format == self.RENDER_FORMAT_LATEX:
                return '%1.2f' % self.value
        elif self._type == MathObjectType.SYMBOLIC_SCALAR_CONTANT:
            if format == self.RENDER_FORMAT_NORMAL:
                return '$' + self.value
            elif format == self.RENDER_FORMAT_LATEX:
                return self.FormatLatexName(self.value)
        elif self._type == MathObjectType.SYMBOLIC_VECTOR_CONSTANT:
            if format == self.RENDER_FORMAT_NORMAL:
                return self.value
            elif format == self.RENDER_FORMAT_LATEX:
                return self.FormatLatexName(self.value, 'vec')
        elif self._type == MathObjectType.MATRIX:
            if format == self.RENDER_FORMAT_NORMAL:
                pass
            elif format == self.RENDER_FORMAT_LATEX:
                latex_matrix = r'\left[\begin{array}{%s}' % ('c' * len(self.value[0]))
                for i in range(len(self.value)):
                    for j in range(len(self.value[i])):
                        sub_render = self.value[i][j].Render(format)
                        latex_matrix += sub_render + ' & ' if j + 1 < len(self.value[i]) else r' \\ '
                latex_matrix += r'\end{array}\right]'
                return latex_matrix
        elif any([self._type == _type for _type in MathObjectType.InfixBinaryOperatorList()]):
            sub_render_list = []
            for sub_math_object in self.value:
                sub_render = sub_math_object.Render(format)
                if MathObjectType.ParanWrap(sub_math_object._type, self._type):
                    sub_render = self.ParanWrap(sub_render)
                sub_render_list.append(sub_render)
            symbol = MathObjectType.Symbol(self._type, format)
            return symbol.join(sub_render_list)
        elif any([self._type == _type for _type in MathObjectType.FunctionList()]):
            sub_render_list = [sub_math_object.Render(format) for sub_math_object in self.value]
            symbol = MathObjectType.Symbol(self._type, format)
            return symbol + self.ParanWrap(','.join(sub_render_list))

    @staticmethod
    def Symbol(_type, format):
        if _type == MathObjectType.ADD:
            return ' + '
        elif _type == MathObjectType.MULTIPLY:
            return '*'
        elif _type == MathObjectType.INNER_MULTIPLY:
            if format == MathObject.RENDER_FORMAT_NORMAL:
                return '.'
            elif format == MathObject.RENDER_FORMAT_LATEX:
                return r'\cdot'
        elif _type == MathObjectType.OUTER_MULTIPLY:
            if format == MathObject.RENDER_FORMAT_NORMAL:
                return '^'
            elif format == MathObject.RENDER_FORMAT_LATEX:
                return r'\wedge'
        elif _type == MathObjectType.NEGATE:
            return '-'
        elif _type == MathObjectType.REVERSE:
            if format == MathObject.RENDER_FORMAT_NORMAL:
                return '~'
            elif format == MathObject.RENDER_FORMAT_LATEX:
                return r'\sim'
        elif any([_type == _type_func for _type_func in MathObjectType.FunctionList()]):
            func_name = None
            if _type == MathObjectType.SINE:
                func_name = 'sin'
            elif _type == MathObjectType.COSINE:
                func_name = 'cos'
            if format == MathObject.RENDER_FORMAT_LATEX:
                func_name = r'\mbox{%s}' % func_name
            return func_name

    @staticmethod
    def InfixBinaryOperatorList():
        return [
            MathObjectType.ADD,
            MathObjectType.MULTIPLY,
            MathObjectType.INNER_MULTIPLY,
            MathObjectType.OUTER_MULTIPLY
        ]

    @staticmethod
    def LeftUnaryOperatorList():
        return [
            MathObjectType.NEGATE
        ]

    @staticmethod
    def RightUnaryOperatorList():
        return [
            MathObjectType.REVERSE,
            MathObjectType.FACTORIAL
        ]

    @staticmethod
    def FunctionList():
        return [
            MathObjectType.SQUARE_ROOT,
            MathObjectType.SINE,
            MathObjectType.COSINE,
            MathObjectType.TANGENT,
            MathObjectType.EXPONENT,
            MathObjectType.LOG
        ]

    @staticmethod
    def ParanWrap(type_inner, type_outer):
        pass
'''