#!/usr/bin/env python3
"""Make NAME available in the current directory, fetching URL only if the shared cache lacks it.

usage: fetch_mcpl.py NAME URL   (cache: $MCCODE_MCPL_CACHE or ~/.cache/mccode/mcpl)
"""
import os
import shutil
import sys
import time
import urllib.request

name, url = sys.argv[1], sys.argv[2]
cache = os.environ.get("MCCODE_MCPL_CACHE", os.path.expanduser("~/.cache/mccode/mcpl"))
cached = os.path.join(cache, name)

if not (os.path.isfile(cached) and os.path.getsize(cached) > 0):
    os.makedirs(cache, exist_ok=True)
    part = f"{cached}.{os.getpid()}.part"
    for attempt in range(5):
        try:
            with urllib.request.urlopen(url, timeout=60) as src, open(part, "wb") as dst:
                shutil.copyfileobj(src, dst)
            os.replace(part, cached)  # atomic: concurrent workers never see a partial file
            break
        except OSError as err:
            if os.path.exists(part):
                os.remove(part)
            if attempt == 4:
                sys.exit(f"fetch_mcpl: could not fetch {url}: {err}")
            time.sleep(2 ** attempt)

if not os.path.exists(name):
    try:
        os.symlink(cached, name)
    except OSError:  # e.g. Windows without symlink rights
        shutil.copyfile(cached, name)
