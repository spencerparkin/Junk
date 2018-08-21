# MathConsole.py

if __name__ == '__main__':
    # Here we are going to use the Python interpreter is our parser.
    # Here is an example...
    # Prompt: a = MathObject.Cast('a')
    # Prompt: b = MathObject.Cast(1.0)
    # Prompt: c = a + b
    # Prompt: r = c.Simplify()
    # Prompt: print(r)
    # Note that there is no code sanitization here.  If I ever wanted
    # to expose this system on a web-page that rendered nice Latex output,
    # then I would have to worry about sanitizing the Python input.
    locals_dict = {}
    while True:
        python_code = input('Prompt: ')
        if python_code == 'exit':
            break
        try:
            from MathObject import MathObject
            exec(python_code, None, locals_dict)
        except Exception as ex:
            error = str(ex)
            print('Error: ' + error)