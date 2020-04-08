/* ************************************************************************
*  file: limits.h , Limit/Gain control module             Part of DIKUMUD *
*  Usage: declaration of title type                                       *
************************************************************************* */

// Public Procedures

void gain_condition(struct char_data *ch, int condition, int value);
int mana_limit(struct char_data *ch);
int hit_limit(struct char_data *ch);
int move_limit(struct char_data *ch);

// Limit/Gain data structure(s)

struct title_type
{
    char *title_m;
    char *title_f;
    int exp;
};
