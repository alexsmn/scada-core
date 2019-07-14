from argparse import ArgumentParser
from jinja2 import Environment, FileSystemLoader, Template


parser = ArgumentParser()
parser.add_argument('input', help='Input file')
parser.add_argument('namespace', nargs='?', default='', help='Namespace')
args = parser.parse_args()

namespace = args.namespace
input_path = args.input

items = []
lines = open(input_path).readlines()
for line in lines:
  name, id, type = line.rstrip().split(',')
  items.append({'name': name, 'id': id, 'type': type})
items.sort(key=lambda item:item['name'])

loader=FileSystemLoader('.')
env = Environment(loader=loader,
  trim_blocks=True,
  lstrip_blocks=True)
template = env.get_template('node_ids.h.template')
print(template.render(namespace=namespace, items=items))