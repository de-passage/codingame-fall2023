#!/usr/bin/env bash


PLAYER_1=${PLAYER_1:-build/final_program}
PLAYER_2=${PLAYER_2:-ais/basic}
OUTPUT_FILE=${OUTPUT_FILE:-output.json}

function err() {
  if [[ -t 2 ]]; then
    echo "$(tput setaf 1; tput bold)${*}$(tput sgr0)" >&2
  else
    echo "$*"
  fi
}

declare -a args

while (( $# > 0 )); do
  case "$1" in
    --seed)
      if [[ -z "$2" ]]; then
        err "$1 expects a seed as parameter"
      fi
      SEED="$2"
      shift
      ;;
    --p1)
      if [[ -x "$2" ]]; then
        PLAYER_1="$2"
      else
        err "$1 expects an executable file as parameter"
      fi
      shift
      ;;
    --p2)
      if [[ -x "$2" ]]; then
        PLAYER_2="$2"
        shift
      else
        err "$1 expects an executable file as parameter"
      fi
      ;;
    --output)
      if [[ -z "$2" ]]; then
        err "$1 expects a file name as parameter"
      fi
      OUTPUT_FILE="$2"
      shift
      ;;
    --)
      shift
      args+=("$@")
      break
      ;;
    *) ;;
  esac
  shift
done

if [[ -n "$args" ]]; then
  PLAYER_1+=" ${args[@]}"
fi

cmd="java --add-opens java.base/java.lang=ALL-UNNAMED -jar '$HOME/workspace/codingame-fall2023/ais/referee.jar' -p1 '${PLAYER_1}' -p2 '${PLAYER_2}' -l '${OUTPUT_FILE}'"
if [[ -n "$SEED" ]]; then
  cmd+=" -d 'seed=${SEED}'"
fi
eval "${cmd}"
echo "Output written to ${OUTPUT_FILE}"
