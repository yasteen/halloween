#!/bin/zsh
find -name "*.[ch]pp" -type f | entr -rc ./build.sh
