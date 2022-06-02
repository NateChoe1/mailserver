#!/bin/sh

# Unfortunately, git doesn't allow you to create empty directories.

for dir in config mail-data mail-logs mail-state ; do
	mkdir -p docker-data/dms/$dir
done
