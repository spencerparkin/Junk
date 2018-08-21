# MathException.py

class MathException(Exception):
    def __init__(self, error):
        super().__init__(error)