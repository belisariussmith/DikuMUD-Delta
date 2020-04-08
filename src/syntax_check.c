/**************************************************************************
*  file: SYNTAX_CHECKER.c , Check syntax of all files     Part of DIKUMUD *
*  Usage: QUICK AND DIRTY!!                                               *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
***************************************************************************/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "structs.h"
#include "utils.h"
#include "db.h"
#include "comm.h"
#include "handler.h"
#include "limits.h"

// *************************************************************************
// declarations of most of the 'global' variables                          *
// **********************************************************************  *

struct room_data *world;              // dyn alloc'ed array of rooms
struct obj_data  *object_list    = 0; // the global linked list of obj's
struct char_data *character_list = 0; // global l-list of chars
int top_of_world                 = 0; // ref to the top element of world

struct zone_data *zone_table;         // table of reset data
struct message_list fight_messages[MAX_MESSAGES]; // fighting messages
struct player_index_element *player_table = 0; // index to player file
int top_of_zone_table = 0;
int top_of_p_table    = 0;            // ref to top of table
int top_of_p_file     = 0;

FILE *mob_f,                          // file containing mob prototypes
     *obj_f,                          // obj prototypes
     *wld_f,                          // World file
     *zon_f;

struct index_data *mob_index;         // index table for mobile file
struct index_data *obj_index;         // index table for object file
struct index_data *wld_index;

struct help_index_element *help_index = 0;

int top_of_mobt = 0;                  // top of mobile index table
int top_of_objt = 0;                  // top of object index table
int top_of_wldt = 0;

struct time_data time_info;           // the infomation about the time
struct weather_data weather_info;     // the infomation about the weather

// Local procedures
struct index_data *generate_indices(FILE *fl, int *top);
void boot_zones(void);
void setup_dir(FILE *fl, int room, int dir);
void allocate_room(int new_top);
void boot_world(void);
void build_player_index(void);
void char_to_store(struct char_data *ch, struct char_file_u *st);
void store_to_char(struct char_file_u *st, struct char_data *ch);
void reset_zone(int zone);
void renum_world(void);
void renum_zone_table(void);
void reset_time(void);
void clear_char(struct char_data *ch);
int file_to_string(char *name, char *buf);
int is_empty(int zone_nr);

// ************************************************************************
// routines for booting the system                                        *
// *********************************************************************  *

void assume(int actual, int number, int place, char *errmsg)
{
    if (number != actual)
    {
        printf("Error has occured at #%d.\n\r", place);
        printf("Message is : %s\n\r", errmsg);
        printf("Actual number read is %d\n\r", actual);
        exit(1);
    }
}

// generate index table for object, monster or world file
struct index_data *generate_indices(FILE *fl, int *top)
{
    struct index_data *index = NULL;
    char buf[82];
    int i = 0;
    int number;

    rewind(fl);

    for (;;)
    {
        if (fgets(buf, 81, fl))
        {
            if (*buf == '#')
            {
                // allocate new cell

                if (!i)
                {
                    // first cell
                    CREATE(index, struct index_data, 1);
                }
                else
                {
                    if (!(index =
                        (struct index_data*) realloc(index,
                        (i + 1) * sizeof(struct index_data))))
                    {
                        printf("load indices");
                        exit(1);
                     }
                }

                number = sscanf(buf, "#%d", &index[i].virtual);
                assume(number, 1, index[i].virtual, "Next string with E/A/$");

                index[i].pos    = ftell(fl);
                index[i].number = index[i].virtual;
                index[i].func   = 0;
                i++;
            }
            else
            {
                if (*buf == '$')
                {
                    // EOF
                    break;
                }
            }
        }
        else
        {
            printf("Error when generating index, based upon #xxxx numbers.\n\r");
            printf("   Probably error at end of file.\n\r");

            exit(1);
        }
    }

    index[i-1].number = -1;
    *top = i - 1;
    return(index);
}

int exist_index(struct index_data *index_list, int top, int num)
{
    int i;
    int found;

    found = FALSE;

    for (i = 0; (i <= top) && !(found); i++)
    {
        if (index_list[i].number == num)
        {
            found = TRUE;
        }
    }

    if (!found)
    {
        printf("Reference to non-existent number #%d\n\r", num);
    }

    return (found);
}

