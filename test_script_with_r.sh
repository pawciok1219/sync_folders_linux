#!/bin/bash

# Na początku usuwamy z racji możliwości pozostawienia katalogów przez poprzednie odpalanie skryptu.

rm -r /home/pawel/CLionProjects/projekt_so/source_dir
rm -r /home/pawel/CLionProjects/projekt_so/target_dir

mkdir -p /home/pawel/CLionProjects/projekt_so/source_dir
mkdir -p /home/pawel/CLionProjects/projekt_so/target_dir

# Procedura delete usunie katalog zawarty w katalogu docelowym , ponieważ nie będzie go w katalogu źródłowym.
mkdir -p /home/pawel/CLionProjects/projekt_so/target_dir/delete_dir
echo "wiadomość testowa" > /home/pawel/CLionProjects/projekt_so/source_dir/delete_file.txt

# Skrypt skopiuje do katalogu docelowego katalog z katalogu źródłowego.
mkdir -p /home/pawel/CLionProjects/projekt_so/source_dir/dir_copy
echo "wiadomość testowa" > /home/pawel/CLionProjects/projekt_so/source_dir/dir_copy/copy_file.txt

# Skrypt zastapi zawartosci jednego z plikow zawartym w
#katalogu docelowym na zawartość pliku o tej samej nazwie
# z katalogu źródłowego.

mkdir -p /home/pawel/CLionProjects/projekt_so/source_dir/dir_example
mkdir -p /home/pawel/CLionProjects/projekt_so/target_dir/dir_example
echo "Prawidlowa wiadomosc" > /home/pawel/CLionProjects/projekt_so/source_dir/dir_example/example_file.txt
echo "Nieprawidlowa wiadomosc" > /home/pawel/CLionProjects/projekt_so/target_dir/dir_example/example_file.txt


echo "Wykonano skrypt do testowania funkcjonowania projektu z parametrem r (SYNCHRONIZACJA KATALOGÓW)"
