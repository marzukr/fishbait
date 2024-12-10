#!/bin/sh

while [ ! -f /libvol/done ]; do
  echo "Waiting for AI shared library..."
  sleep 2
done

rm /libvol/done
export LD_LIBRARY_PATH=/libvol/lib:$LD_LIBRARY_PATH

cd /api/src
/api/venv/bin/ddtrace-run /api/venv/bin/gunicorn --log-level=info -b 0.0.0.0:5000 app:app
