# Compiler
CC     = gcc
# Extra compiler flags
EFLAGS = -Wall -Wno-char-subscripts -Wno-unused-but-set-variable
# Libraries
LIBS   = -lcrypt
# Compiler flags
CFLAGS = -g -O2 $(EFLAGS)

SRCFILES := comm.c act.comm.c act.informative.c act.movement.c act.obj1.c act.obj2.c act.offensive.c act.other.c act.social.c act.wizard.c changes.c handler.c db.c interpreter.c utility.c spec_assign.c shop.c limits.c mobact.c fight.c modify.c weather.c spells1.c spells2.c spell_parser.c reception.c constants.c spec_procs.c signals.c board.c mar_fiz_maz.c magic.c
OBJFILES := comm.o act.comm.o act.informative.o act.movement.o act.obj1.o act.obj2.o act.offensive.o act.other.o act.social.o act.wizard.o changes.o handler.o db.o interpreter.o utility.o spec_assign.o shop.o limits.o mobact.o fight.o modify.o weather.o spell_parser.o spells1.o spells2.o reception.o constants.o spec_procs.o signals.o board.o mar_fiz_maz.o magic.o

default: diku

all:
	$(MAKE) diku sign delplay insert_any repairgold list syntax_check update

diku : $(OBJFILES)
	$(CC) -o diku $(OBJFILES) $(LIBS)

sign: sign.o
	$(CC) -o sign sign.o

delplay: delplay.o
	$(CC) -o delplay delplay.o

insert_any: insert_any.o
	$(CC) -o insert_any insert_any.o

repairgold: repairgold.o
	$(CC) -o repairgold repairgold.o

list: list.o
	$(CC) -o list list.o

syntax_check: syntax_check.o
	$(CC) -o syntax_check syntax_check.o

update: update.o
	$(CC) -o update update.o

$%.o: %.c
	$(CC) $< $(CFLAGS) -c -o $@

clean:
	rm -f *.o diku sign delplay insert_any repairgold list syntax_check update
