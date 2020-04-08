///////////////////////////////////////////////////////////////////////////////
// file: sign.c , sign server                                  Part of DikuMUD
// Usage: Present a message on a port
///////////////////////////////////////////////////////////////////////////////
// Expand: Mostly used for when the MUD is down to provide players information
//        on why the MUD is down and when it may come back up, however, can be
//        used or modified for any purpose really.
///////////////////////////////////////////////////////////////////////////////
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// Local Functions

void watch(int port, char *text);
void wave(int sock, char *text);
void nonblock(int s);
int new_connection(int s);
int init_socket(int port);
int write_to_descriptor(int desc, char *txt);

// Primary Program

int main(int argc, char **argv)
{
    char txt[2048];
    char buf[83];
    FILE *fl;
    int port;

    if (argc != 3)
    {
        fputs("Usage: sign (<filename> | - ) <port #>\n", stderr);
        exit(1);
    }

    if (!strcmp(argv[1], "-"))
    {
        fl = stdin;
        puts("Input text (terminate with ^D)");
    }
    else if (!(fl = fopen(argv[1], "r")))
    {
        perror(argv[1]);
        exit(1);
    }

    for (;;)
    {
        if (fgets(buf, 81, fl) == NULL)
        {
            if (feof(fl))
            {
                break;
            }
            else
            {
                fputs("sign: error: error reading file, possible corrupted.", stderr);
                exit(0);
            }
        }

        strcat(buf, "\r");

        if (strlen(buf) + strlen(txt) > 2048)
        {
            fputs("String too long\n", stderr);
            exit(1);
        }

        strcat(txt, buf);
    }

    if ((port = atoi(argv[2])) <= 1024)
    {
        fputs("Illegal port #\n", stderr);
        exit(1);
    }

    watch(port, txt);
}

void watch(int port, char *text)
{
    fd_set input_set;
    int mother;

    mother = init_socket(port);

    FD_ZERO(&input_set);

    for(;;)
    {
        FD_SET(mother, &input_set);

        if (select(64, &input_set, 0, 0, 0) < 0)
        {
            perror("select");
            exit(1);
        }

        if (FD_ISSET(mother, &input_set))
        {
            wave(mother, text);
        }
    }
}

void wave(int sock, char *text)
{
    int socketfd;

    if ((socketfd = new_connection(sock)) < 0)
    {
        return;
    }

    write_to_descriptor(socketfd, text);
    sleep(6);
    close(socketfd);
}

int new_connection(int s)
{
    // struct sockaddr peer;
    struct sockaddr isa;
    socklen_t socketSize;
    int t;

    socketSize = sizeof(isa);
    getsockname(s, &isa, &socketSize);

    if ((t = accept(s, &isa, &socketSize)) < 0)
    {
        perror("Accept");
        return(-1);
    }

    nonblock(t);

    /*
    socketSize = sizeof(peer);
    if (!getpeername(t, &peer, &socketSize))
    {
        *(peer.sa_data + 49) = '\0';
        sprintf(buf, "New connection from addr %s\n", peer.sa_data);
        mudlog(buf);
    }
    */

    return(t);
}

int init_socket(int port)
{
    struct sockaddr_in signAddress;
    struct hostent *hp;
    struct linger ld;
    char *opt;
    char hostname[1024];
    int socketfd;

    bzero(&signAddress, sizeof(struct sockaddr_in));
    gethostname(hostname, 1023);
    hp = gethostbyname(hostname);

    if (hp == NULL)
    {
        perror("gethostbyname");
        exit(1);
    }

    signAddress.sin_family  = hp->h_addrtype;
    signAddress.sin_port    = htons(port);
    socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketfd < 0)
    {
        perror("Init-socket");
        exit(1);
    }

    if (setsockopt (socketfd, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof (opt)) < 0)
    {
        perror ("setsockopt REUSEADDR");
        exit(1);
    }

    ld.l_onoff = 1;
    ld.l_linger = 1000;

    if (setsockopt(socketfd, SOL_SOCKET, SO_LINGER, &ld, sizeof(ld)) < 0)
    {
        perror("setsockopt LINGER");
        exit(1);
    }

    if (bind(socketfd, (struct sockaddr *) &signAddress, sizeof(signAddress)) < 0)
    {
        perror("bind");
        close(socketfd);
        exit(1);
    }

    listen(socketfd, 5);
    return(socketfd);
}

int write_to_descriptor(int desc, char *txt)
{
    int sofar;
    int thisround;
    int total;

    total = strlen(txt);
    sofar = 0;

    do
    {
        thisround = write(desc, txt + sofar, total - sofar);

        if (thisround < 0)
        {
            perror("Write to socket");
            return(-1);
        }

        sofar += thisround;
    }
    while (sofar < total);

    return(0);
}

void nonblock(int s)
{
    if (fcntl(s, F_SETFL, FNDELAY) == -1)
    {
        perror("Noblock");
        exit(1);
    }
}
