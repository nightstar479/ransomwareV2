#!/bin/bash

gcc -Wall "$1".c -o "$1" -lcrypto
./"$1"
