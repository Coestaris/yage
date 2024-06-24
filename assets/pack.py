#!/usr/bin/env python3

#
# @file pack.py
# @date 24-06-2024
# @author Maxim Kurylko <vk_vm@ukr.net>
#

import glob
import zipfile


def main():
    ext_to_pack = [".fsh", ".vsh", ".meta"]
    with zipfile.ZipFile("assets.zip", "w", zipfile.ZIP_DEFLATED) as zipf:
        for ext in ext_to_pack:
            for file in glob.glob(f"*{ext}"):
                print(f"Adding {file} to assets.pack")
                zipf.write(file)


if __name__ == '__main__':
    main()
