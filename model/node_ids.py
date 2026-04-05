import csv
import re
from glob import glob


class Record:
    pass

names = dict()

for path in glob("*_node_ids.h"):
    with open(path, "r", newline="") as file:
        for line in file.readlines():
            line = line.strip()
            p = re.match("const scada::NumericId (.+) = (\d+);", line)
            if p:
                names[p.group(1)] = int(p.group(2))
            p = re.match("const scada::NodeId (.+)\{(\d+),", line)
            if p:
                names[p.group(1)] = int(p.group(2))

with open("scada_model.csv", "w", newline="") as file:
    writer = csv.writer(file, delimiter=",")
    for name, id in names.items():
        writer.writerow([name, id])
