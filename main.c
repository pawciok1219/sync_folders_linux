#include "function.h"

static volatile sig_atomic_t sygnal = 0;

// Handler sygnalu.
void sigusr1_handler(int signal)
{
    if (signal == SIGUSR1) {
        syslog(LOG_INFO, "Wybudzenie demona przez sygnal SIGUSR1");
        sygnal = 1;
    }
}

int main(int argc, char * argv[])
{
    openlog("Demon synchronizujący dwa podkatalogi",LOG_PID,LOG_USER);
    if( argc<5 ){
        fprintf(stderr,"%s","Podano za małą ilość argumentów!");
        exit(EXIT_FAILURE);
    }
    int recursion = 0;
    int size_bufor = 20;
    int default_sleep = 300;
    char *dir_source;
    char *dir_target;

    int choice = 0;
    while( (choice = getopt(argc, argv, "s:d:n:b:r")) != -1){
        switch(choice){
            case 'n':
                default_sleep = atoi(optarg) < 0 ? default_sleep : atoi(optarg);
                break;
            case 's':
                dir_source = optarg;
                IsDirectory(dir_source);
                break;
            case 'd':
                dir_target = optarg;
                IsDirectory(dir_target);
                break;
            case 'r':
                recursion = 1;
                break;
            case 'b':
                size_bufor = atoi(optarg);
                if (size_bufor < 1){
                    fprintf(stderr,"%s","Podano niedozwolony rozmiar bufora !");
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr,"%s","Nieprawidłowe argumenty !");
                exit(EXIT_FAILURE);
        }
    }
    pid_t pid , sid;

    /* stworz nowy proces */

    pid = fork();
    if (pid == -1) {
        exit(EXIT_FAILURE);
    } else if (pid != 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);
    /* stwórz nową sesje i grupę procesów */

    sid = setsid();
    if(sid < 0){
        exit(EXIT_FAILURE);
    }
    if((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    if(signal(SIGUSR1,sigusr1_handler) == SIG_ERR) {
        fprintf(stderr,"%s","Nie można obsłużyć sygnału SIGUSR1 \n");
    }


    while(1)
    {
        Delete(dir_source,dir_target,recursion);
        run_demon(dir_source,dir_target,recursion,size_bufor);
        syslog(LOG_INFO,"Demon w czasie uśpienia");
        sygnal = 0;

        if (sygnal == 1 || sleep(default_sleep) == 0)
        {
            syslog(LOG_INFO, "Demon wstaje ...");
        }

    }
    closelog();
    exit(EXIT_SUCCESS);
}
