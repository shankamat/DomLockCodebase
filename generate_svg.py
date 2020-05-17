"""
Samples of the various charts. Run this script to generate the reference
samples.
"""

import os

from svg.charts.plot import Plot
from svg.charts import bar
from svg.charts import time_series
from svg.charts import pie
from svg.charts import schedule
from svg.charts import line


def generate_samples():
    yield 'Line', SampleBar.sample_Line()
    

class SampleBar:
    fields = ['0.0','0.1','0.2','0.3','0.4','0.5','0.6','0.7','0.8','0.9','1.0']

    @classmethod
    def sample_Line(cls):
        g = line.Line()
        options = dict(
            scale_integers=False,
			scale_divisions = 0.1,
            area_fill=True,
            width=640,
            height=480,
            fields=SampleBar.fields,
			x_title='Value of a',
            y_title='Average degree of precision',
            graph_title='',
            show_graph_title=True,
			show_x_title=True,
			show_y_title=True,
            no_css=False,
        )
        g.__dict__.update(options)
        g.add_data({'data': [0.621,0.614,0.617,0.618,0.618,0.619,0.619,0.619,0.620,0.618,0.619], 'title': '100 Nodes'})
        g.add_data({'data': [0.602,0.602,0.603,0.603,0.603,0.601,0.603,0.601,0.603,0.604,0.603], 'title': '1000 Nodes'})
        g.add_data({'data': [0.583,0.583,0.581,0.581,0.581,0.582,0.582,0.581,0.582,0.582,0.582], 'title': '10000 Nodes'})
        g.add_data({'data': [0.566,0.567,0.566,0.567,0.566,0.566,0.566,0.566,0.567,0.566,0.566], 'title': '100000 Nodes'})
        g.add_data({'data': [0,0,0,0,0,0,0,0,0,0,0], 'title': ''})
        return g

def save_samples():
    root = os.path.dirname(__file__)
    for sample_name, sample in generate_samples():
        res = sample.burn()
        with open(os.path.join(root, sample_name + '.py.svg'), 'w') as f:
            f.write(res)


if __name__ == '__main__':
    save_samples()
