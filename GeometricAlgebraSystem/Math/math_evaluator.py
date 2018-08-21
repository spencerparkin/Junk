# math_evaluator.py

class Evaluator(object):
    def __init__(self):
        self.manipulator_list = []
    
    def Evaluate(self, number, debug_print=False):
        from Math.math_formatter import Formatter, NumberFormat
        formatter = Formatter(NumberFormat.PARSEABLE)
        number = number.Clone()
        # Note that for now we assume that the manipulators can be run in any order,
        # and that eventually the algorithm will terminate.  It's possibly, however,
        # that some manipulators may fight back and forth.
        keep_going = True
        while keep_going:
            keep_going = False
            for manipulator in self.manipulator_list:
                while True:
                    manipulated, number = self._ApplyManipulator(manipulator, number)
                    if manipulated:
                        keep_going = True
                        if debug_print:
                            print(formatter.Print(number))
                    else:
                        break
        return number
    
    def _ApplyManipulator(self, manipulator, number):
        manipulated, number = manipulator.Manipulate(number)
        if not manipulated and type(number.value) is list:
            for i, other_number in enumerate(number.value):
                manipulated, other_number = self._ApplyManipulator(manipulator, other_number)
                if manipulated:
                    number.value[i] = other_number
                    break
        return manipulated, number
    
    def BecomeExpander(self):
        from Math.Manipulators.math_assoc_manipulator import AssociativePropertyManipulator
        from Math.Manipulators.math_distrib_manipulator import DistributivePropertyManipulator
        from Math.Manipulators.math_ip_manipulator import InnerProductManipulator
        from Math.Manipulators.math_op_manipulator import OuterProductManipulator
        from Math.Manipulators.math_gp_manipulator import GeometricProductManipulator
        from Math.Manipulators.math_prod_manipulator import ProductManipulator
        from Math.Manipulators.math_sum_manipulator import SumManipulator
        from Math.Manipulators.math_rev_manipulator import ReverseManipulator
        self.manipulator_list = [
            AssociativePropertyManipulator(),
            DistributivePropertyManipulator(),
            InnerProductManipulator(),
            OuterProductManipulator(),
            GeometricProductManipulator(),
            ProductManipulator(),
            SumManipulator(),
            ReverseManipulator()
        ]