// check the rooms
void check_world(FILE *fl)
{
    char *temp;
    char *temp2;
    char chk[50];
    int virtual_nr, flag, tmp, old_virtual;
    int number;

    world          = 0;
    character_list = 0;
    object_list    = 0;

    rewind(fl);

    old_virtual = -1;

    do
    {
        number = fscanf(fl, " #%d\n", &virtual_nr);
        assume(number,1, virtual_nr, "Reading #xxx");

        if (old_virtual > virtual_nr)
        {
            assume(0, 1, virtual_nr, "Error - #'s not in order.");
        }

        old_virtual = virtual_nr;

        temp = fread_string(fl);

        if ( (flag = (*temp != '$')) )
        {
            // a new record to be read
            temp2 = fread_string(fl);

            number = fscanf(fl, " %d ", &tmp);
            assume(number, 1, virtual_nr, "In room basic 3 numbers");

            number = fscanf(fl, " %d ", &tmp);
            assume(number, 1, virtual_nr, "In room basic 3 numbers");

            number = fscanf(fl, " %d ", &tmp);
            assume(number, 1, virtual_nr, "In room basic 3 numbers");

            for (;;)
            {
                number = fscanf(fl, " %s \n", chk);
                assume(number, 1, virtual_nr, "Reading D/E/S string");

                if (*chk == 'D')
                {
                    // direction field
                    setup_dir(fl, virtual_nr, atoi(chk + 1));
                }
                else if (*chk == 'E')
                {
                    // extra description field
                    temp2 = fread_string(fl); // Description
                    temp2 = fread_string(fl); // Keywords
                }
                else if (*chk == 'S')    // end of current room
                {
                    break;
                }
                else
                {
                    assume(FALSE, 0, virtual_nr, "MISSING D/E or S");
                }
            }

        }
    }
    while (flag);
}

// read direction data
void setup_dir(FILE *fl, int room, int dir)
{
    char *temp;
    int tmp;
    int number;

    temp = fread_string(fl);
    temp = fread_string(fl);

    number = fscanf(fl, " %d ", &tmp);
    assume(number, 1, room, "One of three Direction data");
    number = fscanf(fl, " %d ", &tmp);
    assume(number, 1, room, "One of three Direction data");
    number = fscanf(fl, " %d ", &tmp);
    assume(number, 1, room, "One of three Direction data");
    exist_index(wld_index, top_of_wldt, tmp);
}

// load the zone table and command tables
void check_zones(FILE *fl)
{
    char *check, buf[81];
    char cmd_type;
    int line_no;
    int number, tmp1, tmp2, tmp3, tmp4;
    int expand;
    int zon    = 0;
    int cmd_no = 0;
    int result = 0;

    rewind(fl);
    line_no = 1;

    for (;;)
    {
        number = fscanf(fl, " #%*d\n");
        assume(number, 0, line_no++, "Zone number not found");

        check = fread_string(fl);
        line_no++;

        if (*check == '$')
        {
            // end of file
            break;
        }

        // alloc a new zone

        number = fscanf(fl, " %d ", &zon);
        assume(number, 1, line_no, "Zone Room < number not found");

        number = fscanf(fl, " %d ", &zon);
        assume(number, 1, line_no, "Life Span");

        number = fscanf(fl, " %d ", &zon);
        assume(number, 1, line_no++, "Reset Mode");

        // read the command table

        cmd_no = 0;

        for (expand = 1;;)
        {
            result = fscanf(fl, " ");        // skip blanks
            number = fscanf(fl, "%c", &cmd_type);
            assume(number, 1, line_no, "Command type M/*/O/G/E/S missing");

            if (cmd_type == 'S')
            {
                break;
            }

            if (cmd_type == '*')
            {
                expand = 0;
                if (fgets(buf, 80, fl) == NULL) // skip command
                {
                    // Not necessarily a bad thing
                }
                line_no++;
                continue;
            }

            number = fscanf(fl, " %d %d %d", &tmp1, &tmp2, &tmp3);
            assume(number, 3, line_no, "Three values after command missing");

            if (cmd_type == 'M' || cmd_type == 'O' ||
                cmd_type == 'D' || cmd_type == 'P')
            {
                number = fscanf(fl, " %d", &tmp4);
                assume(number, 1, line_no, "Fourth value after command missing");
            }

            switch (cmd_type)
            {
                case 'M' :
                    exist_index(mob_index, top_of_mobt, tmp2);
                    exist_index(wld_index, top_of_wldt, tmp4);
                    break;

                case 'O' :
                    exist_index(obj_index, top_of_objt, tmp2);
                    exist_index(wld_index, top_of_wldt, tmp4);
                    break;

                case 'G' :
                    exist_index(obj_index, top_of_objt, tmp2);
                    break;

                case 'E' :
                    exist_index(obj_index, top_of_objt, tmp2);
                    break;

                case 'P' :
                    exist_index(obj_index, top_of_objt, tmp2);
                    exist_index(obj_index, top_of_objt, tmp4);
                    break;

                case 'D' :
                    exist_index(wld_index, top_of_wldt, tmp2);
                    break;

                case 'R' :
                    exist_index(wld_index, top_of_wldt, tmp2);
                    exist_index(obj_index, top_of_objt, tmp3);
                    break;

                case '*' :
                    break;

                default  :
                    printf("Illegal command type");
                    exit(1);
                    break;
            }

            if (fgets(buf, 80, fl) == NULL)    // read comment
            {
                // Not necessarily a bad thing
            }
            line_no++;
        }
    }
}

