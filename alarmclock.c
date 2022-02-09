#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

struct Alarm
{
    int pid;
    time_t time;
};

struct Alarm alarms[100];

int schedule()
{
    char time_input[20];
    printf("Schedule alarm at which date and time?\n");

    scanf(" %[^\n]%*c", time_input);

    printf("\nINput: %s\n", time_input);

    struct tm tm_value = {0};
    strptime(time_input, "%Y-%m-%d %T", &tm_value);

    time_t t = mktime(&tm_value);

    struct Alarm alarm;
    alarm.time = t;

    int pid = fork();

    time_t now = time(NULL);
    time_t duration = alarm.time - now;

    if (pid == 0)
    {
        sleep(duration);
        printf("\aRING");
        exit(0);
    }
    else
    {
        printf("%ld\n", duration);
        alarm.pid = pid;
    }

    return 0;
}

int menu()
{
    char input[1];
    printf("> ");
    scanf("%1s", input);

    if (*input == 's')
    {
        schedule();
    }
    else if (*input == 'l')
    {
    }
    else if (*input == 'c')
    {
    }
    else if (*input == 'x')
    {
        return 1;
    }

    return 0;
}

int main()
{
    printf("Please enter \"s\" (schedule), \"l\" (list), \"c\" (cancel), \"x\" (exit)\n");

    while (1)
    {
        if (menu())
        {
            break;
        }
    }

    return 0;
}
