#!/bin/bash

nsswitch_config_exe="$PWD/../nsswitch-config"

if ! [[ -f "$nsswitch-config_exe" ]]; then
    nsswitch_config_exe="$PWD/nsswitch-config"
    if ! [[ -f "$nsswitch_config_exe" ]]; then
        echo "Couldn't find the nsswitch-config executable on $PWD"
        exit 1
    fi
fi
nsswitch_config_root="$PWD/../../tests/test1"
if ! [[ -d "$nsswitch_config_root" ]]; then
    nsswitch_config_root="$PWD/../tests/test1"
    if ! [[ -d "$nsswitch_config_root" ]]; then
        echo "Couldn't find $nsswitch_onfig_root"
        exit 1
    fi
fi

echo $nsswitch_config_exe --etcdir=$nsswitch_config_root/etc --output=$nsswitch_config_root/nsswitch.cmp
$nsswitch_config_exe --etcdir=$nsswitch_config_root/etc --output=$nsswitch_config_root/nsswitch.cmp

file1=$nsswitch_config_root/nsswitch.cmp
file2=$nsswitch_config_root/nsswitch.out
if cmp -s "$file1" "$file2"; then
    printf 'The file "%s" is the same as "%s"\n' "$file1" "$file2"
    rm $file1
    exit 0
else
    printf 'The file "%s" is different from "%s"\n' "$file1" "$file2"
    exit 1
fi
