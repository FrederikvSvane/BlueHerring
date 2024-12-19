#!/bin/bash
set -e
./build.sh
rm random.txt
while true
do
    ./BlueHerring -H random.txt -m output_example.txt
done
