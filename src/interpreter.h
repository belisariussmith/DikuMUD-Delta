/* ************************************************************************
*  file: Interpreter.h , Command interpreter module.      Part of DIKUMUD *
*  Usage: Procedures interpreting user command                            *
************************************************************************* */

// Command interpreter procedures

void command_interpreter(struct char_data *ch, char *argument);
void argument_interpreter(char *argument, char *first_arg, char *second_arg);
void half_chop(char *string, char *arg1, char *arg2);
void nanny(struct descriptor_data *d, char *arg);
char *one_argument(char *argument,char *first_arg);
char lower( char c );
int search_block(char *arg, char **list, bool exact);
int fill_word(char *argument);
int old_search_block(char *argument,int begin,int length,char **list,int mode);
int is_abbrev(char *arg1, char *arg2);

// Command interpreter data structure(s)

struct command_info
{
    void (*command_pointer) (struct char_data *ch, char *argument, int cmd);
    byte minimum_position;
    byte minimum_level;
};
