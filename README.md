# ransomware
Membre du groupe 10:
BOUCHAKOUR Nadir 
IMADALI Belakcem

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
