# math_exception.py

class MathException(Exception):
    def __init__(self, string):
        super().__init__(string)