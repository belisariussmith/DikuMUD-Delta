/**************************************************************************
*  File: DelPlay.c                                        Part of DikuMud *
*  Usage: A tool for deleting a player from a DikuMUD player file.        *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include "structs.h"

#define TOLOWER(c)  (((c)>='A'  && (c) <= 'Z') ? ((c)+('a'-'A')) : (c))

// Local functions

int str_cmp(char *str1, char *str2)
{
    for ( ; *str1 || *str2; str1++, str2++)
    {
        if (TOLOWER(*str1) != TOLOWER(*str2))
        {
            return(1);
        }
    }

    return(0);
}

void del(char *filename, int name)
{
    struct char_file_u player;
    FILE *fl;
    char confirm[80];
    long end;
    int pos, num;

    if (!(fl = fopen(filename, "r+")))
    {
        perror("list");
        exit(1);
    }

    puts("Searching for player:");

    for (num = 1, pos = 0;; pos++, num++)
    {
        if (!fread(&player, sizeof(player), 1, fl))
        {
            fprintf(stderr, "delplay: could not locate %d.\n", name);
            exit(1);
        }

        if (feof(fl))
        {
            fprintf(stderr, "delplay: could not locate %d.\n", name);
            exit(1);
        }

        if (num == name)
        {
            printf("Confirm deletion of [%s] by typing Yes: ", player.name);
            if (!scanf("%s", confirm))
            {
                printf("Reading user input failed.\n");
                exit(1);
            }

            if (str_cmp("Yes", confirm))
            {
                printf("Aborted delete.\n");
                exit(1);
            }
            else
            {
                break;
            }
        }

    }

    // read the last player
    fseek(fl,      -sizeof(player), 2);
    if (!fread(&player,  sizeof(player), 1, fl))
    {
        printf("Failure to read file. Possibly corrupted.\n");
        exit(1);
    }
    fseek(fl,   pos*sizeof(player), 0);
    fwrite(&player, sizeof(player), 1, fl);
    fseek(fl, 0, 2);
    end = ftell(fl);
    fclose(fl);

    if (truncate(filename, end-sizeof(player)))
    {
        perror("truncate");
    }
}

// Delete player program
int main(int argc, char **argv)
{
    if (argc != 3)
    {
        puts("Usage: delplay <DikuMUD player filename> <Player Number>");
    }
    else
    {
        if (atoi(argv[2]) < 1)
        {
            puts("Illegal player number, must be >= 1");
        }
        else
        {
            del(argv[1], atoi(argv[2]));
        }
    }
}
