#!/usr/bin/env bash

output_file="output.json"
print_stderr=false
print_scores=false
paging=auto
decorations=auto

err() {
  if [[ -t 2 ]]; then
    echo "$(tput bold;tput setaf 1)$@$(tput sgr0)" >&2
  else
    echo "$@" >&2
  fi
}

while (( $# > 0 )); do
  case "$1" in
    --output) output_file=$2; shift;;
    --stderr) print_stderr=true; shift;;
    --scores) print_scores=true; shift;;
    *) err "Unknown argument: $1"; exit 1;;
  esac
  shift
done

jq_cmd='.'

if [[ "$print_stderr" == true ]]; then
  jq_cmd='.errors.["0"] | map(select(.!=null)) | to_entries | map("Turn: " + (.key+1 | tostring) + ":\n" + .value) | .[]'
  decorations=never
  paging=auto
elif [[ "$print_scores" == true ]]; then
  jq_cmd='.scores as $scores | .agents | map({ name : .name, score : ($scores.[.index | tostring]) })'
  decorations=never
  paging=never
else
  decorations=auto
  paging=always
fi

jq "$jq_cmd" "${output_file}" -r | bat --paging="$paging" --language=json --color=always --decorations="$decorations"
