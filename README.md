# FootballSimulator2324

## Build Instructions

## Linux <img src="https://github.com/devicons/devicon/blob/master/icons/linux/linux-original.svg" title="Linux" alt="Linux" width="30" height="30"/>
### - Pre-requisiti
- Compilatore gcc installato.
  - Ubuntu: `sudo apt install build-essential`.
  - Arch: `sudo pacman -S base-devel`.
  - Fedora: `sudo dnf groupinstall "Development Tools" "Development Libraries"`.
- Docker e/o Docker compose installato.

### - Server
Per lanciare in esecuzione il server, aprire il terminale nella root directory del progetto ed eseguire `./run_app.sh`, che eseguirà `docker compose up` e alla fine dell'esecuzione esporterà un file di log.
In alternativa, se non si vuole usare Docker, eseguire `./build_server.sh` o `./build.sh`, e successivamente `./main_server`

### - Client
Per lanciare in esecuzione il client, aprire il terminale nella root directory del progetto ed eseguire `./build_client.sh` e successivamente `./main_client`.

## Windows <img src="https://github.com/devicons/devicon/blob/master/icons/windows11/windows11-original.svg" title="Windows" alt="Windows" width="30" height="30"/>
### - Pre-requisiti
- Aver installato la WSL - Windows Subsystem for Linux.
  - Comando per installarla tramite PowerShell: `wsl --install`.
- Aver installato una qualsiasi distribuzione di Linux (Ubuntu, Arch, Fedora) sulla WSL.
- Compilatore gcc installato.
  - Ubuntu: `sudo apt install build-essential`.
  - Arch: `sudo pacman -S base-devel`.
  - Fedora: `sudo dnf groupinstall "Development Tools" "Development Libraries"`.
- Docker Desktop installato.

### - Server 
Lanciare in esecuzione la WSL nella root directory del progetto (tasto destro > apri nel terminale e poi eseguire `ubuntu run`, o comando equivalente in caso si abbia un'altra distribuzione Linux installata).
Eseguire `./run_app.sh`, che eseguirà `docker compose up` e alla fine dell'esecuzione esporterà un file di log.
In alternativa, se non si vuole usare Docker, eseguire `./build_server.sh` o `./build.sh`, e successivamente `./main_server`

### - Client
Lanciare in esecuzione la WSL nella root directory del progetto (tasto destro > apri nel terminale e poi eseguire `ubuntu run`, o comando equivalente in caso si abbia un'altra distribuzione Linux installata), eseguire `./build_client.sh` e seccessivamente `./main_client`.

## Indirizzo IP e Porta
La configurazione di default permette di eseguire Server e Client in locale. Se si vuole eseguire il server in remoto, va modificata la costante `SERVER_IP_ADDRESS` in `utils/constants.h`.
Di default, viene usata la porta `12122`; e nel caso si desideri sostituirla, vanno modificati il file `compose.yml` e la costante `PORT` in `utils/constants.h`.
