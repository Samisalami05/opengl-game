#!/bin/bash

cmake -B build
cmake --build build
cp build/main .
