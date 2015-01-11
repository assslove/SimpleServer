#!/bin/bash

#generate protobuf src

cd proto/def
ls | xargs protoc -I=./ --cpp_out=../../proto/lib 
cd ../../
