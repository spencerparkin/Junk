# math_calculator.py

from Math.math_number import Number
from Math.math_evaluator import Evaluator
from Math.math_exception import MathException

if __name__ == '__main__':
    evaluator = Evaluator()
    while True:
        string = input('calculator: ')
        if string == 'exit':
            break
        try:
            number = Number().FromString(string)
            result = evaluator.Evaluate(number)
            print('Input: ' + number.ToString())
            print('Output: ' + result.ToString())
        except MathException as ex:
            print('Math exception: %s' % str(ex))