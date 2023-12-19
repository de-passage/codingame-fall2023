#!/usr/bin/env bash
#
# Combine C/C++ files and their dependencies into a single file. 
# I use this when working on single-file solutions to online problems (codewars, codingame...) 
#
# Usage: 
#  combine.bash [-I./path/to/includes] [-o output.cpp] FILE...

unset source_file
declare target_file="result.cpp"
declare -a include_paths=()
declare debug_mode=false

function err() {
  if [[ -t 2 ]]; then
    echo "$(tput setaf 1)$(tput bold)${*}$(tput sgr0)" >&2
  else
    echo "$@" >&2
  fi
  exit 1
}

function debug() {
  if [[ "$debug_mode" == true ]]; then
    echo "$@" >&2
  fi
}

# Return the path to the file given in argument.
# Goes through the list of include directories to find a match
function find_file_path() {
  target="$1"

  target_dir="$(dirname "$target")"
  target_base="$(basename "$target")"

  if [[ -f "${target_dir}/${target_base}" ]]; then
    echo "${target_dir}/${target_base}"
    return
  else
    for path in "${include_paths[@]}"; do
      debug "Looking in ${path}"
      if [[ -f "${path}/${target_dir}/${target_base}" ]]; then
        echo "${path}/${target_dir}/${target_base}"
        return
      fi
    done
  fi
  err "Missing input file '$target'"
}

while (( $# > 0 )); do
  case "$1" in
    --output)
      if [[ -z "$2" ]]; then
        err ""
      fi
      target_file="$2"
      shift
      ;;
    --*)
      err "Unhandled option '$1'"
      ;;
    -*)
      for (( i = 1; i < ${#1}; i++ )); do
        option="${1:$i:1}"
        case "$option" in
          I)
            if [[ -n "${1:$i+1}" ]]; then
              include_paths+=( "${1:$i+1}" )
              break
            elif [[ -n "$2" ]]; then
              include_paths+=( "$2" )
              shift
              break
            else
              err "Missing parameter for '-${option}'"
            fi
            ;;
          o)
            if [[ -n "${1:$i+1}" ]]; then
              target_file="${1:$i+1}"
              break
            elif [[ -n "$2" ]]; then
              target_file="$2"
              shift
              break
            else
              err "Missing parameter for '-${option}'"
            fi
            ;;
          *) err "Unhandled short form option '-${option}'";;
        esac
      done
      ;;
    *)
      source_file+=("$1")
      ;;
  esac
  shift
done

if (( ${#source_file[@]} == 0 )); then
  err "Need at least one source file"
fi

temp_file=$(mktemp)
trap 'rm "$temp_file"' EXIT

cat "${source_file[@]}" > "$temp_file"

declare -A already_included=()

while true; do
  includes="$(grep -m 1 '^\s*#include\s\+"[^""]*"' "$temp_file" | sed 's/[^"]*"\([^"]*\)"/\1/')"
  if [[ -z "$includes" ]]; then
    break
  fi
  while IFS= read -r line; do
    filename="$(find_file_path "$line")"

    if [[ -n "${already_included[$filename]}" ]]; then
      debug "Already included: $filename"
      sed '/^\s*#include\s\+"'"${line/\//\/\/}"'"/d' -i "$temp_file"
    else
      already_included[$filename]=included
      debug "Splicing in '$filename'"
      sed '/^\s*#include\s\+"'"${line/\//\/\/}"'"/ { r'"${filename}"'
      :p;n;bp }' "$temp_file" -i
    fi
  done <<< "$includes"
  debug "includes:$includes"
done

# Cleanup
remaining_includes=$(sed -n '/^\s*#include\s\+<\([^>]*\)>/p' "$temp_file" | sort | uniq)
sed '/^\s*#include\s\+<\([^>]*\)>/d;/^\s*#pragma\s\+once/d' -i "$temp_file"
echo "$remaining_includes" > "$target_file"
cat "$temp_file" >> "$target_file"