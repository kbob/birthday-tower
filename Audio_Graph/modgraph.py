#!/usr/bin/env python3

import fileinput

replacements = [
    ('AudioSynthSimpleDrum', 'AudioSynthMutableFMDrum'),
    ('AudioSynthKarplusStrong', 'AudioSynthMutableBassDrum'),
    ('AudioEffectMidSide', 'AudioEffectMutableReverb'),
    ('drum1', 'top1'),
    ('drum2', 'top2'),
    ('drum3', 'top3'),
    ('drum4', 'top4'),
    ('drum5', 'bottom1'),
    ('drum6', 'bottom2'),
    ('drum7', 'bottom3'),
    ('drum8', 'bottom4'),
    ('string1', 'launch1'),
    ('midside1', 'reverb1'),
]

for line in fileinput.input():
    for (old, new) in replacements:
        line = line.replace(old, new)
    print(line.strip())
