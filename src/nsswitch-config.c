/*
  Copyright (C) 2025 SUSE LLC

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#include "libeconf.h"
#include "libeconf_ext.h"

#define UNUSED(x) (void)(x)
#define PROGRAM_NAME "nsswitch-config"
#define COMMENT "#"
#define DELIMITERS ":"

static char *output_file = "/etc/nsswitch.conf"; /* output of all merged files */
static char *etc_dir = "/etc"; /* output of all merged files */
#ifdef VENDORDIR
static char *vendor_dir = VENDORDIR
#else
static char *vendor_dir = NULL;
#endif


/**
 * @brief Shows the usage.
 */
static void usage(void)
{
    fprintf(stderr, "Usage: %s [OPTIONS]\n\n", PROGRAM_NAME);
    fprintf(stderr, "\ngeneral Options:\n");
    fprintf(stderr, "--vendordir <string>: Directory for vendor specific configuration files (e.g. /usr/lib).\n");
#ifdef VENDORDIR
    fprintf(stderr, "                      default: %s\n", VENDORDIR);
#else
    fprintf(stderr, "                      default: -none-\n");
#endif
    fprintf(stderr, "--etcdir <string>:    Directory for user changed configuration files (default: /etc).\n");    
    fprintf(stderr, "--output <string>:    Path of generated nsswitch.conf file (default: /etc/nsswitch.conf).\n");
    fprintf(stderr, "--verbose:            Generates additional information in output file.\n");
}

/**
 * @brief printing error with linenr and filename
 */
static void print_error(const econf_err error)
{
  char *filename = NULL;
  uint64_t line_nr = 0;

  econf_errLocation( &filename, &line_nr);
  fprintf(stderr, "%s (line %d): %s\n", filename, (int) line_nr,
	  econf_errString(error));
  free(filename);
}

