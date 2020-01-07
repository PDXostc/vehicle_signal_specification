#!/usr/bin/env python

from distutils.core import setup

setup(
    name='vehicle_signal_specification',
    version=open('../VERSION', 'r').read().replace('\n', ''),
    description='GENIVI Vehicle Signal Specification tooling.',
    url='https://github.com/GENIVI/vehicle_signal_specification',
    license='Mozilla Public License v2',
    py_modules=['vspec'],
    scripts=['vspec2csv.py', 'vspec2franca.py', 'vspec2cnative.py', 'vspec2json.py', 'vspec2c.py'],
    python_requires='>=3.4'
)
