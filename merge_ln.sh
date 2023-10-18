#!/bin/bash
# Merge two files or two directories by creating hard links to identical files

function merge_files {
    if cmp -s "$1" "$2"; then
        echo "Files $1 and $2 are the same, merging as hard links"
        ln -fn "$1" "$2"
    else
        echo "Files $1 and $2 are different"
    fi
}

function merge_directories {
    for file1 in "$1"/*; do
        file2="$2/$(basename "$file1")"
        if [ -f "$file1" ] && [ -f "$file2" ]; then
            merge_files "$file1" "$file2"
        elif [ -d "$file1" ] && [ -d "$file2" ]; then
            merge_directories "$file1" "$file2"
        fi
    done
}

if [ -f "$1" ] && [ -f "$2" ]; then
    merge_files "$1" "$2"
elif [ -d "$1" ] && [ -d "$2" ]; then
    merge_directories "$1" "$2"
else
    echo "Usage: merge_ln file1 file2 OR merge_ln dir1 dir2"  
fi
