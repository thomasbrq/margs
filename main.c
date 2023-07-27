#define MARGS_IMPLEMENTATION
#include "margs.h"

void    parser(char c, char *arg) {
    switch (c)
    {
    case 'n':
        printf("your nickname is now: %s\n", arg);
        break;
    
    case 'a':
        printf("your avatar is now: %s\n", arg);
        break;

    default:
        printf("option not recognized\n");
        break;
    }
}

int main(int argc, char **argv) {
    start_margs(argc, argv);

    add_option('n', "name", MARGS_REQUIRED_ARGUMENT);
    add_option('a', "avatar", MARGS_OPTIONAL_ARGUMENT);

    char *arg = NULL;
    signed char r = 0;
    while ( (r = get_next_option(&arg)) >= 0 ) {
        parser(r, arg);
    }

    end_margs();

    return 0;
};