int main (int argc, char *argv[])
{
    econf_file *output_key_file = NULL;
    econf_file **key_file_list = NULL;
    size_t list_length = 0;
    econf_err econf_error = ECONF_SUCCESS;
    int ret = 0;
    int verbose = 0;

    /* parse command line arguments. See getopt_long(3) */
    int opt;
    int index = 0;
    static struct option longopts[] = {
    /*   name,     arguments,      flag, value */
        {"help",        no_argument,       0, 'h'},
	{"vendordir",   required_argument, 0, 'v'},
	{"etcdir",      required_argument, 0, 'e'},
	{"output",      required_argument, 0, 'o'},
        {"verbose",     no_argument,       0, 'l'}, 
        {0,             0,                 0,  0 }
    };

    while ((opt = getopt_long(argc, argv, "hvo:", longopts, &index)) != -1) {
        switch(opt) {
        case 'v':
	    vendor_dir = optarg;
            break;
        case 'e':
	    etc_dir = optarg;
            break;	    
        case 'o':
	    output_file = optarg;
            break;
        case 'l':
            verbose = 1;
            break;	    
        case 'h':
            usage();
            return EXIT_SUCCESS;
        case '?':
        default:
            fprintf(stderr, "Try '%s --help' for more information.\n", PROGRAM_NAME);
            exit(EXIT_FAILURE);
            break;
        }
    }

    /* Saving old file */
    if (access(output_file, F_OK) == 0) {
        char *dest = NULL;
	if (asprintf(&dest, "%s.nsswitch-config-sav", output_file) <0) {
	   ret = ECONF_NOMEM;
	   print_error(econf_error);
	   return -1;
	}
	if (access(dest, F_OK) == 0)
 	   remove(dest);

	if (rename(output_file, dest) != 0) {
  	   fprintf(stderr, "Cannot move %s to %s\n", output_file, dest);
	   return -1;
	}
	free(dest);	
    }

    econf_error = econf_newKeyFile( &output_key_file,
				    ':', /* DELIMITERS */
				    '#'  /* COMMENT */);
    if (econf_error) {
	print_error(econf_error);
        return -1;
    }    

    econf_error = econf_readDirsHistory( &key_file_list,
					 &list_length,
					 vendor_dir,
					 etc_dir,
					 "nsswitch",
				         "conf",
					 DELIMITERS,
					 COMMENT);
    if (econf_error) {
	print_error(econf_error);
	ret = -1;
	goto cleanup;	    
    }
    
    for (size_t i=0; i < list_length; i++) {
        char *path = econf_getPath(key_file_list[i]);
	char **keys = NULL;
        size_t key_count = 0;

	if (strlen(path) > 0 && verbose) {
	    fprintf(stderr, "Evaluating: %s\n\n", path);
	    free(path);	    
	}

        econf_error = econf_getKeys(key_file_list[i],
				    NULL, &key_count, &keys);
        if (econf_error) {
	    print_error(econf_error);
            econf_free(keys);
            ret = -1;
            goto cleanup;	    
        }

	for (size_t k = 0; k < key_count; k++) {
	    econf_ext_value *ext_value = NULL;
	    econf_ext_value *add_ext_value = NULL;	    
	    char *value = NULL;
	    char *add_value = NULL;

            econf_error = econf_getExtValue(key_file_list[i], NULL, keys[k], &add_ext_value);
            if (econf_error) {
		print_error(econf_error);
                econf_free(keys);
                ret = econf_error;
		goto cleanup;
            }
            econf_error = econf_getStringValue(key_file_list[i], NULL, keys[k], &add_value);
            if (econf_error) {
		print_error(econf_error);
		econf_freeExtValue(add_ext_value);
                econf_free(keys);
                ret = econf_error;
		goto cleanup;
            }	    

	    /* adding value */
            econf_error = econf_getStringValue(output_key_file, NULL, keys[k], &value);
            if (econf_error == ECONF_NOKEY) {
	        econf_error = econf_setStringValue(output_key_file, NULL, keys[k], add_value);
	    } else if (econf_error == ECONF_SUCCESS) {
	        if (strstr(value, add_value) == NULL) {
		   /* value is not in list */
		   char *new_value = NULL;
		   if (asprintf(&new_value, "%s %s", value, add_value) <0) {
		      ret = ECONF_NOMEM;
		      print_error(econf_error);
		      free(value);
		      free(add_value);
		      econf_freeExtValue(add_ext_value);
		      econf_free(keys);
		      goto cleanup;
		   }
		   econf_error = econf_setStringValue(output_key_file, NULL, keys[k], new_value);
		   free(new_value);
		}
	    }
	    free(value);
	    free(add_value);

	    /* adding comments */
	    if (econf_error == ECONF_SUCCESS &&
		add_ext_value->comment_before_key != NULL &&
		strlen(add_ext_value->comment_before_key) > 0) {
	        if (econf_getExtValue(output_key_file, NULL, keys[k], &ext_value) == ECONF_SUCCESS) {
		    if (ext_value->comment_before_key == NULL) {
			ext_value->comment_before_key = strdup(add_ext_value->comment_before_key);
		    } else {
			char *new_comment = NULL;			    
			if (asprintf(&new_comment, "%s\n%s", ext_value->comment_before_key, add_ext_value->comment_before_key) <0) {
		            ret = ECONF_NOMEM;
			    print_error(econf_error);			    
 	                    econf_freeExtValue(add_ext_value);
			    econf_freeExtValue(ext_value);
		            econf_free(keys);
		            goto cleanup;
			} else {
			    free(ext_value->comment_before_key);
			    ext_value->comment_before_key = new_comment;
			}
		    }
		    econf_error = econf_setExtValue(output_key_file, NULL, keys[k], ext_value);
		    econf_freeExtValue(ext_value);
		}
	    }
	}
	econf_free(key_file_list[i]);
    }

    econf_error = econf_writeFile(output_key_file, dirname(output_file), basename(output_file));
    if (econf_error) {
	print_error(econf_error);
        ret = -1;
    }    
    
 cleanup:
    free(key_file_list);
    econf_free(output_key_file);
    
    return ret;
}
