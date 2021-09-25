#ifndef PROJEKT_SO_FUNCTION_H
#define PROJEKT_SO_FUNCTION_H
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <utime.h>
#include <fcntl.h>
#include <syslog.h>
#include <assert.h>
#include <errno.h>
#include <linux/fs.h>
#include <syslog.h>

// sprawdza czas modyfikacji pliku
time_t CheckMtime(char* file);

// sprawdza czy plik jest katalogiem -> zwraca blad
int CheckDirectory(char* dir);
void IsDirectory(char* dir);


// sprawdza czy podana scieżka jest plikiem regularnym.
int check_regular_file(char* path);

// porównuje czasy modyfikacji dwóch plików.
long comparator_time(char* source, char* target);

// Funkcja do zmiany czasu ostatniej modyfikacji na taka jaka ma pierwsza sciezka.
void setModificationTime(char* source_path, char* target_path);

// Czy dany plik istnieje.
bool check_file_exist(char* file);

// rozmiar pliku in
off_t get_size_file(char *source);

// Tworzy sciezke do pliku.
char* create_file_path(char* path,char* addition);

// Usuwanie pliku.
void Delete(char* path_source, char* path_destination, int recursion);

// Funkcje kopiowania.
void CopyFile (char *input,char *output,int size_of_bufor);
void CopyFileWithMapping (char *input, char *output);
void Copy(char *input,char *output,int size_bufor);

// Funkcja główna demona.
void run_demon(char* path_source, char* path_destination, int recursion, int bufor_size);

#endif //PROJEKT_SO_FUNCTION_H
