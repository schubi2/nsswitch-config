## nsswitch-config

Tool which merges different nsswitch configuration file snippets into one single
nsswitch.conf file, which is normally located in the /etc directory.

The program parses different configurations files with the order defined by the
UAPI group:
https://github.com/uapi-group/specifications/blob/main/specs/configuration_files_specification.md
and combines the parsed values to one single file.

### Example

Files will be managed in following order:

* /usr/share/nsswitch.conf

  Content:
  ```
  # comment for /usr/share/nsswitch.conf
  hosts: files
  ```

* /usr/share/nsswitch.conf.d/10-mdns.conf

  Content:
  ```
  # comment for /usr/share/nsswitch.conf.d/10-mdns.conf
  hosts: mdns_minimal [NOTFOUND=return]
  ```

* /etc/nsswitch.conf.d/20-compat.conf

  Content:
  ```
  # comment for /etc/nsswitch.conf.d/20-compat.conf
  passwd:         compat
  group:          compat
  shadow:         compat
  ```

* /usr/share/nsswitch.conf.d/30-dns.conf

  Content:
  ```
  # comment for /usr/share/nsswitch.conf.d/30-dns.conf
  hosts: dns
  ```

* /etc/nsswitch.conf.d/40-dns.conf

  Content:
  ```
  # comment for etc/nsswitch.conf.d/40-dns.conf (no double entry)
  hosts: dns
  ```
  
Generates /etc/nsswitch.conf with following content:

```
# comment for /usr/share/nsswitch.conf
# comment for /usr/share/nsswitch.conf.d/10-mdns.conf
# comment for /usr/share/nsswitch.conf.d/30-dns.conf
# comment for etc/nsswitch.conf.d/40-dns.conf (no double entry)
hosts:files mdns_minimal [NOTFOUND=return] dns
# comment nr.1 for /etc/nsswitch.conf.d/20-compat.conf
# comment nr.2 for /etc/nsswitch.conf.d/20-compat.conf
passwd:compat
group:compat
shadow:compat
```

Before writing /etc/nsswitch.conf a copy will be generated:
/etc/nsswitch.conf.nsswitch-config-sav

If the list of files, which have to be parsed, contains the output file
e.g. /etc/nsswitch.conf, the output file will NOT be parsed/merged.


### general OPTIONS

--vendordir \<string\> :
  Directory for vendor specific configuration files.
  default: /usr/share

--etcdir \<string\> :
  Directory for user changed configuration files (default: /etc).

--output \<string\> :
  Path of generated nsswitch.conf file (default: /etc/nsswitch.conf).

--verbose :
  Generates additional information.

### Building nsswitch-config

#### Prerequires:
* meson
* libeconf

#### Build:
```
# meson  <builddir>
# cd <builddir>
# meson compile
```

#### Installation:
```
# cd <builddir>
# meson install
```

#### Test:
```
# cd <builddir>
# meson test
```

