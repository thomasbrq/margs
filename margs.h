#ifndef __MARGS__
#define __MARGS__

#ifndef MARGS_MALLOC
	#include <stdlib.h>
	#define MARGS_MALLOC malloc
#endif

#include <getopt.h>
extern char *optarg;
extern int optind, opterr, optopt;

#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_OPT 1024

typedef struct {
	int             argc;
	char            **argv;
	char			*short_opts;
	struct option   opts[MAX_OPT + 1];
	int             opts_len;
} Margs;

enum {
	MARGS_NO_ARGUMENT,
	MARGS_REQUIRED_ARGUMENT,
	MARGS_OPTIONAL_ARGUMENT,
};

Margs   margs_ll__;

/**
 * Prototypes
*/
void    start_margs(int argc, char **argv);
void	end_margs();

void    add_option(char short_opt, char *long_opt, int type);
char	get_next_option(char **arg);

#endif // __MARGS__

/**
 * C Implementation
*/
#ifdef MARGS_IMPLEMENTATION

size_t	__get_short_options_lenght__() {
	size_t	length = 0;

	for (int i = 0; i < margs_ll__.opts_len; i++)
	{
		struct option o = margs_ll__.opts[i];
		length += (o.has_arg + 1);
	}
	
	return length;
}

void	__format_short_options__() {
	size_t	max = __get_short_options_lenght__();

	char	*ret = (char *) MARGS_MALLOC((max + 1) * sizeof(char));
	if (!ret) {
		_exit(-1);
	}

	ret[max] = '\0';

	int index = 0;
	for (int i = 0; i < margs_ll__.opts_len; i++)
	{
		struct option o = margs_ll__.opts[i];
		ret[index] = o.val;
		for (int j = 0; j < o.has_arg; j++) 
			ret[++index] = ':';
		index++;
	}

	margs_ll__.short_opts = ret;
}

void    start_margs(int argc, char **argv)
{
	margs_ll__.opts_len = 0;
	margs_ll__.argc = argc;
	margs_ll__.argv = argv;
	margs_ll__.short_opts = NULL;
	bzero(margs_ll__.opts, sizeof(margs_ll__.opts));

	add_option('h', "help", MARGS_NO_ARGUMENT);
}

int	__is_option_already_added__(char short_opt, char *long_opt) {
	for (int i = 0; i < margs_ll__.opts_len; i++)
	{
		struct option o = margs_ll__.opts[i];
		if ( o.val == short_opt || strcmp(long_opt, o.name) == 0 ) {
			return 1;
		}
	}
	return 0;
}

void    add_option(char short_opt, char *long_opt, int type)
{
	if (margs_ll__.opts_len >= MAX_OPT) {
		write(STDERR_FILENO, "error: too many options\n", strlen("error: too many options\n"));
		end_margs();
		exit(EXIT_FAILURE);
	}

	if ( __is_option_already_added__(short_opt, long_opt) ) {
		write(STDERR_FILENO, "error: duplicated option\n", strlen("error: duplicated option\n"));
		end_margs();
		exit(EXIT_FAILURE);
	}

	struct option *opt = &(margs_ll__.opts[margs_ll__.opts_len]);

	opt->name       = long_opt;
	opt->val        = short_opt;
	opt->has_arg    = type;
	opt->flag       = 0;
	
	++margs_ll__.opts_len;
}

void	__print_help__() {
	printf("usage: %s [option] (argument)\n", margs_ll__.argv[0]);
	for (int i = 0; i < margs_ll__.opts_len; i++)
	{
		struct option o = margs_ll__.opts[i];
		printf("-%c --%s ", o.val, o.name);
		if ( o.has_arg == MARGS_OPTIONAL_ARGUMENT ) printf("(%s)\n", o.name);
		else if ( o.has_arg == MARGS_REQUIRED_ARGUMENT ) printf("<%s>\n", o.name);
		else printf("\n");
	}
}

char	get_next_option(char **arg) {
	if ( !margs_ll__.short_opts ) {
		__format_short_options__();
	}

	int		option_index = 0;
	int		argc 	= margs_ll__.argc;
	char	**argv	= margs_ll__.argv;

	int c = '?';
	if ( ( c = getopt_long(argc, argv, margs_ll__.short_opts, margs_ll__.opts, &option_index) ) == '?')
	{
		end_margs();
		exit(EXIT_FAILURE);
	}

	if ( optarg && strcmp(optarg, "-") == 0 ) {
		write(STDERR_FILENO, "parsing error\n", strlen("parsing error\n"));
		end_margs();
		exit(EXIT_FAILURE);
	}

	if ( c == 'h' ) {
		__print_help__();
		end_margs();
		exit(EXIT_SUCCESS);
	}

	// retrieve the argument in 'arg' if the argument is optional
	if ( optarg == NULL && argv[optind] != NULL && argv[optind][0] != '-' ) {
		*arg = argv[optind];
		++optind;
	} else {
		*arg = optarg;
	}

	// removes '=' from argument when -o'='something
	if ( *arg && strncmp("=", *arg, 1) == 0 ) {
		memmove(*arg, *arg + 1, strlen(*arg));
	}

	return c;
}

void end_margs() {
	if (margs_ll__.short_opts) {
		free(margs_ll__.short_opts);
		margs_ll__.short_opts = NULL;
	}
}

#endif // __MARGS_IMPLEMENTATION__