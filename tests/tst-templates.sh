#!/bin/bash

# Generate nsswitch.conf from delivered templates in /usr/share/nsswitch.conf.d:
#  10-header.conf
#  20-files.conf
#  30-compat.conf
#  40-systemd.conf
#  50-dns.conf
#  60-usrfiles.conf

nsswitch_config_exe="$PWD/../nsswitch-config"

if ! [[ -f "$nsswitch-config_exe" ]]; then
    nsswitch_config_exe="$PWD/nsswitch-config"
    if ! [[ -f "$nsswitch_config_exe" ]]; then
        echo "Couldn't find the nsswitch-config executable on $PWD"
        exit 1
    fi
fi
nsswitch_etc="$PWD/../../tests/templates/etc"
if ! [[ -d "$nsswitch_etc" ]]; then
    nsswitch_etc="$PWD/../tests/templates/etc"
    if ! [[ -d "$nsswitch_etc" ]]; then
        echo "Couldn't find $nsswitch_etc"
        exit 1
    fi
fi

nsswitch_usr="$PWD/../../data/usr/share"
if ! [[ -d "$nsswitch_usr" ]]; then
    nsswitch_usr="$PWD/../data/usr/share"
    if ! [[ -d "$nsswitch_usr" ]]; then
        echo "Couldn't find $nsswitch_usr"
        exit 1
    fi
fi

echo $nsswitch_config_exe --vendordir=$nsswitch_usr --etcdir=$nsswitch_etc --output=$nsswitch_etc/nsswitch.conf
$nsswitch_config_exe --vendordir=$nsswitch_usr --etcdir=$nsswitch_etc --output=$nsswitch_etc/nsswitch.conf

file1=$nsswitch_etc/nsswitch.conf
file2=$nsswitch_etc/nsswitch.cmp
if cmp -s "$file1" "$file2"; then
    printf 'The file "%s" is the same as "%s"\n' "$file1" "$file2"
    rm $file1
    exit 0
else
    printf 'The file "%s" is different from "%s"\n' "$file1" "$file2"
    exit 1
fi


exit 0

