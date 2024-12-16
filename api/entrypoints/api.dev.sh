#!/bin/sh
set -e

export LD_LIBRARY_PATH=/libvol/lib:$LD_LIBRARY_PATH

cd /api/src
/api/venv/bin/flask --app app.py --debug run --host=0.0.0.0
