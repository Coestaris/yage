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

    # Files to copy to assets.pack, without any processing
    DIRECT_EXTENSIONS = [".fsh", ".vsh", ".meta"]
    # Image files are processed differently: we need to create a
    #   .meta file for each image (if it doesn't exist) and write
    #   default values there
    IMAGE_EXTENSIONS = [".png", ".jpeg", ".jpg"]

    def pack(self):
        logging.debug(f"[0] Start packing {self.dir} to {self.output}")

        # Collect all files into a temporary directory,
        # to not call zipf.write() for each file
        out_temp_dir = os.path.join(tempfile.gettempdir(), "assets")
        if os.path.exists(out_temp_dir):
            shutil.rmtree(out_temp_dir)
        os.makedirs(out_temp_dir)

        logging.debug(f"[1] Processing direct extensions: {self.DIRECT_EXTENSIONS}")
        for ext in self.DIRECT_EXTENSIONS:
            for file in glob.glob(os.path.join(self.dir, "*" + ext)):
                logging.info(f"Adding %s to assets.pack", file)
                fast_copy(file, out_temp_dir)

        logging.debug(f"[2] Processing image extensions: {self.IMAGE_EXTENSIONS}")
        for ext in self.IMAGE_EXTENSIONS:
            for file in glob.glob(os.path.join(self.dir, "*" + ext)):
                basename = os.path.basename(file)
                # Get rid of the extension
                basename = os.path.splitext(basename)[0]

                # If .meta file not exists, create it
                metafile = os.path.join(self.dir, f"{basename}.meta")
                if not os.path.exists(metafile):
                    with open(os.path.join(out_temp_dir, f"{basename}.meta"), "w") as meta:
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
                    fast_copy(metafile, out_temp_dir)

                logging.debug(f"Adding %s to assets.pack", file)
                fast_copy(file, out_temp_dir)

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
