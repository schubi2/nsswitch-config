## nsswitch-config

### Building nsswitch-config

#### Prerequires:
* meson
* libeconf

#### Build:
```
# meson  [<MESON-OPTIONS>] <builddir>
# cd <builddir>
# meson compile
```

##MESON-OPTIONS:

-Dvendordir=<DIR>	Directory for distribution provided configuration files
			(e.g. "/usr/etc")
			
#### Installation:
```
# meson install
```

