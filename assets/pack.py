#!/usr/bin/env python3

#
# @file pack.py
# @date 24-06-2024
# @author Maxim Kurylko <vk_vm@ukr.net>
#

import glob
import json
import os
import tempfile
import zipfile


def main():
    ext_to_pack = [".fsh", ".vsh", ".meta"]
    im_ext = [".png", ".jpeg"]
    with zipfile.ZipFile("assets.zip", "w", zipfile.ZIP_DEFLATED) as zipf:
        for ext in ext_to_pack:
            for file in glob.glob(f"*{ext}"):
                print(f"Adding {file} to assets.pack")
                zipf.write(file)

        for ext in im_ext:
            for file in glob.glob(f"*{ext}"):
                basename = os.path.basename(file)
                # Get rid of the extension
                basename = os.path.splitext(basename)[0]

                # If .meta file not exists, create it
                if not os.path.exists(f"{basename}.meta"):
                    with tempfile.NamedTemporaryFile(mode="w",
                                                     delete=False) as meta:
                        metafile = meta.name
                        meta_content = {
                            "type": "texture",
                            "tex_type": "TEXTURE_2D",
                            "format": "RGBA",
                            "border_color": [0, 0, 0, 0],
                            "min_filter": "LINEAR",
                            "mag_filter": "LINEAR",
                            "wrap_s": "CLAMP_TO_EDGE",
                            "wrap_t": "CLAMP_TO_EDGE",
                            "wrap_r": "CLAMP_TO_EDGE",
                            "textures": [os.path.basename(file)]
                        }
                        meta.write(json.dumps(meta_content))
                else:
                    metafile = f"{basename}.meta"

                print(f"Adding {file} to assets.pack")
                zipf.write(file)
                zipf.write(metafile, f"{basename}.meta")


if __name__ == '__main__':
    main()
