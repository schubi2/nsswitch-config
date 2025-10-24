project('nsswitch-config', 'c',
	version : '0.0.1',
	license : 'LGPLv2.1+',
	default_options : [
		'buildtype=debugoptimized',
		'default_library=shared',
		'b_pie=true',
		'warning_level=2',
        	'c_std=gnu99',
		'prefix=/usr',
		'sysconfdir=/etc',
		'werror=true',
	],
	meson_version : '>= 0.49.0',
)
man_dir = get_option('mandir')

cc = meson.get_compiler('c')
pkg = import('pkgconfig')

add_project_arguments(['-D_GNU_SOURCE=1',
		       '-DXTSTRINGDEFINES',
		       '-D_FORTIFY_SOURCE=2'], language : 'c')

possible_cc_flags = [
		  '-fstack-protector-strong',
		  '-funwind-tables',
		  '-fasynchronous-unwind-tables',
		  '-fstack-clash-protection',
		  '-Werror=return-type',
		  '-flto=8',
		  '-Wbad-function-cast',
		  '-Wcast-align',
		  '-Wcast-qual',
		  '-Winline',
		  '-Wmissing-declarations',
		  '-Wmissing-prototypes',
		  '-Wnested-externs',
		  '-Wshadow',
		  '-Wstrict-prototypes',
		  '-Wundef',
		  ]
add_project_arguments(cc.get_supported_arguments(possible_cc_flags), language : 'c')

nsswitch_config_src = ['src/nsswitch-config.c']

libeconf_dep = dependency('libeconf')

# Commandline program
executable('nsswitch-config', nsswitch_config_src, dependencies : libeconf_dep, install : true, )

# Unit tests
subdir('tests')

# documentation
subdir('doc')