#!/usr/bin/env bash

ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"

# Do not escape the -r path, it is escaped by the java runtime
java -jar "$ROOT_DIR/ais/cg-brutaltester-1.0.0-SNAPSHOT.jar" \
  -r "java --add-opens java.base/java.lang=ALL-UNNAMED -jar $ROOT_DIR/ais/referee.jar" \
  "$@"
