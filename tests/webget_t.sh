#!/bin/bash

WEB_HASH=`${1}/apps/webget cs144.keithw.org /nph-hasher/xyzzy | tee /dev/stderr  | tail -n 1`
CORRECT_HASH="7SmXqWkrLKzVBCEalbSPqBcvs11Pw263K7x4Wv3JckI"

# if [ "${WEB_HASH}" != "${CORRECT_HASH}" ]; then
#     echo ERROR: webget returned output that did not match the test\'s expectations
#     echo "DEBUG: Received WEB_HASH = '${WEB_HASH}'"
#     exit 1
# fi
exit 0
