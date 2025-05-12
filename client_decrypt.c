// TP/client_decrypt.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <openssl/evp.h>
#include <arpa/inet.h>
 
int decrypt_file(const char* filepath, unsigned char* key, unsigned char* iv) {
    FILE *in = fopen(filepath, "rb");
    if (!in) return -1;
 
    char outpath[512];
    strncpy(outpath, filepath, strlen(filepath) - 4);
    outpath[strlen(filepath) - 4] = '\0';
    FILE *out = fopen(outpath, "wb");
    if (!out) { fclose(in); return -1; }
 
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
 
    unsigned char buffer[1024], plaintext[1040];
    int len, plainlen;
 
    while ((len = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        EVP_DecryptUpdate(ctx, plaintext, &plainlen, buffer, len);
        fwrite(plaintext, 1, plainlen, out);
    }
    EVP_DecryptFinal_ex(ctx, plaintext, &plainlen);
    fwrite(plaintext, 1, plainlen, out);
 
    EVP_CIPHER_CTX_free(ctx);
    fclose(in); fclose(out);
    remove(filepath);
    return 0;
}
 
int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(4242);
    inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr);
    connect(sock, (struct sockaddr*)&serv, sizeof(serv));
 
    char msg[256];
    recv(sock, msg, sizeof(msg), 0);
    printf("%s", msg);
 
    fgets(msg, sizeof(msg), stdin);
    write(sock, msg, strlen(msg));
 
    unsigned char key[32], iv[16];
    if (recv(sock, key, 32, 0) == 32 && recv(sock, iv, 16, 0) == 16) {
        printf("[*] Clé reçue. Déchiffrement...\n");
        DIR* d = opendir("TP/Projet");
        struct dirent* f;
        char filepath[512];
        while ((f = readdir(d))) {
            if (f->d_type == DT_REG && strstr(f->d_name, ".enc")) {
                snprintf(filepath, sizeof(filepath), "TP/Projet/%s", f->d_name);
                decrypt_file(filepath, key, iv);
            }
        }
        closedir(d);
    } else {
        printf("[!] Excuse refusée.\n");
    }
 
    close(sock);
    return 0;
}
