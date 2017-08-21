// Guilherme Costa Vieira               Nº USP: 9790930
// Victor Chiaradia Gramuglia Araujo    Nº USP: 9793756

#include "embdCmds.h"

void embdDate()
{
    char *day_info;
    struct tm *clock;
    time_t epochtime;
    clock = emalloc(sizeof(struct tm));
    time(&epochtime);
    day_info = ctime(&epochtime);
    // Imprime dia da semana, mês, dia do mês e horário atual.
    for (int i = 0; i < strlen(day_info) - 5; i++)
        printf("%c", day_info[i]);
    free(clock);
    clock = localtime(&epochtime);
    // Imprime a zona de fuso horário e o ano.
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

    // Procura o nome do usuário no comando fornecido.
    i = 0;
    while (commands[1][i] != ':') {
        addToBuffer(usr, commands[1][i]);
        i++;
    }

    if (!usr->top)
        uid = -1; // O usuário não foi fornecido.
    else {
        // Pega os dados do usuário e guarda em user_data.
        addToBuffer(usr, '\0');
        user_data = getpwnam(usr->word);
        uid = user_data->pw_uid;
    }
    i++;

    // Procura o nome do grupo no comando fornecido.
    for (j = i; j < strlen(commands[1]); j++)
        addToBuffer(grp,commands[1][j]);

    if (!grp->top)
        gid = -1; // O grupo não foi fornecido.
    else {
        // Pega os dados do grupo e guarda em group_data.
        addToBuffer(grp, '\0');
        group_data = getgrnam(grp->word);
        gid = group_data->gr_gid;
    }
    // Chama a syscall chown e verifica se foi bem sucedida.
    if (!chown(commands[2], uid, gid))
        fprintf(stderr,
                "ERRO: Não foi executar o comando chown.\n");

    destroyBuffer(usr);
    destroyBuffer(grp);
    return;
}
