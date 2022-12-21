#!/bin/bash

gcc -w "$1".c -o "$1" -lm -lcrypto
./"$1" "$2"
