#include "function.h"
#include <time.h>

bool check_file_exist(char* file)
{
    struct stat st;
    return (stat(file,&st) == 0);
}

void setModificationTime(char* source_path, char* target_path)
{

    struct utimbuf time;
    time.modtime = CheckMtime(source_path);

    if(utime(target_path,&time) == -1){
        syslog(LOG_ERR,"Błąd przy zmianie czasu ostatniej modyfikacji!");
        exit(EXIT_FAILURE);
    }
}

off_t get_size_file(char *source)
{
    struct stat size;
    if(stat(source, &size) == 0)
    {
        return size.st_size;
    }
    syslog(LOG_ERR,"Błąd przy pobraniu rozmiaru pliku %s : ", strerror(errno));
    exit(EXIT_FAILURE);
}


// Funkcja sluzaca do porownywania czasow modyfikacji
// Zwraca 1 jesli rowne
// Zwraca 0 jesli nierowne

long comparator_time(char* source, char* target)
{
    return CheckMtime(source) == CheckMtime(target)? 1 : 0;
}


// Funkcja do uzycia w funkcjach.
// Jeżeli jest katalogiem to 0 ,w przeciwnym wypadku 1.

int CheckDirectory(char* dir)
{
    struct stat st;
    if (stat(dir, &st) == 0)
    {
        if (st.st_mode & S_IFDIR)
        {
            return 0;
        }
    }
    return 1;
}


// Funkcja do menu.
void IsDirectory(char* dir)
{
    if(!check_file_exist(dir)){
        fprintf(stderr,"%s","Podany katalog nie istnieje");
        exit(EXIT_FAILURE);
    }
    struct stat st;
    if (stat(dir, &st) == 0)
    {
        if (st.st_mode & S_IFDIR){

        }
        else{
            fprintf(stderr,"%s %s","Podana scieżka nie jest katalogiem !",dir);
            exit(EXIT_FAILURE);
        }
    }
}


// czas modyfikacji danego pliku.
time_t CheckMtime(char* file){
    struct stat czas;
    if (stat(file ,&czas) != 0)
    {
        syslog(LOG_ERR,"Błąd w pobraniu czasu ostatniej modyfikacji!");
        exit(EXIT_FAILURE);
    }
    return czas.st_mtime;
}



// sprawdza czy podana scieżka jest plikiem regularnym.
int check_regular_file(char* path) {
    struct stat st;
    lstat (path, &st);
    if (S_ISREG (st.st_mode))
        return 1;
    else
        return 0;
}

// Jeżeli w katalogu docelowym jest plik/folder ktorego nie ma w źródłowym to usuwamy.
void Delete(char* path_source, char* path_destination, int recursion)
{
    struct dirent *file;
    DIR *destination;
    destination = opendir(path_destination);

    syslog(LOG_INFO,"Szukanie przez demona zbędnych plików w katalogu docelowym");

    while (file = readdir(destination))
    {

        char* source_path = create_file_path(path_source,file->d_name);

        if (file->d_type != DT_DIR) // nie jest katalogiem
        {
            if (check_file_exist(source_path) == false)
            {
                char* delete_path = create_file_path(path_destination,file->d_name);
                syslog(LOG_INFO, "Usunięto plik %s", delete_path);
                remove(delete_path);

            }
        }
        else {
        if(recursion == 1 && (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0))
            {
                char* delete_path = malloc(strlen(path_destination) + strlen(file->d_name));
                strcpy(delete_path, path_destination);
                strcat(delete_path, "/");
                strcat(delete_path, file->d_name);
                Delete(source_path, delete_path, recursion);

                if (CheckDirectory(source_path) == 1)
                {
                    delete_path[strlen(path_destination) + 1 + strlen(file->d_name)] = '\0';
                    syslog(LOG_INFO, "Usunieto %s", delete_path);
                    remove(delete_path);
                }
            }
        }
    }
    closedir(destination);
}


