#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct State{
    float temp;
    float pres;
    int humid;
    int light_lvl; // mapped 0 - 255
    int relay; // 0 or 1
    int fan; // 0 - 255
};

typedef struct State State_t;

int report_state_string(State_t *state, char *s, size_t cnt);
int handle_command(char *s);

State_t myState;
char s[100];

char command[] = "w:set:1234:fan:128";

int main()
{

    myState.temp = 21.5;
    myState.pres = 987.45;
    myState.humid = 64;
    myState.light_lvl = 180;
    myState.relay = 0;
    myState.fan = 255;

   // report_state_string(&myState, s, sizeof(s));
    //printf("%s\n", s);

    handle_command(command);

    return 0;
}


int report_state_string(State_t *state, char *s, size_t cnt)
{
    if( snprintf(s, cnt, "e:data:0:%.2f:%.2f:%d:%d:%d:%d", state->temp, state->pres, state->humid, state->light_lvl, state->relay, state->fan) != EOF)
        return 0;
    return -1;
}


int handle_command(char *s)
{
    char *tok = strtok(s, ":");
    if(tok && strcmp(tok, "w"))
        return -1;

    tok = strtok(NULL, ":");
    if(tok && strcmp(tok, "set"))
        return -1;

    tok = strtok(NULL, ":");
    // optionally check id
    //if(tok && strcmp(tok, "set"))
       // return -1;

    tok = strtok(NULL, ":"); // ce e
    switch (tok[0])
    {
        case 'r':
            tok = strtok(NULL, ":");
            if(tok[0] == '0')
                printf("Relay off \n");
            if(tok[0] == '1')
                printf("Relay on \n");
        break;

        case 'f':
            tok = strtok(NULL, ":");
            int fan_speed = atoi(tok);
            printf("Fan speed %d\n", fan_speed);
        break;

        default:
            printf("Unknown command %s\n", tok);
            return -1;
        break;

    }

    return 0;

}
