#!/usr/bin/env bash

if [[ -f /mnt/c/Windows/System32/clip.exe ]]; then
  cat "${1}" | /mnt/c/Windows/System32/clip.exe
else
  xclip -i "${1}" -selection clipboard
fi
