#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>

#ifdef q1
#define TEST_DIR "./test_ssd_mf" //Dossier de test, à supprimer a la main, rm -rf test_ssd

#elif q2
#define TEST_DIR "./test_ssd_tmf"

#elif q3
#define TEST_DIR "./test_ssd_el" //Dossier de test, à supprimer a la main, rm -rf test_ssd

#else
#define TEST_DIR "./test_ssd"
#endif

#define FILE_PREFIX "file_"
#define BLOCK_SIZE (512 * 1024)  // (4*1024) = 4 Ko
#define FILE_SIZE (1024 * 1024*10) // (1024 * 1024 * 10) = 10 Mo, (1024 * 1024 * 100) = 100 Mo, (1024L * 1024L * 1024L) = 1 Go
#define MAX_FILES 1000000 // Nombre maximal de fichiers à tester

void test_max_files() {
    char filename[256];
    FILE *fp;
    int count = 0;
    FILE *log = fopen("res.txt", "a");
	if (!log) {
		perror("Erreur ouverture fichier log");
		return;
	}
	mkdir(TEST_DIR, 0777);
	fprintf(log, "Début du test du nombre maximal de fichier...\n");
    for (int i = 0; i < MAX_FILES; i++) {
        snprintf(filename, sizeof(filename), "%s/%s%d.txt", TEST_DIR, FILE_PREFIX, i);
        fp = fopen(filename, "w");
        if (!fp) {
            printf("Nombre maximum de fichiers atteints: %d\n", count);
            break;
        }
        fclose(fp);
        count++;
    }
    fprintf(log, "Taille maximale atteinte: %lu fichier\n", count);
    printf("fin test_max_files");
    fclose(log);
}

void test_max_file_size() {
    char filename[256];
    mkdir(TEST_DIR, 0777);
    snprintf(filename, sizeof(filename), "%s/max_file.bin", TEST_DIR);
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("Erreur lors de la création du fichier");
        return;
    }
    FILE *log = fopen("res.txt", "a");
	if (!log) {
		perror("Erreur ouverture fichier log");
		return;
	}
    size_t size = 0;
    char *buffer = malloc(FILE_SIZE);
    if (!buffer) {
        perror("Échec d'allocation mémoire");
        fprintf(log, "Échec d'allocation mémoire.\n");
        fclose(fp);
        fclose(log);
        return;
    }
    memset(buffer, 'A', FILE_SIZE);
    fprintf(log, "Début du test de la taille maximale du fichier...\n");
    while (fwrite(buffer, 1, FILE_SIZE, fp) == FILE_SIZE) {
        size += FILE_SIZE;
        if (size % (1024L * 1024L * 1024L * 5) == 0) {
          printf("Écrit %lu Go...\n", size / (1024L * 1024L * 1024L));
        }
    }
    
    fprintf(log,"Taille maximale du fichier atteinte: %lu Mo\n", size / (1024 * 1024));
    fprintf(log, "Taille maximale atteinte : %lu Go\n", size / (1024L * 1024L * 1024L));
    fclose(fp);
    free(buffer);
    fclose(log);
}

void test_read_write_speed() {
    char filename[256];
    mkdir(TEST_DIR, 0777);
    snprintf(filename, sizeof(filename), "%s/speed_test.bin", TEST_DIR);
    FILE *fp = fopen(filename, "wb");
    struct timeval start, end;
    
    if (!fp) {
        perror("Erreur ouverture fichier");
        return;
    }
    FILE *log = fopen("res.txt", "a");
	if (!log) {
		perror("Erreur ouverture fichier log");
		return;
	}
	fprintf(log, "Début du test Ecriture/Lecture...\n");
    char *buffer = malloc(FILE_SIZE);
    memset(buffer, 'B', FILE_SIZE);
    
    gettimeofday(&start, NULL);
    fwrite(buffer, 1, FILE_SIZE, fp);
    gettimeofday(&end, NULL);
    
    double write_time = (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_usec - start.tv_usec);
    fprintf(log,"Temps d'écriture: %.2f ms\n", write_time / 1000);
    fclose(fp);
    
    fp = fopen(filename, "rb");
    if (!fp) {
        perror("Erreur ouverture fichier");
        return;
    }
    
    gettimeofday(&start, NULL);
    fread(buffer, 1, FILE_SIZE, fp);
    gettimeofday(&end, NULL);
    
    double read_time = (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_usec - start.tv_usec);
    fprintf(log,"Temps de lecture: %.2f ms\n", read_time / 1000);
    fclose(log);
    fclose(fp);
    free(buffer);
}

void test_block_access() {
    char filename[] = "block_access_test.bin";
    FILE *fp = fopen(filename, "wb+");
    struct timeval start, end;

    if (!fp) {
        perror("Erreur ouverture fichier");
        return;
    }

    // Allouer un buffer pour écrire/ lire les blocs
    char *buffer = malloc(BLOCK_SIZE);
    if (!buffer) {
        perror("Erreur allocation mémoire");
        fclose(fp);
        return;
    }
    memset(buffer, 'A', BLOCK_SIZE);

    // Test d'écriture pour différents blocs
    gettimeofday(&start, NULL);
    for (long i = 0; i < FILE_SIZE; i += BLOCK_SIZE) {
        fseek(fp, i, SEEK_SET);  // Se déplacer à la position du bloc
        fwrite(buffer, 1, BLOCK_SIZE, fp);
        fsync(fileno(fp));  // Forcer l'écriture sur disque
    }
    gettimeofday(&end, NULL);
    double write_time = (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_usec - start.tv_usec);
    if (write_time < 0) {
        printf("Erreur de calcul du temps d'écriture\n");
    } else {
        printf("Temps d'ecriture pour %.2f Mo en blocs de %.2f Ko: %.2f ms\n", FILE_SIZE / (1024.0 * 1024), BLOCK_SIZE / 1024.0, write_time / 1000);
    }

    // Réinitialiser le fichier pour la lecture
    fseek(fp, 0, SEEK_SET);

    // Test de lecture pour différents blocs
    gettimeofday(&start, NULL);
    for (long i = 0; i < FILE_SIZE; i += BLOCK_SIZE) {
        fseek(fp, i, SEEK_SET);  // Se déplacer à la position du bloc
        fread(buffer, 1, BLOCK_SIZE, fp);
    }
    gettimeofday(&end, NULL);
    double read_time = (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_usec - start.tv_usec);
    if (read_time < 0) {
        printf("Erreur de calcul du temps de lecture\n");
    } else {
        printf("Temps de lecture pour %.2f Mo en blocs de %.2f Ko: %.2f ms\n", FILE_SIZE / (1024.0 * 1024), BLOCK_SIZE / 1024.0, read_time / 1000);
    }

    fclose(fp);
    free(buffer);
}

int main() {
	#ifdef q1
    printf("Test du nombre maximal de fichiers\n");
    test_max_files();
    
    #elif q2
    printf("\nTest de la taille maximale d'un fichier\n");
    test_max_file_size();
    
    #elif q3
    printf("\nTest des vitesses de lecture/écriture\n");
    test_read_write_speed();

    #elif q4
    printf("\nTest cours acces aux blocs\n");
    test_block_access();

    #endif
    return 0;
}

