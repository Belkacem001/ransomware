# ransomware
Membre du groupe 10:

IMADALI Belakcem

BOUCHAKOUR Nadir 


# 🛡️ TP – Ransomware pédagogique
 
## 🎯 Objectif
 
Simuler le fonctionnement d’un **ransomware éducatif** pour mieux comprendre :
 
- Le chiffrement AES-256 CBC
- Les sockets TCP locaux
- La surveillance de fichiers et les minuteurs
 
## ⚙️ Fonctionnement
 
1. Le programme `ransomware` surveille `TP/Projet`.
2. Après 1h (30s en test), tous les fichiers `.txt`, `.md`, `.c`, etc. sont chiffrés.
3. Le serveur reçoit la clé/IV.
4. L'utilisateur doit s'excuser via `client_decrypt`.
5. Si accepté, les fichiers `.enc` sont restaurés.

## 🧪 Test rapide
 
```bash
cd TP/
gcc ransomware.c -o ransomware -lssl -lcrypto
gcc serveur_pardon.c -o serveur_pardon
gcc client_decrypt.c -o client_decrypt -lssl -lcrypto
 
mkdir Projet
echo "Hello world" > Projet/test.txt
 
./serveur_pardon &
./ransomware &
sleep 35
./client_decrypt
