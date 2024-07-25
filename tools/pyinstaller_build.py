#!/usr/bin/env bash

NAME="hpr-sim"
WORKPATH="build/pyinstaller"
DISTPATH="build/pyinstaller/dist"
OPTS="--workpath $WORKPATH --distpath $DISTPATH --noconfirm"
SPECFILE="$NAME.spec"

pyinstaller $OPTS $SPECFILE
cp -rfv input $DISTPATH/$NAME
mkdir -p $DISTPATH/$NAME/output
