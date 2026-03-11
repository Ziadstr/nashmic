#!/bin/bash
set -e

VERSION="${1:?Usage: build-deb.sh VERSION ARCH}"
ARCH="${2:-amd64}"
PKG="nashmic_${VERSION}_${ARCH}"

mkdir -p "$PKG/DEBIAN"
mkdir -p "$PKG/usr/bin"
mkdir -p "$PKG/usr/share/nashmic/runtime"
mkdir -p "$PKG/usr/share/nashmic/stdlib"

sed "s/VERSION/$VERSION/; s/ARCH/$ARCH/" packaging/deb/DEBIAN/control.template > "$PKG/DEBIAN/control"

cp build/mansaf "$PKG/usr/bin/"
cp runtime/nsh_runtime.c runtime/nsh_runtime.h "$PKG/usr/share/nashmic/runtime/"
cp -r stdlib/* "$PKG/usr/share/nashmic/stdlib/"

dpkg-deb --build "$PKG"
echo "Built $PKG.deb"