// ************************************************************************
// procedures for resetting, both play-time and boot-time                 *
// *********************************************************************  *

// read a mobile from MOB_FILE
void check_mobile(FILE *fl)
{
   char *temp;
    char bogst;
    long tmp, tmp2, tmp3;
    int virtual_nr, old_virtual, number, flag;

    old_virtual = -1;

    rewind(fl);

    do
    {
        number = fscanf(fl, " #%d\n", &virtual_nr);
        assume(number, 1, virtual_nr, "Reading #xxx");

        if (old_virtual > virtual_nr)
        {
            assume(0, 1, virtual_nr, "Error - #'s not in order.");
        }

        old_virtual = virtual_nr;

        temp = fread_string(fl);  // Namelist

        if ( (flag = (*temp != '$')) )
        {
            // a new record to be read

            //**** String data *** *
            // Name already read mob->player.name = fread_string(fl);
            temp = fread_string(fl);  // short description
            temp = fread_string(fl);  //long_description
            temp = fread_string(fl);  // player.description

            // *** Numeric data *** *

            number = fscanf(fl, "%ld ", &tmp);
            assume(number, 1, virtual_nr, "ACT error");

            number = fscanf(fl, " %ld ", &tmp);
            assume(number, 1, virtual_nr, "affected_by error");

            number = fscanf(fl, " %ld ", &tmp);
            assume(number, 1, virtual_nr, "Monster Alignment Error");

            number = fscanf(fl, " %c \n", &bogst);
            assume(number, 1, virtual_nr, "Simple/Detailed error");

            if (bogst != 'S')
            {
                printf("%c %d\n", bogst, bogst);
            }

            if (bogst == 'S')
            {
                // The new easy monsters

                number = fscanf(fl, " %ld ", &tmp);
                assume(number, 1, virtual_nr, "Level error");

                number = fscanf(fl, " %ld ", &tmp);
                assume(number, 1, virtual_nr, "THAC0 error");

                number = fscanf(fl, " %ld ", &tmp);
                assume(number, 1, virtual_nr, "AC error");

                number = fscanf(fl, " %ldd%ld+%ld ", &tmp, &tmp2, &tmp3);
                assume(number, 3, virtual_nr, "Hitpoints");

                number = fscanf(fl, " %ldd%ld+%ld \n", &tmp, &tmp2, &tmp3);
                assume(number, 3, virtual_nr, "Damage error");

                number = fscanf(fl, " %ld ", &tmp);
                assume(number, 1, virtual_nr, "GOLD error");

                number = fscanf(fl, " %ld \n", &tmp);
                assume(number, 1, virtual_nr, "XP error");

                number = fscanf(fl, " %ld ", &tmp);
                assume(number, 1, virtual_nr, "POSITION error");

                number = fscanf(fl, " %ld ", &tmp);
                assume(number, 1, virtual_nr, "DEFAULT POS error");

                number = fscanf(fl, " %ld \n", &tmp);
                assume(number, 1, virtual_nr, "SEXY error");

        }
        else
        {
            // The old monsters are down below here

            printf("Detailed monsters can't be syntax-checked (yet).\n\r");
            assume(0, 1, virtual_nr, "DETAIL ERROR");

            exit(1);
            /*   ***************************
            fscanf(fl, " %D ", &tmp);
            mob->abilities.str = tmp;

            fscanf(fl, " %D ", &tmp);
            mob->abilities.intel = tmp;

            fscanf(fl, " %D ", &tmp);
            mob->abilities.wis = tmp;

            fscanf(fl, " %D ", &tmp);
            mob->abilities.dex = tmp;

            fscanf(fl, " %D \n", &tmp);
            mob->abilities.con = tmp;

            fscanf(fl, " %D ", &tmp);
            fscanf(fl, " %D ", &tmp2);

            mob->points.max_hit = 0;
            mob->points.hit = mob->points.max_hit;

            fscanf(fl, " %D ", &tmp);
            mob->points.armor = tmp;

            fscanf(fl, " %D ", &tmp);
            mob->points.mana = tmp;
            mob->points.max_mana = tmp;

            fscanf(fl, " %D ", &tmp);
            mob->points.move = tmp;
            mob->points.max_move = tmp;

            fscanf(fl, " %D ", &tmp);
            mob->points.gold = tmp;

            fscanf(fl, " %D \n", &tmp);
            GET_EXP(mob) = tmp;

            fscanf(fl, " %D ", &tmp);
            mob->specials.position = tmp;

            fscanf(fl, " %D ", &tmp);
            mob->specials.default_pos = tmp;

            fscanf(fl, " %D ", &tmp);
            mob->player.sex = tmp;

            fscanf(fl, " %D ", &tmp);
            mob->player.class = tmp;

            fscanf(fl, " %D ", &tmp);
            GET_LEVEL(mob) = tmp;

            fscanf(fl, " %D ", &tmp);
            mob->player.birth.hours = time_info.hours;
            mob->player.birth.day    = time_info.day;
            mob->player.birth.month = time_info.month;
            mob->player.birth.year  = time_info.year - tmp;

            fscanf(fl, " %D ", &tmp);
            mob->player.weight = tmp;

            fscanf(fl, " %D \n", &tmp);
            mob->player.height = tmp;

            for (i = 0; i < 3; i++)
            {
                fscanf(fl, " %D ", &tmp);
                GET_COND(mob, i) = tmp;
            }
            fscanf(fl, " \n ");

            for (i = 0; i < 5; i++)
            {
                fscanf(fl, " %D ", &tmp);
                mob->specials.apply_saving_throw[i] = tmp;
            }

            fscanf(fl, " \n ");
            mob->points.damroll = 0;
            mob->specials.damnodice = 1;
            mob->specials.damsizedice = 6;

            mob->points.hitroll = 0;
            ************************************* */
        }

        }
    }
    while (flag);

}


