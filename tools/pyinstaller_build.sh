#!/usr/bin/env bash

DISTPATH="build/dist"
OPTS="--distpath $DISTPATH --noconfirm"
SPECFILE="hpr-sim.spec"

pyinstaller $OPTS $SPECFILE
cp -rfv config build/dist
