#!/usr/bin/env bash

start="$1"
end="$2"
input="$3"

awk "/$start/ { flag=1 } flag { print} /$end/ { flag=0 }" "${input}"
