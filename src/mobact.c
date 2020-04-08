/* ************************************************************************
*  file: mobact.c , Mobile action module.                 Part of DIKUMUD *
*  Usage: Procedures generating 'intelligent' behavior in the mobiles.    *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
************************************************************************* */
#include <stdio.h>
#include "utils.h"
#include "structs.h"
#include "db.h"
#include "comm.h"
#include "handler.h"

// External variables

extern struct char_data *character_list;
extern struct index_data *mob_index;
extern struct room_data *world;
extern struct str_app_type str_app[];

// External functions

void hit(struct char_data *ch, struct char_data *victim, int type);

//
// Mobile activity
//

void mobile_activity(void)
{
    void do_move(struct char_data *ch, char *argument, int cmd);
    void do_get(struct char_data *ch, char *argument, int cmd);
    extern int no_specials;

    register struct char_data *ch;
    struct char_data *tmp_ch;
    struct obj_data *obj;
    struct obj_data *best_obj;
    char buf[256];
    int door, found, max;

    for (ch = character_list; ch; ch = ch->next)
    {
        if (IS_MOB(ch))
        {
            // Examine call for special procedure
            if (IS_SET(ch->specials.act, ACT_SPEC) && !no_specials)
            {
                if (!mob_index[ch->nr].func)
                {
                    sprintf(buf, "Non-Existing MOB[%d] SPEC procedure (mobact.c)", mob_index[ch->nr].virtual);
                    mudlog(buf);
                    REMOVE_BIT(ch->specials.act, ACT_SPEC);
                }
                else
                {
                    if ((*mob_index[ch->nr].func) (ch, 0, ""))
                    {
                        continue;
                    }
                }
            }

            if (AWAKE(ch) && !(ch->specials.fighting))
            {
                if (IS_SET(ch->specials.act, ACT_SCAVENGER))
                {
                    if (world[ch->in_room].contents && !number(0, 10))
                    {
                        for (max = 1, best_obj = 0, obj = world[ch->in_room].contents; obj; obj = obj->next_content)
                        {
                            if (CAN_GET_OBJ(ch, obj))
                            {
                                if (obj->obj_flags.cost > max)
                                {
                                    best_obj = obj;
                                    max = obj->obj_flags.cost;
                                }
                            }
                        }

                        if (best_obj)
                        {
                            obj_from_room(best_obj);
                            obj_to_char(best_obj, ch);
                            act("$n gets $p.",FALSE,ch,best_obj,0,TO_ROOM);
                        }
                    }
                }

                // Is this a mob that wanders?
                if (!IS_SET(ch->specials.act, ACT_SENTINEL) &&
                    (GET_POS(ch) == POSITION_STANDING)      &&
                    ((door = number(0, 45)) <= 5)           &&
                    CAN_GO(ch, door)                        &&
                    !IS_SET(world[EXIT(ch, door)->to_room].room_flags, NO_MOB) &&
                    !IS_SET(world[EXIT(ch, door)->to_room].room_flags, DEATH) )
                {
                    if (ch->specials.last_direction == door)
                    {
                        ch->specials.last_direction = -1;
                    }
                    else
                    {
                        if (!IS_SET(ch->specials.act, ACT_STAY_ZONE))
                        {
                            ch->specials.last_direction = door;
                            do_move(ch, "", ++door);
                        }
                        else
                        {
                            if (world[EXIT(ch, door)->to_room].zone == world[ch->in_room].zone)
                            {
                                ch->specials.last_direction = door;
                                do_move(ch, "", ++door);
                            }
                        }
                    }
                }

                if (IS_SET(ch->specials.act, ACT_AGGRESSIVE))
                {
                    found = FALSE;

                    for (tmp_ch = world[ch->in_room].people; tmp_ch && !found; tmp_ch = tmp_ch->next_in_room)
                    {
                        if (!IS_NPC(tmp_ch) && CAN_SEE(ch, tmp_ch))
                        {
                            if (!IS_SET(ch->specials.act, ACT_WIMPY) || !AWAKE(tmp_ch))
                            {
                                hit(ch, tmp_ch, 0);
                                found = TRUE;
                            }
                        }
                    }
                }
            }
        }
    }
}
