#!/usr/bin/python3
"""
Generates the metadata file for the any% save files.
"""
import copy
from enum import IntEnum
import struct

class Requirements(IntEnum):
    POS = 1
    CAM = 2

default_entry = {
    "requirements": 0,
    "pos": (0.0, 0.0, 0.0),
    "angle": 0,
    "cam": {"pos": (0, 0, 0), "target": (0, 0, 0)},
    "counter": 0,
}

# order matters
# must be the .bin filenames
file_names = [
    "ordon_gate_clip",
    "ordon_gate_clip",
    "goats",
    "sewers",
    "sewers_tower",
    "rooftops",
    "hugo",
    "ems",
    "purple_mist",
    "forest_bit",
    "forest_escape",
    "gorge_void",
    "rupee_roll",
    "lanayru_gate_clip",
    "karg_fight",
    "karg",
    "eldin_twilight",
    "basement_bugs",
    "eld_inn",
    "bomb_house_skip",
    "lanayru_twilight",
    "inner_zd_bug",
    "waterfall_sidehop",
    "ct_bug",
    "dock_bug",
    "boss_bug",
    "iza",
    "plumm_oob",
    "elh",
    "lakebed_1",
    "wormhole",
    "deku_toad",
    "lakebed_bk_skip",
    "morpheel",
    "louise_glitch",
    "rope_skip",
    "mdh_tower",
    "mdh_bridge",
    "post_mdh",
    "snowpeak_cave",
    "spr_warp",
    "blind_snowboarding",
    "spr",
    "lfc",
    "darkhammer",
    "fence_clip",
    "camp",
    "ag",
    "poe_gate_clip",
    "ag_early_bk",
    "ag_tss",
    "death_sword_skip",
    "epic_spinner",
    "stallord",
    "stallord",
    "stallord",
    "stallord",
    "stallord",
    "mirror_chamber",
    "cits_early",
    "cits_1",
    "arg_cs_skip",
    "city_east_wing",
    "city_gate_clip",
    "aeralfos_skip",
    "cits_2",
    "city_bk_clip",
    "fan_tower",
    "argorok",
    "palace_1",
    "pz1",
    "stupid_room",
    "smart_room",
    "pz2",
    "palace_2",
    "pot_bk",
    "early_platform",
    "zant",
    "zant_dangoro",
    "zant_final",
    "hc_mailman",
    "hc",
    "kb4",
    "darknut",
    "hc_aeralfos",
    "hc_tower",
    "beast_ganon",
    "horseback_ganon",
    "ganondorf",
]

any_p = [{**copy.deepcopy(default_entry), "id": i, "filename": name}
            for i, name in enumerate(file_names)]

file_dict = {}
for i, e in enumerate(file_names):
    if not e in file_dict:
        file_dict[e] = [i]
    else:
        file_dict[e].append(i)

def update_entry(filename, data, n = 1):
    count = sum(1 for entry in any_p if entry["filename"] == filename)
    if n <= count and n > 0:
        any_p[file_dict[filename][n - 1]] = {**any_p[file_dict[filename][n - 1]], **data}

# ordon gate clip
# for each of these, the angle is unsigned. putting a negative for the angle will crash when loading the saves and all saves after
# the camera stuff seems to be dysfunctional
update_entry("ordon_gate_clip", n = 1, data = {
    'requirements': Requirements.POS | Requirements.CAM,
    'pos': (827.450012, 216.490097, -4533.90625),
    'angle': 498,
    'cam': {
        'pos': (833.467468, 477.604675, -4241.97266),
        'target': (827.497559, 329.622986, -4532.90723)
    },
    'counter': 10
})

update_entry("ordon_gate_clip", n = 2, data = {
    'requirements': Requirements.POS | Requirements.CAM,
    'pos': (466.622467, 319.770752, -11651.3867),
    'angle': 52540,
    'cam': {
        'pos': (735.525391, 524.418701, -11576.4746),
        'target': (465.674622, 421.052704, -11651.0684)
    },
    'counter': 10
})

# back in time
update_entry("bit", {
    'requirements': Requirements.POS | Requirements.CAM,
    'pos': (466.622467, 319.770752, -11651.3867),
    'angle': 52540,
    'cam': {
        'pos': (735.525391, 524.418701, -11576.4746),
        'target': (465.674622, 421.052704, -11651.0684)
    },
    'counter': 10
})

# sewers tower
update_entry("sewers_tower", {
    'requirements': Requirements.POS | Requirements.CAM,
    'pos': (25524, -3010, 8087),
    'angle': 32737,
    'cam': {
        'pos': (25771.3672, -2753.22388, 7785.64941),
        'target': (25535.5352, -2890, 8078.35254)
    },
    'counter': 30
})

