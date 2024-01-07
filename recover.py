#!/usr/bin/python3
# -*- coding: utf-8 -*-

import os
import sys
import subprocess
import io
import time
from pathlib import Path

dev = "/dev/disk3s2"
idx = "0"
folder = "/备份/ctripwiki"
out = "/Volumes/wd4black/backfrombroken"
exclude = []
# ["/Users/matthew/Library/Developer/Xcode/DerivedData",
#            "/Users/matthew/Library/Application Support/Google/Chrome"]
paths = []

# stats
last_ts = 0.0
list_errors = 0
list_count = 0
list_total = 125267
# list_total = 8891
recover_errors = 0

def ls(path):
    global list_errors, list_count
    dirs = []
    files = []
    args = ["./bin/apfs-list", dev, idx, path]
    p = subprocess.run(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    list_count = list_count + 1
    if p.returncode == 0:
        lines = p.stderr.decode("utf-8")
        for ln in lines.split("\n"):
            if ln.find("- DIR REC") >= 0:
                parts = ln.split("||")
                if len(parts) >= 4:
                    name = parts[-1].strip().split("=")[-1].strip()
                    if parts[1].strip() == "Dirctry":
                        dirs.append(name)
                    else:
                        files.append(name)
    else:
        list_errors = list_errors+1
        print("\n" + " ".join(args))
    return [dirs, files]

def scan(path):

    global total, last_ts, paths, list_errors, list_count
    for e in exclude:
        if e == path:
            return 0

    dirs,files = ls(path)
    for f in files:
        paths.append(path + "/" + f)
    if time.time() - last_ts > 2.0:
        print("Scanning", len(paths), "files...", "%.1f%%" % (len(paths)*100.0/list_total), "(%i errors %.1f%%)" % (list_errors, list_errors*100.0/list_count))
        last_ts = time.time()

    # and then scan the subfolders...
    for d in dirs:
        s = path + "/" + d
        scan(s)

scan(folder)

if 1:
    start_ts = time.time()
    created = dict()

    for progress, i in enumerate(paths):
        fld,leaf = os.path.split(i)
        part = fld[len(folder):]

        # check if we need to create output folder?
        outdir = out + leaf
        if outdir not in created and not os.path.isdir(outdir):
            os.makedirs(outdir)
            created[outdir] = True

        if not leaf == ".DS_Store":
            o = out + part + "/" + leaf
            # print(i, "->", o)
            # if not os.path.exists(o) or Path(o).stat().st_size == 0:
            if 1:
                args = ["./bin/apfs-recover", dev, idx, i]
                outfile = open(o, "wb")
                p = subprocess.run(args, stdout=outfile, stderr=subprocess.PIPE)
                if not p.returncode == 0:
                    recover_errors = recover_errors + 1
                    print(" ".join(args))
                outfile.close()
            if time.time() - last_ts > 2.0:
                last_ts = time.time()
                elapsed = last_ts - start_ts
                if elapsed > 0.0:
                    rate = progress / elapsed
                    if rate > 0:
                        remaining_files = len(paths) - progress
                        remaining_time = remaining_files / rate
                        hrs = int(remaining_time / 3600)
                        remaining_mins = remaining_time - (hrs * 3600)
                        min = int(remaining_mins / 60)
                        sec = int(remaining_mins) % 60
                        # print(remaining_files, remaining_time, rate, hrs, min, sec)
                        print("Exporting", progress, "of", len(paths), "(%.2f%%)" % (progress * 100.0 / len(paths)), "files...", "("+str(recover_errors), "errors)", "%ih%im%is @ %.1f" % (hrs, min, sec, rate))

