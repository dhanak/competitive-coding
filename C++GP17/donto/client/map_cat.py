import os

map = {}

type_to_enum = {
	'#': 'WALL',
	'T': 'TOP_LANE',
	'M': 'MIDDLE_LANE',
	'B': 'BOTTOM_LANE',
	't': 'TOP_JUNGLE',
	'b': 'BOTTOM_JUNGLE',
	'H': 'HOME_BASE',
	'E': 'ENEMY_BASE',
}

with open("map_cat.txt", "r") as f:
	y = 38
	for line in f:
		line = line.strip()
		for x in range(0, len(line)):
			map[(x,y)] = line[x]
		y -= 1

with open("MapCat.cpp", "w") as f:
	f.write('#include "stdafx.h"\n#include "MapCat.h"\n\n')
	f.write("unordered_map<POS, ZONE> Zones {\n")
	for p,t in sorted(map.iteritems()):
		f.write("\t{ POS(%d,%d), %s },\n" % (p[0], p[1], type_to_enum[t]))
	f.write("};\n")
