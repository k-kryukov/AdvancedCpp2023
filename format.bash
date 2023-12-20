#!/bin/env bash

clang-format-14 --style=file -i $(find ./client -name *.cpp)
clang-format-14 --style=file -i $(find ./server -name *.cpp)