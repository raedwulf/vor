#!/bin/sh

echo "Getting enwik8... "
curl http://cs.fit.edu/~mmahoney/compression/enwik8.bz2 | bzip2 -d > enwik8
echo "Getting enwik9... "
curl http://cs.fit.edu/~mmahoney/compression/enwik9.bz2 | bzip2 -d > enwik9
