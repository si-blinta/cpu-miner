#!/bin/bash
for i in {1..24}; do
    ./bin/host-miner 1 "$i" 20000
done