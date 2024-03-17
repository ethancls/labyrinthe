ETAPE 1 : ajouter des ";" a la fin des derniers éléments de la ligne (j'ai pas compris pourquoi faire %19[^\n] ne fonctionnait pas avec j = LARGEUR - 1, c'est à a dire qu'on voit si on est au dernier élément et qu'on le lit avec le saut de ligne). Donc pour ca on utilise "conv.py" qui ajoute ce ";" (deja fait dans le converted.txt) et on regarde si le prochain caractere est un "\n" ou EOF donc break ou on remets le char avec ungetc

ETAPE 2 : gcc -O3 -Wall -Wextra chemin.c -o chemin

ETAPE 3 : ./chemin labyrinthe_hard.txt

Le programme utilise une lecture en chaine de caracteres pour faire du padding avec des 0 jusqu'a une certaine precision (16 ici), puis transforme en uint64 avec strtoull

PS: gestion mémoire ok, no leaks avec valgrind

en prime, une barre de progression :) 

La valeur affichée est sans le ".", mais on compte le 16 de precision - 1 (pour les 0,...) donc on place le "." a 10^15 environ donc 4944.976 613 649 986 805