# sewers rooftops
update_entry("rooftops", {
    'requirements': Requirements.POS,
    'pos': (27441, 2930, 6796),
    'angle': 32465,
    'counter': 30
})

# hugo
update_entry("hugo", {
    'requirements': Requirements.POS | Requirements.CAM,
    'pos': (701.797302, 85.5212784, -5299.6123),
    'angle': 63622,
    'cam': {
        'pos': (735.525391, 524.418701, -11576.4746),
        'target': (465.674622, 421.052704, -11651.0684)
    },
})

# purple mist
update_entry("purple_mist", {
    'requirements': Requirements.POS,
    'pos': (-23524.6152, 250.0, -16220.166),
    'angle': 40758,
    'counter': 30
})

# forest escape
update_entry("forest_escape", {
    'requirements': Requirements.POS | Requirements.CAM,
    'pos': (-13485.0713, -72, -15362.1299),
    'angle': 27375,
    'cam': {
        'pos': (-12552.8252, -53.5801048, -16729.5313),
        'target': (-12433.2979, -106.667023, -17104.9512)
    },
    'counter': 30
})

# gorge void
update_entry("gorge_void", {
    'requirements': Requirements.POS,
    'pos': (-9704.47266, -7200.0, 58475.5195),
    'angle': 54288,
})

# rupee roll
update_entry("rupee_roll", {
    'requirements': Requirements.POS | Requirements.CAM,
    'pos': (-11130.208, -5700.0, 56423.1953),
    'angle': 31571,
    'cam': {
        'pos': (-11178.1504, -5506.71338, 56843.1797),
        'target': (-11124.4697, -5589.99902, 56373.5195)
    },
    'counter': 15
})

# lanayru gate clip
update_entry("lanayru_gate_clip", {
    'requirements': Requirements.POS | Requirements.CAM,
    'pos': (-63026.2852, -9065.92578, 71680.3438),
    'angle': 44248,
    'cam': {
        'pos': (-62655.8125, -8900.91309, 71903.6328),
        'target': (-63064.2148, -8969.97656, 71661.0781)
    },
    'counter': 15
})

# kargarok rider fight
update_entry("karg_fight", {
    'requirements': Requirements.POS,
    'pos': (-107015.05, -23436.449, 47115.9219),
    'angle': 52164,
    'counter': 30
})

# eldin twilight
update_entry("eldin_twilight", {
    'requirements': Requirements.POS | Requirements.CAM,
    'pos': (455.088379, -150.0, 11516.7227),
    'angle': 6058,
    'cam': {
        'pos': (219.367218, -20.1253014, 11157.582),
        'target': (482.515137, -39.9999771, 11558.5283)
    },
    'counter': 10
})

# inner zd bug
update_entry("inner_zd_bug", {
    'requirements': Requirements.POS | Requirements.CAM,
    'pos': (0, -8.27449322, -2750),
    'angle': 32768,
    'cam': {
        'pos': (-280.019684, 76.9521179, -2899.97949),
        'target': (8.7422813, 86.7255249, -2740)
    },
    'counter': 30
})

# ct bug
update_entry("ct_bug", {
    'requirements': Requirements.POS,
    'pos': (2185.91479, -725, 6443.65088),
    'angle': 19198,
})

# boss bug
update_entry("boss_bug", {
    'requirements': Requirements.POS,
    'pos': (-89100.00, -18811.2363, 39410.00),
    'angle': 21504,
})

# iza
update_entry("iza", {
    'requirements': Requirements.POS,
    'pos': (5979.97217, 150.0, -2748.34155),
    'angle': 10114,
})

# snowpeak messenger skip
update_entry("spr_warp", {
    'requirements': Requirements.POS | Requirements.CAM,
    'pos': (-9294.87988, 980.0, -11712.3838),
    'angle': 346,
    'cam': {
        'pos': (-9256.47559, 1200, -11842.25),
        'target': (-9256.6748, 1140, -11712.25)
    },
    'counter': 10
})

# spr
update_entry("spr", {
    'requirements': Requirements.POS,
    'pos': (0.0, -150.0, 6000.0),
    'angle': 33768,
})

# bk skip
update_entry("lakebed_bk_skip", {
    'requirements': Requirements.POS | Requirements.CAM,
    'pos': (71.9835968, 1500.00, 2839.01587),
    'angle': 32767,
    'cam': {
        'pos': (71.9835968, 1719.93542, 2969.04565),
        'target': (71.9835968, 1660.0, 2839.01587)
    },
    'counter': 30
})

# morpheel
update_entry("morpheel", {
    'requirements': Requirements.POS | Requirements.CAM,
    'pos': (-1193.0, -23999.00, -770.0),
    'angle': 10754
})

# louise glitch
update_entry("louise_glitch", {
    'requirements': Requirements.POS,
    'pos': (2861.78076, -1150, 4507.41846),
    'angle': 32750,
})

