#!/bin/bash
set -e

~/dev/scripts/backup.rb \
    --name "pico-os" \
    --url "git@github.com:asynts/pico-os" \
    --upload "s3://backup.asynts.com/git/pico-os"
