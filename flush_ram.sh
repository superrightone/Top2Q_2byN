#!/bin/bash
bash -c 'sync; echo 3 > /proc/sys/vm/drop_caches'
echo "Done."
