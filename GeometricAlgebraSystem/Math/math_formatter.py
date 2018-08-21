# math_formatter.py

import re

class NumberFormat:
    NONE = 'NumberFormat.NONE'
    PARSEABLE = 'NumberFormat.PARSEABLE'
    LATEX = 'NumberFormat.LATEX'

class Formatter(object):
    def __init__(self, format):
        self.format = format

    def _MakeParans(self):
        if self.format == NumberFormat.PARSEABLE:
            left_paran = '('
            right_paran = ')'
        elif self.format == NumberFormat.LATEX:
            left_paran = r'\left('
            right_paran = r'\right)'
        return left_paran, right_paran

    def Print(self, number):
        from Math.math_number import NumberType
        result = ''
        if type(number.value) is list:
            if number._type == NumberType.REVERSE or number._type == NumberType.INVERSE:
                func_name = '?'
                if number._type == NumberType.REVERSE:
                    if self.format == NumberFormat.PARSEABLE:
                        func_name = 'reverse'
                    elif self.format == NumberFormat.LATEX:
                        func_name = r'\mbox{reverse}'
                elif number._type == NumberType.INVERSE:
                    if self.format == NumberFormat.PARSEABLE:
                        func_name = 'inverse'
                    elif self.format == NumberFormat.LATEX:
                        func_name = r'\mbox{inverse}'
                left_paran, right_paran = self._MakeParans()
                result = func_name + left_paran + ','.join(self.Print(other_number) for other_number in number.value) + right_paran
            elif len(number.value) == 0:
                if number._type == NumberType.SUM:
                    result = '0'
                elif any([number._type == _type for _type in [NumberType.GEOMETRIC_PRODUCT, NumberType.INNER_PRODUCT, NumberType.OUTER_PRODUCT]]):
                    result = '1'
            else:
                operator = '?'
                if self.format == NumberFormat.PARSEABLE:
                    if number._type == NumberType.SUM:
                        operator = ' + '
                    elif number._type == NumberType.GEOMETRIC_PRODUCT:
                        operator = '*'
                    elif number._type == NumberType.INNER_PRODUCT:
                        operator = '.'
                    elif number._type == NumberType.OUTER_PRODUCT:
                        operator = '^'
                elif self.format == NumberFormat.LATEX:
                    if number._type == NumberType.SUM:
                        operator = '+'
                    elif number._type == NumberType.GEOMETRIC_PRODUCT:
                        operator = '*'
                    elif number._type == NumberType.INNER_PRODUCT:
                        operator = r'\cdot'
                    elif number._type == NumberType.OUTER_PRODUCT:
                        operator = r'\wedge'
                for i, other_number in enumerate(number.value):
                    left_paran = ''
                    right_paran = ''
                    # TODO: We really should be calling the same operator precedence logic used by the parser here.
                    if number._type != NumberType.SUM and other_number._type == NumberType.SUM:
                        left_paran, right_paran = self._MakeParans()
                    result += left_paran + self.Print(other_number) + right_paran
                    if i < len(number.value) - 1:
                        result += operator
                return result
        else:
            result = str(number.value)
            if number._type == NumberType.SCALAR:
                if self.format == NumberFormat.PARSEABLE:
                    result = self._FormatParseableScalars(result)
                while True:
                    match = re.match('(.*?)([a-zA-Z0-9]+)_([a-zA-Z0-9]+)(.*)', result)
                    if match is None:
                        break
                    vectorA = self._FormatVectorName(match.group(2))
                    vectorB = self._FormatVectorName(match.group(3))
                    if self.format == NumberFormat.PARSEABLE:
                        inner_product = '(%s.%s)' % (vectorA, vectorB)
                    elif self.format == NumberFormat.LATEX:
                        inner_product = r'\left(%s\cdot %s\right)' % (vectorA, vectorB)
                    result = match.group(1) + inner_product + match.group(4)
                if self.format == NumberFormat.LATEX:
                    # This may not be enough.
                    result = result.replace('**', '^')
                if self.format == NumberFormat.PARSEABLE:
                    result = '[' + result + ']'
                elif self.format == NumberFormat.LATEX:
                    result = r'\left[' + result + r'\right]'
            elif number._type == NumberType.VECTOR:
                result = self._FormatVectorName(result)
        return result
    
    def _FormatVectorName(self, name):
        if self.format == NumberFormat.PARSEABLE:
            return name
        elif self.format == NumberFormat.LATEX:
            match = re.match('([a-zA-Z]+)([0-9]+)', name)
            if match is not None:
                return r'\vec{%s}_{%s}' % (self._TranslateVectorName(match.group(1)), match.group(2))
            else:
                return r'\vec{%s}' % self._TranslateVectorName(name)
    
    def _TranslateVectorName(self, name):
        if self.format == NumberFormat.LATEX:
            if name == 'no':
                return 'o'
            elif name == 'ni':
                return r'\infty'
        return name
    
    def _FormatParseableScalars(self, result):
        # In the parser we differentiate between vectors and scalars using a '$' sign.
        match = re.match('(.*?)([a-zA-Z][a-zA-Z0-9_]*)(.*)', result)
        if match is None:
            return result
        start = match.group(1)
        middle = match.group(2)
        end = match.group(3)
        if middle.find('$') < 0 and middle.find('_') < 0:
            middle = '$' + middle
        start = self._FormatParseableScalars(start)
        end = self._FormatParseableScalars(end)
        return start + middle + end