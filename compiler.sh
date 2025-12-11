#!/bin/bash

./main $1 -o $1.s
gcc $1.s -o $1.out