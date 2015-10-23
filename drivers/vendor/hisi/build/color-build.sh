#!/bin/bash -e

ccred=$(echo -e "\033[0;31m")
ccyellow=$(echo -e "\033[0;33m")
ccend=$(echo -e "\033[0m")
./obuild.sh "$@" 2>&1 | sed -E  -e "/[Ww][Aa][Rr][Nn][Ii][Nn][Gg][: ]/ s/^/$ccyellow&/g" -e "/[Ww]arning[: ]/ s/$/&$ccend/g" -e "/[Ee][Rr][Rr][Oo][Rr][: ]/ s/^/$ccred&/g" -e "/[Ee][Rr][Rr][Oo][Rr][: ]/ s/$/&$ccend/g"

