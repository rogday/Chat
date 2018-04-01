#!/bin/bash
rm -rf ./Server/server ./Client/client
./Server/build.sh
./Client/build.sh