// read an object from OBJ_FILE
void check_objects(FILE *fl)
{
    char chk[256];
    char *temp;
    int virtual_nr, old_virtual, number, flag;
    int tmp, i;
    int fscanf_result;

    old_virtual = -1;

    rewind(fl);

    number = fscanf(fl, " %s \n", chk);
    assume(number, 1, virtual_nr, "First #xxx number");

    do
    {
        number = sscanf(chk, " #%d\n", &virtual_nr);
        assume(number, 1, virtual_nr, "Reading #xxx");

        if (old_virtual > virtual_nr)
        {
            assume(0, 1, virtual_nr, "Error - #'s not in order.");
        }

        old_virtual = virtual_nr;

        temp = fread_string(fl);  // Namelist

        if ( (flag = (*temp != '$')) )
        {
            // a new record to be read

            // *** string data *** *

            // temp = fread_string(fl);  name has been read above
            temp = fread_string(fl); // short
            temp = fread_string(fl); // descr
            temp = fread_string(fl); // action

            // *** numeric data *** */

            number = fscanf(fl, " %d ", &tmp);
            assume(number, 1, virtual_nr, "Error reading type flag");

            number = fscanf(fl, " %d ", &tmp);
            assume(number, 1, virtual_nr, "Extra Flag");

            number = fscanf(fl, " %d ", &tmp);
            assume(number, 1, virtual_nr, "wear_flags");

            number = fscanf(fl, " %d ", &tmp);
            assume(number, 1, virtual_nr, "value[0]");

            number = fscanf(fl, " %d ", &tmp);
            assume(number, 1, virtual_nr, "value[1]");

            number = fscanf(fl, " %d ", &tmp);
            assume(number, 1, virtual_nr, "value[2]");

            number = fscanf(fl, " %d ", &tmp);
            assume(number, 1, virtual_nr, "value[3]");

            number = fscanf(fl, " %d ", &tmp);
            assume(number, 1, virtual_nr, "Weight");

            number = fscanf(fl, " %d \n", &tmp);
            assume(number, 1, virtual_nr, "Cost");

            number = fscanf(fl, " %d \n", &tmp);
            assume(number, 1, virtual_nr, "Cost Per Day");

            // *** extra descriptions ***

            while (fscanf_result = fscanf(fl, " %s \n", chk), *chk == 'E')
            {

                temp = fread_string(fl);
                temp = fread_string(fl);
            }

            for( i = 0 ; (i < MAX_OBJ_AFFECT) && (*chk == 'A') ; i++)
            {
                number = fscanf(fl, " %d ", &tmp);
                assume(number, 1, virtual_nr, "affected location");

                number = fscanf(fl, " %d \n", &tmp);
                assume(number, 1, virtual_nr, "Modifier");

                number = fscanf(fl, " %s \n", chk);
                assume(number, 1, virtual_nr, "Next string with E/A/$");
            }
        }
    }
    while (flag);
}

