/**************************************************************************
*  File: RepairGold.c                                     Part of DikuMud *
*  Usage: Used to "repair" a playerfile, and reset the maximum amount of  *
*         gold players have using the arbitrary value (25000 * player lvl)*
*                                                                         *
*                Ex. Level 10 player with 1 million gold would be reset   *
*                    to 250k gold.                                        *
*                                                                         *
*  Explanation: Perhaps if a bug accidentally provided players too much   *
*               money, or a MUD admin wanted to radically reset the       *
*               virtual economy of a MUD, then this could be used for     *
*               such a purpose.                                           *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "structs.h"

// Local functionality

void do_it(FILE *src, FILE *trg, FILE *out);

//
// RepairGold primary program
//

int main(int argc, char **argv)
{
    FILE *src;
    FILE *trg = NULL;
    FILE *out = NULL;

    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s source target ulit\n", argv[0]);
        exit(0);
    }
    else if (!(src = fopen(argv[1], "rb")))
    {
        fprintf(stderr, "%s: Could not open.\n", argv[1]);
    }
    else if (!(trg = fopen(argv[2], "rb")))
    {
        fprintf(stderr, "%s: Could not open.\n", argv[2]);
    }
    else if (!(out = fopen(argv[3], "wb")))
    {
        fprintf(stderr, "%s: Could not open.\n", argv[3]);
    }
    else
    {
        do_it(src, trg, out);
    }

    fclose(src);
    fclose(trg);
    fclose(out);
}

void do_it(FILE *src, FILE *trg, FILE *out)
{
    struct char_file_u inbuf1, inbuf2;
    int n;

    srand((int) time(0));

    n = 1;

    for (;;)
    {
        if (!fread(&inbuf1, sizeof(inbuf1), 1, trg))
        {
            printf("repairgold: error: failure in reading target file");
            break;
        }
        if (!fread(&inbuf2, sizeof(inbuf2), 1, src))
        {
            printf("repairgold: error: failure in reading source file");
            break;
        }

        while (strcmp(inbuf1.name, inbuf2.name))
        {
            if (!fread(&inbuf2, sizeof(inbuf2), 1, src))
            {
                printf("repairgold: error: failed to read source file, possible corrupted");
                break;
            }
        }

        if (feof(trg))
        {
            break;
        }

        inbuf1.points.gold = inbuf2.points.gold;

        if (inbuf1.points.gold > 25000*inbuf1.level)
        {
            inbuf1.points.gold = 25000*inbuf1.level;
        }

        fwrite(&inbuf1, sizeof(inbuf1), 1, out);
    }
}
