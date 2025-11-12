#!/bin/bash

# option --force overwrites admin changed nsswitch.conf

nsswitch_config_exe="$PWD/../nsswitch-config"

if ! [[ -f "$nsswitch-config_exe" ]]; then
    nsswitch_config_exe="$PWD/nsswitch-config"
    if ! [[ -f "$nsswitch_config_exe" ]]; then
        echo "Couldn't find the nsswitch-config executable on $PWD"
        exit 1
    fi
fi
nsswitch_config_root="$PWD/../../tests/test7"
if ! [[ -d "$nsswitch_config_root" ]]; then
    nsswitch_config_root="$PWD/../tests/test7"
    if ! [[ -d "$nsswitch_config_root" ]]; then
        echo "Couldn't find $nsswitch_onfig_root"
        exit 1
    fi
fi

echo $nsswitch_config_exe --vendordir=$nsswitch_config_root/usr/share --etcdir=$nsswitch_config_root/etc --output=$nsswitch_config_root/etc/nsswitch.conf
$nsswitch_config_exe --vendordir=$nsswitch_config_root/usr/share --etcdir=$nsswitch_config_root/etc --output=$nsswitch_config_root/etc/nsswitch.conf --force

file1=$nsswitch_config_root/etc/nsswitch.conf.rpmnew
if [[ -f $file1 ]]; then
    printf 'The file "%s" should not exists\n' "$file1"
fi    

file1=$nsswitch_config_root/etc/nsswitch.conf
file2=$nsswitch_config_root/etc/nsswitch.cmp
if cmp -s "$file1" "$file2"; then
    printf 'The file "%s" is the same as "%s"\n' "$file1" "$file2"
    git checkout "$file1"
else
    printf 'The file "%s" is different from "%s"\n' "$file1" "$file2"    
    exit 1
fi

exit 0

