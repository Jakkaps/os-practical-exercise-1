#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>

struct Alarm
{
    int pid;
    time_t time;
    char time_string[20];
};
const int MAX_ALARMS = 100;
struct Alarm alarms[MAX_ALARMS];

void play_sound()
{
#ifdef __APPLE__
    execl("/bin/sh", "sh", "-c", "afplay ./sounds/alarm.mov", NULL);
#elif __linux__
    execl("/bin/sh", "sh", "-c", "mpg123 ./sounds/alarm.mov", NULL);
#endif
}

int compare(const void *a, const void *b)
{
    struct Alarm alarm_a = *((struct Alarm *)a);
    struct Alarm alarm_b = *((struct Alarm *)b);

    if (alarm_a.pid == 0)
    {
        return 1;
    }
    else if (alarm_b.pid == 0)
    {
        return -1;
    }
    else
    {
        return alarm_a.time - alarm_b.time;
    }
}

int add_alarm(struct Alarm alarm)
{
    for (int i = 0; i < MAX_ALARMS; i++)
    {
        if (alarms[i].pid == 0)
        {
            alarms[i] = alarm;
            qsort(&alarms, MAX_ALARMS, sizeof(struct Alarm), compare);
            return 0;
        }
    }

    return 1;
}

void remove_alarm(int index)
{
    struct Alarm alarm = alarms[index];
    kill(alarm.pid, 3);
    alarms[index].pid = 0;
    qsort(&alarms, MAX_ALARMS, sizeof(struct Alarm), compare);
}

int schedule()
{
    char time_input[20];
    printf("Schedule alarm at which date and time?\n");

    scanf(" %[^\n]%*c", time_input);

    struct tm tm_value = {0};
    strptime(time_input, "%Y-%m-%d %T", &tm_value);

    time_t t = mktime(&tm_value);

    struct Alarm alarm;
    alarm.time = t;

    time_t now = time(NULL);
    time_t duration = difftime(t, now);

    if (duration < 0)
    {
        printf("Cannot set alarms in the past!\n");
        return 0;
    }

    int pid = fork();

    if (pid == 0)
    {
        sleep(duration);
        printf("RING\n");
        fflush(stdout);
        play_sound();
        exit(0);
    }
    else
    {
        alarm.pid = pid;
        strcpy(alarm.time_string, time_input);
        printf("Scheduling an alarm in %ld s\n", duration);
        if (add_alarm(alarm))
        {
            printf("No more space for alarms!\n");
        }
    }

    return 0;
}

/*
 * Clears alarms which have already gone of.
 */
void clean_alarms()
{
    for (int i = 0; i < MAX_ALARMS; i++)
    {
        if (alarms[i].pid != 0)
        {
            if ((alarms[i].time - time(NULL)) < 0)
            {
                remove_alarm(i);
                continue;
            }
        }
    }
}

void wait_for_zombies()
{
    for (int i = 0; i < MAX_ALARMS; i++)
    {
        int status;
        waitpid(alarms[i].pid, &status, WNOHANG);
    }
}

void list_alarms()
{
    clean_alarms();
    for (int i = 0; i < MAX_ALARMS; i++)
    {
        if (alarms[i].pid != 0)
        {
            struct Alarm alarm = alarms[i];
            printf("Alarm %d: ", i);
            printf("alarm_time: %s\n", alarm.time_string);
        }
    }
}

void cancel_alarm()
{
    printf("Cancel which alarm?\n");
    printf("> ");

    int index;
    scanf(" %d", &index);

    remove_alarm(index);

    printf("Remaining alarms:\n");
    list_alarms();
}

int menu()
{
    char input[100];
    printf("> ");
    scanf("%s", input);

    if (strlen(input) > 1)
    {
        printf("Commands can only be one character\n");
    }
    else if (*input == 's')
    {
        schedule();
    }
    else if (*input == 'l')
    {
        list_alarms();
    }
    else if (*input == 'c')
    {
        cancel_alarm();
    }
    else if (*input == 'x')
    {
        printf("Good bye!");
        return 1;
    }
    else
    {
        printf("Invalid command!\n");
    }

    wait_for_zombies();

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
