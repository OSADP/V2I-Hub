#!/bin/sh

tar -xf asn_j2735_r41.tar
cd asn_j2735_r41
cmake .
make
sudo make install
cd ..
rm -rf asn_j2735_r41
