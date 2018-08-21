# math_tokenizer.py

import re

from Math.math_exception import MathException

class TokenType:
    NONE = 'TokenType.NONE'
    LEFT_PARAN = 'TokenType.LEFT_PARAN'
    RIGHT_PARAN = 'TokenType.RIGHT_PARAN'
    OPERATOR = 'TokenType.OPERATOR'
    IDENTIFIER = 'TokenType.IDENTIFIER'
    LITERAL = 'TokenType.LITERAL'
    DELIMITER = 'TokenType.DELIMITER'

class Token(object):
    def __init__(self, _type, string):
        self._type = _type
        self.string = string

class Tokenizer(object):
    def __init__(self):
        pass

    def TokenizeString(self, string):
        token_list = []
        while len(string) > 0:
            string = string.strip()
            token = None
            if string[0] == '(':
                token = Token(TokenType.LEFT_PARAN, string[0])
            elif string[0] == ')':
                token = Token(TokenType.RIGHT_PARAN, string[0])
            elif string[0] == ',':
                token = Token(TokenType.DELIMITER, string[0])
            elif any([string[0] == char for char in ['-', '+', '*', '/', '.', '^', '~']]):
                token = Token(TokenType.OPERATOR, string[0])
            else:
                match = re.match(r'^([$a-zA-Z_]+[0-9]*).*', string)
                if match is not None:
                    token = Token(TokenType.IDENTIFIER, match.group(1))
                else:
                    match = re.match(r'^([0-9]+[0-9.]*).*', string)
                    if match is not None:
                        # TODO: This doesn't work in a few cases where the period is ambiguous.
                        #       It could be the inner product or a decimal point.  I may just leave
                        #       it to the user to disambiguate things using parenthesis.
                        token = Token(TokenType.LITERAL, match.group(1))
            if token is None:
                raise MathException('Failed to parse token at: %s' % string)
            else:
                token_list.append(token)
                string = string[len(token.string):]
        return token_list