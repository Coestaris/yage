#!/usr/bin/env python3

#
# @file pack.py
# @date 24-06-2024
# @author Maxim Kurylko <vk_vm@ukr.net>
#

import glob
import json
import logging
import os
import shutil
import subprocess
import sys
import tempfile
import zipfile
from abc import abstractmethod
from typing import Dict, List

from watchdog.observers import Observer
import argparse


class Fore:
    GREEN = "\x1b[32m"
    CYAN = "\x1b[36m"
    RED = "\x1b[31m"
    YELLOW = "\x1b[33m"
    RESET = "\x1b[39m"


def get_format_string(colored: bool, details: bool) -> str:
    green = Fore.GREEN if colored else ""
    cyan = Fore.CYAN if colored else ""
    reset = Fore.RESET if colored else ""

    if details:
        return f"{green}%(asctime)s{reset} - {cyan}%(name)s:%(funcName)s:%(lineno)d{reset} - %(levelname)s - %(message)s"
    else:
        return f"{green}%(asctime)s{reset} - {cyan}%(name)s{reset} - %(levelname)s - %(message)s"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Pack assets")
    parser.add_argument("-d", "--dir", type=str, default=".", help="Directory to watch")
    parser.add_argument("-o", "--output", type=str, default="assets.pack", help="Output file")
    parser.add_argument("-w", "--watchdog", action="store_true", default=False)
    parser.add_argument("--no-compress", action="store_true", help="Do not compress the output file")
    parser.add_argument("-l", "--log", type=str, default="info", choices=["debug", "info", "warning", "error", "critical"],
                        help="Log level. Note: 'debug' log level may print sensitive information,\n"
                             "produce a lot of output and program may run slower/incorectly")
    parser.add_argument("-c", "--colorless", action="store_true", help="Disable colored output")
    return parser.parse_args()


def fast_copy(src: str, dst: str):
    if sys.platform == "win32":
        os.system(f"copy /Y {src} {dst}")
    else:
        shutil.copy(src, dst)


