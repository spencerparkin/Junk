# math_number.py

import copy

from Math.math_formatter import Formatter, NumberFormat
from Math.math_parser import Parser
from Math.math_tokenizer import Tokenizer

class NumberType:
    NONE = 'NumberType.NONE'
    SCALAR = 'NumberType.SCALAR'
    VECTOR = 'NumberType.VECTOR'
    SUM = 'NumberType.SUM'
    GEOMETRIC_PRODUCT = 'NumberType.GEOMETRIC_PRODUCT'
    INNER_PRODUCT = 'NumberType.INNER_PRODUCT'
    OUTER_PRODUCT = 'NumberType.OUTER_PRODUCT'
    REVERSE = 'NumberType.REVERSE'
    INVERSE = 'NumberType.INVERSE'

class Number(object):
    def __init__(self, _type=NumberType.NONE, value=None):
        self._type = _type
        self.value = value
    
    def Clone(self):
        return copy.deepcopy(self)
    
    def ToString(self, format=NumberFormat.PARSEABLE):
        formatter = Formatter(format)
        return formatter.Print(self)
    
    def FromString(self, string):
        tokenizer = Tokenizer()
        token_list = tokenizer.TokenizeString(string)
        parser = Parser()
        parser.Parse(token_list, self)
        return self