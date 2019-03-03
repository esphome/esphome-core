#!/usr/bin/env bash

set -e

if git diff-index --quiet HEAD --; then
    echo "No changes detected, formatting is correct!"
    exit 0
else
    echo "========================================================="
    echo "Your formatting is not correct, ESPHome uses clang-format"
    echo "The following files are outdated:"
    git diff HEAD --name-only | sed 's/^/  /'
    echo "To format them, please apply these changes:"
    echo "========================================================="
    echo
    git diff HEAD
    exit 1
fi
