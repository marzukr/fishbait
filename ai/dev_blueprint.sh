#!/bin/bash
set -e

mkdir -p /build/out/ai/clustering
cd /build/out/ai/clustering
wget -nc https://fishbait-clusters.s3.us-east-1.amazonaws.com/flop_clusters.cereal
wget -nc https://fishbait-clusters.s3.us-east-1.amazonaws.com/turn_clusters.cereal
wget -nc https://fishbait-clusters.s3.us-east-1.amazonaws.com/river_clusters.cereal
cd /build
./bin/blueprint.out
./bin/avg_to_hd5.out out/ai/mccfr/dev/average_final.cereal out/ai/mccfr/dev/blueprint_dev.hdf
