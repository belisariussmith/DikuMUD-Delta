# CHANGES
The changelog for the Delta version of DikuMUD

## Changes since Alfa:   Belisarius Smith <bsmith@belisariussmith.com>


- Resolved conflict between <socket.h> version of int 'shutdown' and
  global variable 'shutdown' by renaming Diku version to 'shutdownMUD'

      act.wizard.c    ;;;    do_shutdown()
      comm.c          ;;;    game_loop()
      modify.c        ;;;    night_watchman(), check_reboot(), gr()
      signals.c       ;;;    shutdown_request(), hupsig()

- Included <stdlib.h> and <unistd.h> [comm.c]

- Included <time.h> and <string.h>  [reception.c]

- Included <string.h> [magic.c]

- Included <stdlib.h> [utils.h]

- Included <stdlib.h> and <unistd.h> [sign.c]

- Included <stdlib.h> and <string.h> [update.c]

- Included <stdlib.h> [insert_any.c]

- Included <stdlib.h> [delplay.c]

- Included <stdlib.h> [light.c]

- Included <stdlib.h> [repairgold.c]

- Included <time.h>   [act.wizard.c]

- Included <time.h>   [changes.c]

- Included <arpa/inet.h>   [comm.c]   (for inet_ntoa())

- Included "interpreter.h" [board.c]

- Included "handler.h"     [board.c]

- Included "interpreter.h" [handler.c]

- Included "handler.h"     [interpreter.c]

- Included "handler.h"     [limits.c]

- Included "handler.h"     [mobact.c]

- Included "comm.h"        [weather.c]

- Fixed the #define COMA_SIGN message [comm.c]

- Removed references to old coma() function, now using the new coma()

- Removed deprecated 4th parameter to bind() in comm.c and sign.c

- Renamed log() (which is now recognized as a built-in C function), to
  the more specific mudlog()

     act.obj1.c
     act.obj2.c
     act.other.c
     act.social.c
     act.wizard.c
     changes.c
     comm.c
     db.c
     fight.c
     handler.c
     interpreter.c
     magic.c
     mobact.c
     modify.c
     reception.c
     signals.c
     sign.c
     spells1.c
     spells2.c
     utility.c

- Added references to mudlog(), dice(), MIN(), and MAX() [utils.h]

- Added references to close_socket() and send_to_outdoor() [comm.h]

- Removed unnecessary and redundant reference to strdup() in act.obj1.c,
  handler.c in create_money(), fight.c in make_corpse(), spec_procs.c, magic.c and the old strdup() function in utility.c

- Removed unused NEW macro [modify.c]

- Removed pointless #ifdef/#endif [modify.c]

- Provided size of char array to fgets() in nogames() [modify.c]

- Moved dead code fclose() statement in nogames() [modify.c] above return
  statement

- Added external declaration to do_look() [magic.c]

- Updated and remade the Makefile

- Added changes.c to the Makefile

- Added -lcrypt flag in Makefile

- Removed unused SPEC bit flag from Mobs #4159 (the small troll) and #4160 (the orc)

- Removed assert() statements from dice() [utility.c], and other asserts
  in spell_parser.c and magic.c

- Deleted interpreter-quinn.c

- Included "db.h" [magic.c]

- Added reference to do_look() [act.movement.c]

- Added references to stop_fighting(), damage(), hit(), set_fighting(),
  do_simple_move(), and saves_spell()

- Commented out code that did nothing (likely deprecated or legacy code that
  was either used at some point or was meant to be used in the future). Did
  leave in commented code that could add functionality (e.g., spell_identify()
  in magic.c which if added in would provide information on a PC's attrs)

- Renamed some variables to be more useful (e.g., s -> sockfd)

- Fixed bug, turning comparison to assignment of found variable to TRUE in
  for-loop of objects inside of containers in do_identify() [act.wizard.c]

- Replaced old K&R style function parameter lists with modern standard

- Updated fgets(), fread(), and fscanf() calls that didn't capture return
  value and added a break/return statement if NULL response was returned
  in various files

- Updated exit() calls with an integer value of 1 (old version
  needed no parameter) in sign.c, insert_any.c, delplay.c, syntax_check.c,
  and mail.c

- Changed variable type of i to socklen_t in new_connection() [sign.c]

- Changed variable type of isa to sockaddr (from sockaddr_in) in
  new_connection() [sign.c]

- Cast signAddress to ( struct sockaddr * ) [sign.c]

- Added EQUIP_XXXX definitions to structs.h and changed appropriate entries
  in act.obj2.c when calling wear() related functionality, as well as do_grab()

- Added reference to gain_exp_regardless() and do_look() [act.wizard.c]

- Added reference to page_string() in board.c

- Added references to assign_mobiles(), assign_rooms(), set_title(), and
  assign_objects() [db.c]

- Added reference to gain_exp() in fight.c

- Changed game_loop() to return void instead of int [comm.c]

- Added timezone variable to game_loop() for use in gettimeofday() calls
  in comm.c

- Added reference to free_obj() [handler.c]

- Added reference to do_start() [interpreter.c]

- Added reference to stop_fighting() [limits.c]

- Added references to do_say(), do_tell(), do_action(), and do_emote() [shop.c]

- Changed return types to void (from int) and added (int signum)
  parameters to signal handling functions [signals.c]

- Added references to do_action(), do_look(), add_follower(),
  and do_get() [spec_procs.c]

- Fixed mispelled label deafult to default [syntax_check.c]

- Added references to weather_and_time(), update_time(), and
  night_watchman() to comm.c

- Cast sa (sockaddr) to ( struct sockaddr * ) in bind() call [comm.c]

- Cast isa (sockaddr) to ( struct sockaddr * ) in getsockname() call [comm.c]

- Changed variable i from int to socklen_t in new_connection() [comm.c]

- Changed variable size from int to socklen_t in new_descriptor() [comm.c]

- Changed size of char array 'tmp' from 500 to MAX_STRING_LENGTH (4096) in
  fread_string() [db.c]

- Changed size of char array 'tmp' from 100 to MAX_STRING_LENGTH (4096) in
  fread_string() [syntax_check.c]

- Removed deprecated calls to sigsetmask() in comm.c and modify.c

- Added "\n" entry to drinks[] string array in constants.c (otherwise if
  a poorly set liquid container were to be accessed, bad things happen)

- Added/Dropped/Modified various comments throughout

- Cleared warnings and cleaned areas due to misleading indentations, ambiguous
  if/for/else statements, implicit if/for/else statements, and control reaching
  end of non-void functions.

- Cleared unused variables

- Added conversion modifiers to specifiers where appropriate (e.g., %d -> %ld)

- Took care of all compiler warnings (with -Wall turned on)