# rope skip
# this spawns outside of telma's bar, because the save being in telma's bar will always set it to the wrong layer (4) for some reason
update_entry("rope_skip", {
    'requirements': Requirements.POS,
    'pos': (3159.06836, -500, 5492.82129),
    'angle': 33276,
})

# poe gate clip
update_entry('poe_gate_clip', data = {
    'requirements': Requirements.POS | Requirements.CAM,
    'pos': (10.0035, -19.5500, -600.5942),
    'angle': 32768
})

# triple stalfos skip
update_entry('ag_tss', data = {
    'requirements': Requirements.POS | Requirements.CAM,
    'pos': (-4673.3042, -1700, -2881.75537),
    'angle': 15916,
    'cam': {
        'pos': (-4833.16895, -1480, -2907.09399), 
        'target': (-4703.30225, -1540, -2901.21313)
    },
    'counter': 30,
})

# snowpeak connection cave
update_entry("snowpeak_cave", {
    'requirements': Requirements.POS,
    'pos': (-4615.54688, -1243.95117, -10858.0576),
    'angle': 55644,
})

# blind snowboarding
update_entry("blind_snowboarding", {
    'requirements': Requirements.POS,
    'pos': (-14476.7305, 1814.04504, -9606.24609),
    'angle': 36106,
})

# ladder freezard cancel
update_entry("lfc", {
    'requirements': Requirements.POS,
    'pos': (-2645.54785, 0, -4794.21094),
    'angle': 32768,
})

# desert fence clip
update_entry("fence_clip", {
    'requirements': Requirements.POS,
    'pos': (3885.38916, -732.859985, 18491.7715),
    'angle': 40266,
})

# ag epic spinner forwards
update_entry('epic_spinner', data = {
    'requirements': Requirements.POS | Requirements.CAM,
    'pos': (-4813.71777, -1700, -2830.35278),
    'angle': 49152,
    'counter': 10
})

# mirror chamber portal
update_entry('mirror_chamber', {
    'requirements': Requirements.POS | Requirements.CAM,
    'pos': (5336.63672, 4392.44238, -20123.7754),
    'angle': 54828,
    'cam': {
        'pos': (5772.17188, 4685.60254, -20415.6738),
        'target': (5344.75195, 4542.44629, -20157.0879)
    },
    'counter': 10
})

# city 1
update_entry("cits_1", {
    'requirements': Requirements.POS,
    'pos': (1309.60645, -240.0, 5533.43848),
    'angle': 16384,
    'counter': 10
})

# city east wing inside 1
update_entry('city_east_wing', data = {
    'requirements': Requirements.POS,
    'pos': (10650.8516, 0, -12425),
    'angle': 16384
})

update_entry('city_gate_clip', data = {
    'requirements': Requirements.POS | Requirements.CAM,
    'pos': (16516.6094, 0, -12722.480),
    'angle': 16384
})

# city bk clip aka kai clip
update_entry('city_bk_clip', data = {
    'requirements': Requirements.POS | Requirements.CAM,
    'pos': (1094.59277, 1312.70996, -11795.959),
    'angle': 52312,
    'cam': {
        'pos': (1660.85669, 1466.8136, -12006.7412),
        'target': (1096.87207, 1362.71021, -11830.5078)
    },
    'counter': 30 
})

# pot right wing inside 2
update_entry('smart_room', data = {
    'requirements': Requirements.POS | Requirements.CAM,
    'pos': (3999.17676, -725, 249.467438),
    'angle': 32768,
    'cam': {
        'pos': (4036.18408, -505, 378.042297),
        'target': (4026.32227, -565, 248.41687)
    },
    'counter': 30
})

# Wii specific entries

update_entry("early_gale", {
    'requirements': Requirements.POS,
    'pos': (-3000, 4302.1, 5084.73),
    'angle': 32768,
    'counter': 10
})


update_entry("midna_dive", {
    'requirements': Requirements.POS,
    'pos': (-89349.5132, -18829.6895, 39521.6523),
    'angle': 31044,
    'counter': 10
})

file = open("any.bin", "wb")

for entry in any_p:
    print(entry)
    file.write(entry["requirements"].to_bytes(1, "big", signed=False))
    file.write(int(0).to_bytes(1, "big", signed=False))  # padding
    file.write(entry["angle"].to_bytes(2, "big", signed=False))
    file.write(struct.pack('>fff', *entry["pos"]))
    file.write(struct.pack('>fff', *entry["cam"]["pos"]))
    file.write(struct.pack('>fff', *entry["cam"]["target"]))
    file.write(entry["counter"].to_bytes(4, "big", signed=False))
    file.write(struct.pack(">32s", entry["filename"].encode("ascii")))
    file.write(int(0).to_bytes(4, "big", signed=False))  # padding

