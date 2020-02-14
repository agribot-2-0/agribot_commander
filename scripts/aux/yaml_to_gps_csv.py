import sys
import yaml

filename = sys.argv[1]

latitudes = []
longitudes = []

with open(filename, 'r') as file:
    d = dict(yaml.safe_load(file))
    for p in d.itervalues():
        latitudes.append(p["position"][0])
        longitudes.append(p["position"][1])

for i, v in enumerate(latitudes):
    print('{},{}'.format(latitudes[i], longitudes[i]))
