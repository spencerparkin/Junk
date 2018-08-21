# ga_system.py

import os
import json
import cherrypy
import urllib.request
import random

from Math.math_number import Number
from Math.math_formatter import NumberFormat
from Math.math_evaluator import Evaluator

class GASystemApp(object):
    def __init__(self, root_dir):
        self.root_dir = root_dir

    @cherrypy.expose
    def default(self, **kwargs):
        return cherrypy.lib.static.serve_file(self.root_dir + '/ga_system.html', content_type='text/html')

    def _GenerateLatex(self, number):
        url = 'http://quicklatex.com/latex3.f'
        preamble = r'''
        \usepackage{amsmath}
        \usepackage{amsfonts}
        \usepackage{amssymb}
        '''
        latex_text = number.ToString(NumberFormat.LATEX)
        data = {
            'formula': latex_text,
            'fsize': '17px',
            'fcolor': '000000',
            'mode': 0,
            'out': 1,
            'preamble': preamble,
            'rnd': random.uniform(0.0, 10.0) # Defeat browser caching?
        }
        data = urllib.parse.urlencode(data)
        data = data.replace('+', '%20')
        data = data.encode('utf-8')
        request = urllib.request.Request(url, data)
        with urllib.request.urlopen(request) as response:
            text = response.read()
            text = text.decode('utf-8')
            tokens = text.split()
            return tokens[1]

    @cherrypy.expose
    @cherrypy.tools.json_out()
    def calculate(self, **kwargs):
        content_length = cherrypy.request.headers['Content-Length']
        payload = cherrypy.request.body.read(int(content_length))
        payload = payload.decode('utf-8')
        payload = json.loads(payload)
        data = {}
        try:
            input_text = payload['input_text']
            number = Number().FromString(input_text)
            data['input_text'] = number.ToString(NumberFormat.PARSEABLE)
            data['input_image_url'] = self._GenerateLatex(number)
            evaluator = Evaluator()
            evaluator.BecomeExpander()
            result = evaluator.Evaluate(number, debug_print=True)
            data['output_text'] = result.ToString(NumberFormat.PARSEABLE)
            data['output_image_url'] = self._GenerateLatex(result)
        except Exception as ex:
            import traceback
            traceback.print_exc()
            return {'error': str(ex)}
        return data

if __name__ == '__main__':
    root_dir = os.path.dirname(os.path.abspath(__file__))
    port = int(os.environ.get('PORT', 5100))
    app = GASystemApp(root_dir)
    app_config = {
        'global': {
            'server.socket_host': '0.0.0.0',
            'server.socket_port': port,
        },
        '/': {
            'tools.staticdir.root': root_dir,
            'tools.staticdir.on': True,
            'tools.staticdir.dir': '',
        },
    }
    cherrypy.quickstart(app, '/', config=app_config)