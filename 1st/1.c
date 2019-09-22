#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <unistd.h>

#include <sys/wait.h>


//just smth new
    int
    main(int argc, char *argv[])
    {
        int base;
        char *endptr, *str;
        long val;

        if (argc < 2) {
            fprintf(stderr, "Usage: %s str [base]\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        str = argv[1];
        base = (argc > 2) ? atoi(argv[2]) : 10;

        errno = 0;    /* To distinguish success/failure after call */
        val = strtol(str, &endptr, base);


        /* Check for various possible errors */
        if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
                || (errno != 0 && val == 0)) {
            perror("strtol");
            exit(EXIT_FAILURE);
        }

        if (endptr == str) {
            fprintf(stderr, "No digits were found\n");
            exit(EXIT_FAILURE);
        }

        /* If we got here, strtol() successfully parsed a number */

    //val

    int i;
    if (val == 0)
        return 0;
    pid_t ID = fork();
    if (ID == 0)
        {
            printf("%d - ID = %d\n", 0, getpid());
        }
    if (ID < 0)
    {
        printf("%d - ERROR\n", 0);
        return 0;
    }

    for (i = 1; i < val; i++)
    {
        if (ID == 0)
            ID = fork();

        if (ID == 0)
        {
            printf("%d - ID = %d\n", i, getpid());
        }
        if (ID < 0)
        {
            printf("%d - ERROR\n", i);
        }

        int status;
        wait(&status);
    }


        exit(EXIT_SUCCESS);
    }
