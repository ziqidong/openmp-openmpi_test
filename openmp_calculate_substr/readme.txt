Pour compiler le source fichier, il faut tapper la commande:
gcc-7 -o Dong_ziqi -Wall -std=c99 Dong_ziqi.c -lm -fopenmp

Pour démarrer le fichier exécutable, il faut la commande:
./Dong_ziqi test1
Il va afficher sur terminal:
14 1 5 -2 10 

./Dong_ziqi test2
Il va afficher sur terminal:
92 32 12 48

./Dong_ziqi test3
Il va afficher sur terminal:
28 11 10 -6 4 9 

Le façon d’exécuter les fonction:
void printArray(...)			non parallelisée
struct tablo * allocateTablo(...)	non parallelisée
void montee(...)			parallelisée
void maxmontee(...)			parallelisée
void descente(...)			parallelisée
void maxdescente(...)			parallelisée
void final(...)				non parallelisée
void maxfinal(...)			non parallelisée
void reversesource(...)			parallelisée 
void reverseresulat(...)		parallelisée
void calculatearrayM(…)			parallelisée