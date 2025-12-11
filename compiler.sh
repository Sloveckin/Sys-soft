#!/bin/bash

./main $1 &&
gcc $1.s -o $1.out