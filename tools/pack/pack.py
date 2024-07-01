#!/usr/bin/env python3
import argparse
#
# @file pack.py
# @date 24-06-2024
# @author Maxim Kurylko <vk_vm@ukr.net>
#

import glob
import json
import logging
import os
import sys
import tempfile
import zipfile
from watchdog.observers import Observer


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
    parser.add_argument("-l", "--log", type=str, default="info", choices=["debug", "info", "warning", "error", "critical"],
                        help="Log level. Note: 'debug' log level may print sensitive information,\n"
                             "produce a lot of output and program may run slower/incorectly")
    parser.add_argument("-c", "--colorless", action="store_true", help="Disable colored output")
    return parser.parse_args()


class PackHandler:
    def __init__(self, dir: str, output: str):
        self.dir = dir
        self.output = output

    # Files to copy to assets.pack, without any processing
    DIRECT_EXTENSIONS = [".fsh", ".vsh", ".meta"]
    # Image files are processed differently: we need to create a
    #   .meta file for each image (if it doesn't exist) and write
    #   default values there
    IMAGE_EXTENSIONS = [".png", ".jpeg"]

    def pack(self):
        # Create pack file in a temporary location, so that we don't
        #   corrupt it if something goes wrong
        with tempfile.NamedTemporaryFile(delete=False) as temp:
            out_temp = temp.name

        with zipfile.ZipFile(out_temp, "w", zipfile.ZIP_DEFLATED) as zipf:
            for ext in self.DIRECT_EXTENSIONS:
                for file in glob.glob(os.path.join(self.dir, "*" + ext)):
                    logging.debug(f"Adding %s to assets.pack", file)
                    zipf.write(file, os.path.basename(file))

            for ext in self.IMAGE_EXTENSIONS:
                for file in glob.glob(os.path.join(self.dir, "*" + ext)):
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

                    logging.debug(f"Adding %s to assets.pack", file)
                    zipf.write(file, basename + ext)
                    zipf.write(metafile, f"{basename}.meta")

        # Move the temporary file to the final location
        logging.info(f"Moving {out_temp} to {self.output}")
        os.replace(out_temp, self.output)

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

    pack_handler = PackHandler(args.dir, args.output)

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
