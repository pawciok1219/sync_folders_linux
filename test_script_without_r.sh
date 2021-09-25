#!/bin/bash

# Na początku usuwamy z racji możliwości pozostawienia katalogów przez poprzednie odpalanie skryptu.

rm -r /home/pawel/CLionProjects/projekt_so/source_dir
rm -r /home/pawel/CLionProjects/projekt_so/target_dir


mkdir -p /home/pawel/CLionProjects/projekt_so/source_dir
mkdir -p /home/pawel/CLionProjects/projekt_so/target_dir

# test funkcji delete - w docelowym jest w źrodlowym nie ma.
echo "test delete" > /home/pawel/CLionProjects/projekt_so/target_dir/test_delete.txt


# Bez -r brak synchronizacji katalogów wewnętrznych.

echo "wiadomość testowa" > /home/pawel/CLionProjects/projekt_so/source_dir/copy_file.txt
mkdir -p /home/pawel/CLionProjects/projekt_so/source_dir/dir_ignore
echo "wiadomość testowa_1" > /home/pawel/CLionProjects/projekt_so/source_dir/dir_ignore/file_ignore.txt


# Inne czasy ostatniej modyfikacji.
echo "Prawidlowy czas" > /home/pawel/CLionProjects/projekt_so/source_dir/synchro_time_file.txt
sleep 2
echo "Nieprawidlowy czas" > /home/pawel/CLionProjects/projekt_so/target_dir/synchro_time_file.txt


echo "Wykonano skrypt do testowania funkcjonowania projektu bez parametru r (SYNCHRONIZACJA PLIKÓW)"