class PackHandler:
    def __init__(self, dir: str, output: str, no_compress: bool = False):
        self.dir = dir
        self.output = output
        self.no_compress = no_compress

    class ExtensionProcessor:
        def __init__(self, extensions: list):
            self.extensions = extensions

        def select_files(self, dir: str) -> list:
            files = []
            for ext in self.extensions:
                files.extend(glob.glob(os.path.join(dir, f"*{ext}")))

            return files

        @abstractmethod
        def process(self, dst_dir: str, work_dir: str, selected: any) -> bool:
            raise NotImplementedError

    class GroupExtensionProcessor(ExtensionProcessor):
        def select_files(self, dir: str) -> List[List[str]]:
            # Group shader files by basename
            files = {}
            for ext in self.extensions:
                for file in glob.glob(os.path.join(dir, f"*{ext}")):
                    basename = os.path.splitext(os.path.basename(file))[0]
                    if basename not in files:
                        files[basename] = []
                    files[basename].append(file)

            # Remove groups, that don't have .gltf or .glb file
            exts_to_remove = self.extensions.copy()
            exts_to_remove.remove(".meta")

            files = {k: v for k, v in files.items() if
                     any([f.endswith(ext) for f in v for ext in
                          exts_to_remove])}

            # Convert to a nested list
            files = [v for v in files.values()]

            return files

    class JustCopyProcessor(ExtensionProcessor):
        def __init__(self, extensions: list):
            super().__init__(extensions)

        def process(self, dst_dir: str, work_dir: str, file: str) -> bool:
            fast_copy(file, dst_dir)
            return True

    class ShaderProcessor(GroupExtensionProcessor):
        def __init__(self):
            super().__init__([".fsh", ".vsh", ".meta"])

        def process(self, dst_dir: str, work_dir: str, group: List[str]) -> bool:
            # Has meta file ?
            meta = None
            fsh = None
            vsh = None
            for file in group:
                if file.endswith(".meta"):
                    meta = file
                elif file.endswith(".fsh"):
                    fsh = file
                elif file.endswith(".vsh"):
                    vsh = file

            basename = os.path.basename(file)
            basename = os.path.splitext(basename)[0]
            if not meta:
                with open(os.path.join(dst_dir, f"{basename}.meta"), "w") as meta:
                    meta_content = {
                        "type": "shader",
                        "language": "GLSL",
                        "shaders": []
                    }
                    shaders = []
                    if fsh:
                        shaders.append({"type": "FRAGMENT", "uri": os.path.basename(fsh)})
                    if vsh:
                        shaders.append({"type": "VERTEX", "uri": os.path.basename(vsh)})
                    meta_content["shaders"] = shaders

                    meta.write(json.dumps(meta_content))
            else:
                fast_copy(meta, dst_dir)

            if fsh:
                fast_copy(fsh, dst_dir)
            if vsh:
                fast_copy(vsh, dst_dir)

            return True

    class ImageProcessor(GroupExtensionProcessor):
        def __init__(self):
            super().__init__([".png", ".jpeg", ".jpg", ".bmp", ".meta"])

        def process(self, dst_dir: str, work_dir: str, file: List[str]) -> bool:
            meta = None
            image = None
            for f in file:
                if f.endswith(".meta"):
                    meta = f
                else:
                    if image:
                        logging.warning(f"File {f} has the same basename as {image}")
                    image = f

            basename = os.path.basename(file[0])
            basename = os.path.splitext(basename)[0]
            if not meta:
                with open(os.path.join(dst_dir, f"{basename}.meta"), "w") as meta:
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
                        "uris": [os.path.basename(image)]
                    }
                    meta.write(json.dumps(meta_content))
            else:
                fast_copy(meta, dst_dir)

            fast_copy(image, dst_dir)
            return True

    class GLTFProcessor(GroupExtensionProcessor):
        def __init__(self):
            super().__init__([".gltf", ".glb", ".meta"])

        def process(self, dst_dir: str, work_dir: str, group: List[str]) -> bool:
            meta = None
            gltf = None
            glb = None
            for file in group:
                if file.endswith(".meta"):
                    meta = file
                elif file.endswith(".gltf"):
                    gltf = file
                elif file.endswith(".glb"):
                    glb = file

            basename = os.path.basename(file)
            basename = os.path.splitext(basename)[0]
            if not meta:
                with open(os.path.join(dst_dir, f"{basename}.meta"), "w") as meta:
                    meta_content = {
                        "type": "model",
                    }
                    if gltf:
                        meta_content["model_type"] = "GLTF"
                        meta_content["uri"] = os.path.basename(gltf)
                    if glb:
                        meta_content["model_type"] = "GLB"
                        meta_content["uri"] = os.path.basename(glb)

                    meta.write(json.dumps(meta_content))
            else:
                fast_copy(meta, dst_dir)

            if gltf:
                fast_copy(gltf, dst_dir)
            if glb:
                fast_copy(glb, dst_dir)

            return True

    PROCESSORS = [
        # We don't know what these binary files are, so copy them
        JustCopyProcessor([".bin"]),

        ShaderProcessor(),
        ImageProcessor(),
        GLTFProcessor()
    ]

    def pack(self):
        logging.debug(f"[0] Start packing {self.dir} to {self.output}")

        # Collect all files into a temporary directory,
        # to not call zipf.write() for each file
        out_temp_dir = os.path.join(tempfile.gettempdir(), "assets")
        if os.path.exists(out_temp_dir):
            shutil.rmtree(out_temp_dir)
        os.makedirs(out_temp_dir)

        for processor in self.PROCESSORS:
            selected = processor.select_files(self.dir)
            for something in selected:
                logging.debug(f"Processing {something}")
                if not processor.process(out_temp_dir, self.dir, something):
                    logging.error(f"Failed to process {something}")
                    return

        logging.info(f"Packing {out_temp_dir} to {self.output}")

        # Create a zip archive in a temporary directory
        #   so application can read it while it's being written
        out_zip = os.path.join(tempfile.gettempdir(), "assets.zip")
        with zipfile.ZipFile(out_zip, "w", zipfile.ZIP_DEFLATED if not self.no_compress else zipfile.ZIP_STORED) as zipf:
            for root, _, files in os.walk(out_temp_dir):
                for file in files:
                    zipf.write(os.path.join(root, file), os.path.relpath(os.path.join(root, file), out_temp_dir))

        # Move the temporary archive to the output directory
        shutil.move(out_zip, self.output)

    def dispatch(self, event):
        if event.event_type == "modified" or event.event_type == "created":
            for ext in self.DIRECT_EXTENSIONS + self.IMAGE_EXTENSIONS:
                if event.src_path.endswith(ext):
                    logging.info(f"File {event.src_path} changed, repacking")
                    self.pack()
                    return


def main():
    args = parse_args()

    # Set up logging
    if not args.colorless:
        logging.addLevelName(logging.CRITICAL, f"{Fore.RED}{logging.getLevelName(logging.CRITICAL)}{Fore.RESET}")
        logging.addLevelName(logging.ERROR, f"{Fore.RED}{logging.getLevelName(logging.ERROR)}{Fore.RESET}")
        logging.addLevelName(logging.WARNING, f"{Fore.YELLOW}{logging.getLevelName(logging.WARNING)}{Fore.RESET}")
        logging.addLevelName(logging.INFO, f"{Fore.GREEN}{logging.getLevelName(logging.INFO)}{Fore.RESET}")
        logging.addLevelName(logging.DEBUG, f"{Fore.CYAN}{logging.getLevelName(logging.DEBUG)}{Fore.RESET}")

    logging.getLogger().setLevel(logging.getLevelName(args.log.upper()))

    handler = logging.StreamHandler(sys.stdout)
    handler.setFormatter(logging.Formatter(get_format_string(not args.colorless, args.log == "debug")))
    logging.getLogger().addHandler(handler)

    args.dir = os.path.abspath(args.dir)
    if not os.path.exists(args.dir):
        logging.error(f"Directory {args.dir} does not exist")
        sys.exit(1)
    args.output = os.path.abspath(args.output)

    if args.no_compress:
        logging.warning("Output file will not be compressed")

    pack_handler = PackHandler(args.dir, args.output, args.no_compress)

    if args.watchdog:
        logging.info(f"Watching {args.dir} for changes")
        observer = Observer()
        observer.schedule(pack_handler, path=args.dir)
        observer.start()
        try:
            while True:
                pass
        except KeyboardInterrupt:
            observer.stop()
        observer.join()
    else:
        logging.info(f"Packing {args.dir} to {args.output}")
        pack_handler.pack()


if __name__ == '__main__':
    main()
