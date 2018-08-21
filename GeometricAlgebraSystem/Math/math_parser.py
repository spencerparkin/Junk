# math_parser.py

import sympy

from Math.math_exception import MathException
from Math.math_tokenizer import TokenType

class Parser(object):
    def __init__(self):
        pass

    def _BumpLevel(self, token, level):
        if token._type == TokenType.LEFT_PARAN:
            return level + 1
        elif token._type == TokenType.RIGHT_PARAN:
            return level - 1
        return level

    def _FindMatchingParan(self, token_list, i):
        level = 0
        while True:
            if i >= len(token_list):
                raise MathException('Failed to find closing paran for open paran.')
            token = token_list[i]
            level = self._BumpLevel(token, level)
            if level < 0:
                raise MathException('Closing paran has no opening paran.')
            if level == 0:
                break
            i += 1
        return i

    def _MakeDelineatedTokenLists(self, token_list):
        delineated_token_lists = []
        sub_token_list = []
        level = 0
        for i in range(len(token_list)):
            token = token_list[i]
            level = self._BumpLevel(token, level)
            if level == 1 and token._type == TokenType.DELIMITER:
                delineated_token_lists.append(sub_token_list)
                sub_token_list = []
            else:
                sub_token_list.append(token)
        delineated_token_lists.append(sub_token_list)
        return delineated_token_lists

    def _FindBinaryOperator(self, token_list, assoc='left-to-right'):
        j = -1
        level = 0
        for i in range(len(token_list)):
            if assoc == 'right-to-left':
                i = len(token_list) - 1 - i
            token = token_list[i]
            level = self._BumpLevel(token, level)
            if level == 0 and token._type == TokenType.OPERATOR:
                # Here we're trying to distinguish between unary and binary operators in just a few cases.
                # In all other cases, the user may need to use parenthesis to help out the parser.
                if 0 < i < len(token_list) - 1 and token_list[i-1]._type != TokenType.OPERATOR:
                    if j == -1 or self._OperatorPrecedence(token.string) < self._OperatorPrecedence(token_list[j].string):
                        j = i
        return j

    def _OperatorAssociativity(self, operator):
        if operator == '-':
            return 'right-to-left'
        return 'left-to-right'

    def _OperatorPrecedence(self, operator):
        if operator == '+' or operator == '-':
            return 0
        elif operator == '*' or operator == '/':
            return 1
        elif operator == '.':
            return 2
        elif operator == '^':
            return 3
        raise MathException('Unknown operator (%s) encountered.' % operator)

    def Parse(self, token_list, number=None):
        from Math.math_number import Number, NumberType
        if number is None:
            number = Number()
        number._type = NumberType.NONE
        while token_list[0]._type == TokenType.LEFT_PARAN and token_list[-1]._type == TokenType.RIGHT_PARAN:
            i = self._FindMatchingParan(token_list, 0)
            if i == len(token_list) - 1:
                token_list = token_list[1:-1]
            else:
                break
        if len(token_list) == 0:
            raise MathException('Encountered empty token list.')
        elif len(token_list) == 1:
            token = token_list[0]
            if token._type == TokenType.IDENTIFIER:
                if token.string[0] == '$':
                    number.value = sympy.symbols(token.string[1:])
                    number._type = NumberType.SCALAR
                else:
                    number.value = token.string
                    number._type = NumberType.VECTOR
            elif token._type == TokenType.LITERAL:
                if token.string[0].isdigit():
                    number.value = float(token.string)
                    number._type = NumberType.SCALAR
        elif len(token_list) > 1:
            if token_list[0]._type == TokenType.IDENTIFIER and token_list[1]._type == TokenType.LEFT_PARAN and token_list[-1]._type == TokenType.RIGHT_PARAN:
                i = self._FindMatchingParan(token_list, 1)
                if i == len(token_list) - 1:
                    delineated_token_lists = self._MakeDelineatedTokenLists(token_list[2:-1])
                    number.value = [self.Parse(token_list) for token_list in delineated_token_lists]
                    token = token_list[0]
                    if token.string == 'reverse':
                        number._type = NumberType.REVERSE
                    elif token.string == 'inverse':
                        number._type = NumberType.INVERSE
            else:
                i = self._FindBinaryOperator(token_list)
                if i >= 0:
                    token = token_list[i]
                    assoc = self._OperatorAssociativity(token.string)
                    i = self._FindBinaryOperator(token_list, assoc)
                    left_number = self.Parse(token_list[:i])
                    right_number = self.Parse(token_list[i+1:])
                    if token.string == '+':
                        number._type = NumberType.SUM
                        number.value = [left_number, right_number]
                    elif token.string == '-':
                        number._type = NumberType.SUM
                        number.value = [Number(NumberType.GEOMETRIC_PRODUCT, [Number(NumberType.SCALAR, -1.0), left_number]), right_number]
                    elif token.string == '*':
                        number._type = NumberType.GEOMETRIC_PRODUCT
                        number.value = [left_number, right_number]
                    elif token.string == '/':
                        number._type = NumberType.GEOMETRIC_PRODUCT
                        number.value = [left_number, Number(NumberType.INVERSE, [right_number])]
                    elif token.string == '.':
                        number._type = NumberType.INNER_PRODUCT
                        number.value = [left_number, right_number]
                    elif token.string == '^':
                        number._type = NumberType.OUTER_PRODUCT
                        number.value = [left_number, right_number]
                else:
                    # By checking for binary operators _before_ unary ones, I believe this is correct.
                    if token_list[0]._type == TokenType.OPERATOR:
                        token = token_list[0]
                        other_number = self.Parse(token_list[1:])
                        if token.string == '-':
                            number._type = NumberType.GEOMETRIC_PRODUCT
                            number.value = [Number(NumberType.SCALAR, -1.0), other_number]
                        elif token.string == '~':
                            number._type = NumberType.REVERSE
                            number.value = [other_number]
                    elif token_list[-1]._type == TokenType.OPERATOR:
                        token = token_list[-1]
                        other_number = self.Parse(token_list[:-1])
                        if token.string == '~':
                            number._type = NumberType.REVERSE
                            number.value = [other_number]
        if number._type == NumberType.NONE:
            raise MathException('Failed to parse token list: %s' % ';'.join([token.string for token in token_list]))
        return number
