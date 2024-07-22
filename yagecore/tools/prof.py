#!/usr/bin/env python3

#
# @file prof.py
# @date 18-06-2024
# @author Maxim Kurylko <vk_vm@ukr.net>
#

import argparse
import os
from datetime import datetime

POINTS = {
    0: "R_PRE_FRAME",
    1: "R_DRAW_START",
    2: "R_DRAW_END",
    3: "R_UPDATE_START",
    4: "L_PRE_FRAME",
    5: "L_INPUT",
    6: "L_UPDATE_OBJECTS",
    7: "L_UPDATE_PARTICLES",
    8: "L_UPDATE_PHYSICS",
    9: "L_UPDATE_AI",
    10: "L_UPDATE_SOUND",
    11: "L_POST_FRAME",
}

class Stamp:
    def __init__(self, point: int, time: float, source: str):
        self.source = source
        self.point = point
        # Convert nanoseconds to datetime
        self.time = time


def main():
    parser = argparse.ArgumentParser(
        description='Profiling tool for the game engine')
    parser.add_argument('files', metavar='file', type=str, nargs='+',
                        help='Files to process')
    args = parser.parse_args()

    print(args.files)

    stamps = []
    for file in args.files:
        with open(file, 'r') as f:
            for line in f:
                try:
                    point, time = line.split()
                    stamps.append(Stamp(int(point), float(time), file))
                except:
                    continue
    stamps.sort(key=lambda x: x.time)
    stamps = stamps[:10000]

    print("╔══════════════════════════════════", end='')
    for file in args.files:
        print("╤══════════════════════", end='')
    print("╗")
    print("║ Time                             ", end='')
    for file in args.files:
        print(f"│ {os.path.basename(file):21}", end='')
    print("║")
    print("╠══════════════════════════════════╪══════════════════════╪══════════════════════╣")

    for stamp in stamps:
        ns = stamp.time
        dt = ns // 1e9
        dt = datetime.fromtimestamp(dt)

        ms = (ns % 1e9) / 1e6
        us = (ns % 1e6) / 1e3
        ns = ns % 1e3
        formatted = f"{dt}.{ms:3.0f}.{us:3.0f}.{ns:3.0f}"
        print(f"║ {formatted:32} ", end='')
        for file in args.files:
            if file == stamp.source:
                print(f"│ {POINTS[stamp.point]:21}", end='')
            else:
                print(f"│ {'':21}", end='')
        print("║")


if __name__ == '__main__':
    main()
