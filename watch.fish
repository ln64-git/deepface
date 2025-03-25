#!/usr/bin/env fish

echo "👀 Watching for changes..."

watchexec -e cpp,h --restart --clear -- ./build.sh
