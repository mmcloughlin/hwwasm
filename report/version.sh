#!/usr/bin/env bash

version=$(git rev-parse HEAD)
echo "\\newcommand{\\gitversion}{${version}}"