void CopyFile (char* path_source,char* path_target,int size_of_bufor)
{
    unsigned char bufor[size_of_bufor];
    int descriptor_1, descriptor_2;
    ssize_t check1, check2;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;  // odczyt dla wszystkich + zapis dla uzytkownika
    descriptor_1 = open(path_source, O_RDONLY);
    descriptor_2 = open(path_target, O_CREAT | O_WRONLY | O_TRUNC , mode);
    if(descriptor_1 < 0 || descriptor_2 < 0)
    {
        syslog(LOG_ERR,"Błąd przy otwarciu pliku : %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    do
    {
        check1 = read(descriptor_1,bufor,sizeof(bufor));
        if(check1 < 0 )
        {
            syslog(LOG_ERR,"Błąd przy kopiowaniu : %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
        check2 = write(descriptor_2,(void*)bufor, check1);
        if(check1 != check2)
        {
            syslog(LOG_ERR,"Błąd przy zapisie : %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    while(check1 == sizeof(bufor));
    close(descriptor_1);
    close(descriptor_2);
}

void CopyFileWithMapping (char* path_source, char* path_target)
{

    off_t size_t = get_size_file(path_source);

    int descriptor_1, descriptor_2;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    descriptor_1 = open(path_source, O_RDONLY);
    descriptor_2 = open(path_target, O_CREAT | O_WRONLY | O_TRUNC , mode);
    if(descriptor_1 < 0 || descriptor_2 < 0)
    {
        syslog(LOG_ERR,"Błąd przy otwarciu pliku : %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    char *map = (char*) mmap (NULL, size_t, PROT_READ, MAP_SHARED | MAP_FILE, descriptor_1, 0);
    //http://man7.org/linux/man-pages/man2/mmap.2.html
    if(map == MAP_FAILED)
    {
        syslog(LOG_ERR,"Błąd mapowania : %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    write(descriptor_2, map, size_t);

    close(descriptor_1);
    close(descriptor_2);

    if(munmap(map, size_t) == -1)
    {
        syslog(LOG_ERR,"Niepowodzenie w munmap");
    }
}
// funkcja odpowiadajaca za wybor ktorym sposobem kopiowac,
// Jesli maly bufor -> copy_file
// jesli rozmiar pliku wiekszy niz zalozony rozmiar / ustawiony rozmiar (standardowo buf = 16)
// to funkcja uzywajaca mmap
void Copy(char *source,char *target,int size_bufor)
{
    off_t size_t;
    struct stat st;
    int result = stat(source, &st);


    if (result == 0) {
        size_t = st.st_size;
    } else {
        fprintf(stderr, "%s %s", "Błąd przy pobieraniu rozmiaru pliku", source);
    }

    if (size_bufor >= size_t)
    {
        CopyFile(source,target,size_bufor);
        syslog(LOG_INFO,"Utworzono plik %s",target);
    }
    else
    {
        CopyFileWithMapping(source,target);
        syslog(LOG_INFO,"Utworzono plik przez mapowanie %s",target);

    }
}

char* create_file_path(char* path,char* addition){
    char* new_path = malloc(strlen(path) + strlen(addition));
    strcpy(new_path,path);
    strcat(new_path,"/");
    strcat(new_path,addition);
    new_path[strlen(path) + strlen(addition) + 1] = '\0';
    return new_path;
}

void run_demon(char* path_source, char* path_destination, int recursion, int bufor_size) {
    DIR *source;
    struct dirent *file;
    source = opendir(path_source);
    syslog(LOG_INFO, "Rozpoczecie analizy zawartości scieżki źródłowej i docelowej.");

    while (file = readdir(source)) {

        // Czytana sciezka jest plikiem regularnym.
        if (file->d_type == DT_REG) {
            char *source_check = create_file_path(path_source, file->d_name);
            char *target_check = create_file_path(path_destination, file->d_name);

            // Plik ze sciezki zrodlowej nie istnieje w docelowym.
            if (check_file_exist(target_check) == false) {
                Copy(source_check, target_check, bufor_size);
                setModificationTime(source_check, target_check);

            } // W dwoch sciezkach istnieje taki plik lecz z inna data ostatniej modyfikacji.
            else if ((comparator_time(source_check, target_check) == 0) && (check_file_exist(target_check) == true)) {
                Copy(source_check, target_check, bufor_size);
                setModificationTime(source_check, target_check);
            }
        }
            // Podana scieżka jest katalogiem.
        else if (file->d_type == DT_DIR) {
            if (recursion == 1 && (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0)) {

                char *source_check2 = create_file_path(path_source, file->d_name);
                char *target_check2 = create_file_path(path_destination, file->d_name);

                if (CheckDirectory(target_check2) == 1) {
                    mkdir(target_check2, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                    syslog(LOG_INFO, "Utworzono katalog %s", target_check2);
                    run_demon(source_check2, target_check2, recursion, bufor_size);
                } else {
                    run_demon(source_check2, target_check2, recursion, bufor_size);
                }
            }
        }
    }
}