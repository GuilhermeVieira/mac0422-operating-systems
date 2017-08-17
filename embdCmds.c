#include<stdio.h> //printf().
#include<string.h> //strcmp().
#include<time.h> //tm, ctime(), localtime().
#include<unistd.h> //chown().
#include<pwd.h> // passwd, getpwnam().
#include<grp.h> // group, getgrnam().
#include"Buffer.h"

void embd_date()
{
    char *day_info;
    struct tm *clock;
    time_t epochtime;
    time(&epochtime);
    day_info = ctime(&epochtime);
    for(int i = 0; i < strlen(day_info) - 5; i++){
        printf("%c", day_info[i]);
    }
    clock = localtime(&epochtime);
    printf("%s %d\n", clock->tm_zone, clock->tm_year + 1900);
    return;
}

void embd_chown(char **commands)
{
    int i, j;
    struct passwd *user_data;
    struct group *group_data;
    Buffer *usr, *grp;
    usr = create_buffer();
    grp = create_buffer();
    i = 0;
    while(commands[1][i] != ':'){
        add_to_buffer(usr, commands[1][i]);
        i++;
    }
    if(!usr->top) user_data->pw_uid = -1;
    else{
        add_to_buffer(usr, '\0');
        user_data = getpwnam(usr->palavra);
    }
    i++;
    for(j = i; j < strlen(commands[1]); j++) add_to_buffer(grp,commands[1][j]);
    if(!grp->top) group_data->gr_gid = -1;
    else{
        add_to_buffer(grp, '\0');
        group_data = getgrnam(grp->palavra);
    }
    chown(commands[2], user_data->pw_uid, group_data->gr_gid);
    destroy_buffer(usr);
    destroy_buffer(grp);
    return;
}