//***********************************************************************
// procs of a (more or less) general utility nature                     *
//********************************************************************  *

// read and allocate space for a '~'-terminated string from a given file
char *fread_string(FILE *fl)
{
    register char *point;
    static char buf[MAX_STRING_LENGTH];
    static char tmp[MAX_STRING_LENGTH];
    int flag;

    bzero(buf, MAX_STRING_LENGTH);

    do
    {
        if (!fgets(tmp, MAX_STRING_LENGTH, fl))
        {
            printf("fread_str");
            exit(1);
        }

        if (strlen(tmp) + strlen(buf) > MAX_STRING_LENGTH)
        {
            printf("fread_string: string too large (db.c, fread_string)");
            exit(1);
        }
        else
        {
            strcat(buf, tmp);
        }

        for (point = buf + strlen(buf) - 2; point >= buf && isspace(*point); point--) ;

        if ( (flag = (*point == '~')) )
        {
            if (*(buf + strlen(buf) - 3) == '\n')
            {
                *(buf + strlen(buf) - 2) = '\r';
                *(buf + strlen(buf) - 1) = '\0';
            }
            else
            {
                *(buf + strlen(buf) -2) = '\0';
            }
        }
        else
        {
            *(buf + strlen(buf) + 1) = '\0';
            *(buf + strlen(buf)) = '\r';
        }
    }
    while (!flag);

    return(buf);
}

int main(int argc, char *argv[])
{
    char name[256];

    if (argc != 2)
    {
        printf("Usage : syntax_check <BaseFileName>\n\r");
        exit(0);
    }

    strcpy(name, argv[1]);
    strcat(name, ".wld");

    if (!(wld_f = fopen(name, "r")))
    {
        printf("Could not open world file.\n\r");
        exit(1);
    }

    strcpy(name, argv[1]);
    strcat(name, ".mob");

    if (!(mob_f = fopen(name, "r")))
    {
        printf("Could not open mobile file.\n\r");
        exit(1);
    }

    strcpy(name, argv[1]);
    strcat(name, ".obj");

    if (!(obj_f = fopen(name, "r")))
    {
        printf("Could not open object file.\n\r");
        exit(1);
    }

    strcpy(name, argv[1]);
    strcat(name, ".zon");

    if (!(zon_f = fopen(name, "r")))
    {
        printf("Could not open zone file.\n\r");
        exit(1);
    }


    printf("Generating world file indexes.\n\r");
    wld_index = generate_indices(wld_f, &top_of_wldt);

    printf("Generating mobile file indexes.\n\r");
    mob_index = generate_indices(mob_f, &top_of_mobt);

    printf("Generating object file indexes.\n\r");
    obj_index = generate_indices(obj_f, &top_of_objt);

    printf("Checking World File\n\r");
    check_world(wld_f);

    printf("Checking Mobile File (only simple mobiles).\n\r");
    check_mobile(mob_f);

    printf("Checking Object File.\n\r");
    check_objects(obj_f);

    printf("Checking Zone File .\n\r");
    check_zones(zon_f);

    printf("\n\r\nCheck successfully completed without any obvious errors.\n\r");

    fclose(zon_f);
    fclose(wld_f);
    fclose(mob_f);
    fclose(obj_f);
}
