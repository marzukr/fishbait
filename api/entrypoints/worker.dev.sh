#!/bin/sh
set -e

export LD_LIBRARY_PATH=/libvol/lib:$LD_LIBRARY_PATH

cd /api/src
/api/venv/bin/celery -A tasks worker --loglevel=INFO
