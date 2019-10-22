from argparse import ArgumentParser
from xml.etree import ElementTree
import json
import os


def collect_names(element):
    names = []
    symbolic_name = element.attrib.get("SymbolicName", None)
    if symbolic_name:
        names.append(symbolic_name)
    for child in element:
        names += collect_names(child)
    return names


parser = ArgumentParser()
parser.add_argument('input', help='Input XML file')
parser.add_argument('output', help='Output JSON file')
parser.add_argument('locale', action='extend', nargs="+", type=str, help='Locales')
args = parser.parse_args()

input_path = args.input
output_path = args.output
locale_ids = args.locale

xml = ElementTree.parse(input_path)
names = collect_names(xml.getroot())

translations = {}
if os.path.exists(output_path):
    with open(output_path, encoding="utf8") as f:
        translations = json.load(f)
for locale_id in locale_ids:
    locale = translations.setdefault(locale_id, {})
    for name in names:
        locale.setdefault(name, "")
with open(output_path, "w", encoding="utf8") as f:
    json.dump(translations, f, sort_keys=True, indent=2, ensure_ascii=False)