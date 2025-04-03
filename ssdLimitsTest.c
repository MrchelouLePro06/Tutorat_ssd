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
#define FILE_SIZE (1024L * 1024L * 1024L*10) // (1024 * 1024 * 10) = 10 Mo, (1024 * 1024 * 100) = 100 Mo, (1024L * 1024L * 1024L) = 1 Go
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
    printf("Temps d'écriture: %.2f ms\n", write_time / 1000);
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
    #endif
    return 0;
}

