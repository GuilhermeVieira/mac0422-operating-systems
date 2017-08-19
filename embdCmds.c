#include <stdio.h> //printf().
#include <string.h> //strcmp().
#include <time.h> //tm, ctime(), localtime().
#include <unistd.h> //chown().
#include <pwd.h> // passwd, getpwnam().
#include <grp.h> // group, getgrnam().
#include "Buffer.h"

#include<stdlib.h>

void embdDate()
{
    char *day_info;
    struct tm *clock;
    time_t epochtime;
    clock = emalloc(sizeof(struct tm));
    time(&epochtime);
    day_info = ctime(&epochtime);
    for(int i = 0; i < strlen(day_info) - 5; i++)
        printf("%c", day_info[i]);
    free(clock);
    clock = localtime(&epochtime);
    printf("%s %d\n", clock->tm_zone, clock->tm_year + 1900);
    return;
}

void embdChown(char **commands)
{
    int i, j;
    struct passwd *user_data;
    struct group *group_data;
    uid_t uid;
    gid_t gid;
    Buffer *usr, *grp;
    usr = createBuffer();
    grp = createBuffer();
    i = 0;
    while(commands[1][i] != ':'){
        addToBuffer(usr, commands[1][i]);
        i++;
    }
    if(!usr->top)
        uid = -1;
    else{
        addToBuffer(usr, '\0');
        user_data = getpwnam(usr->palavra);
        uid = user_data->pw_uid;
    }
    i++;
    for(j = i; j < strlen(commands[1]); j++)
        addToBuffer(grp,commands[1][j]);
    if(!grp->top)
        gid = -1;
    else{
        addToBuffer(grp, '\0');
        group_data = getgrnam(grp->palavra);
        gid = group_data->gr_gid;
    }
    chown(commands[2], uid, gid);
    destroyBuffer(usr);
    destroyBuffer(grp);
    return;
}
