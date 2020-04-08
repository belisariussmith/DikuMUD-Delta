/* ************************************************************************
*  file: signals.c , trapping of signals from Unix.       Part of DIKUMUD *
*  Usage : Signal Trapping.                                               *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
************************************************************************* */
#include <signal.h>
#include <stdio.h>
#include <sys/time.h>
#include "utils.h"

// Local functions

void checkpointing(int signum);
void shutdown_request(int signum);
void logsig(int signum);
void hupsig(int signum);

//
// Signal handling procedures
//

void signal_setup(void)
{
    struct itimerval itime;
    struct timeval interval;

    signal(SIGUSR2, shutdown_request);

    // just to be on the safe side:

    signal(SIGHUP,   hupsig);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT,   hupsig);
    signal(SIGALRM,  logsig);
    signal(SIGTERM,  hupsig);

    // set up the deadlock-protection

    interval.tv_sec   = 900;    // 15 minutes
    interval.tv_usec  = 0;
    itime.it_interval = interval;
    itime.it_value    = interval;

    setitimer(ITIMER_VIRTUAL, &itime, 0);
    signal(SIGVTALRM, checkpointing);
}

void checkpointing(int signum)
{
    extern int tics;

    if (!tics)
    {
        mudlog("CHECKPOINT shutdown: tics not updated");
        abort();
    }
    else
    {
        tics = 0;
    }
}

void shutdown_request(int signum)
{
    extern int shutdownMUD;

    mudlog("Received USR2 - shutdown request");
    shutdownMUD = 1;
}

// kick out players etc
void hupsig(int signum)
{
    extern int shutdownMUD;

    mudlog("Received SIGHUP, SIGINT, or SIGTERM. Shutting down");
    exit(0);   // something more elegant should perhaps be substituted
}

void logsig(int signum)
{
    mudlog("Signal received. Ignoring.");
}
