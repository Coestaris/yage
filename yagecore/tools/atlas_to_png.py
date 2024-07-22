#!/usr/bin/env python3

#
# @file atlas_to_png
# @date 22-07-2024
# @author Maxim Kurylko <vk_vm@ukr.net>
#

import argparse
from PIL import Image

def main():
    parser = argparse.ArgumentParser(description='Convert atlas to png')
    parser.add_argument('input', type=str, help='Input atlas file')
    parser.add_argument('output', type=str, help='Output png file')
    args = parser.parse_args()

    with open(args.input, 'rb') as f:
        data = f.read()
        w = int.from_bytes(data[0:4], byteorder='little')
        h = int.from_bytes(data[4:8], byteorder='little')
        data = data[8:]

        im = Image.frombytes('L', (w, h), data)
        im.save(args.output)


if __name__ == '__main__':
    main()
