#!/bin/bash
for i in {1..1000}; do
    ./bin/host-miner "$i" 10 100000
done