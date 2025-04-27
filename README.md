# UES
Non-blocking echo event server ported to Unikernel for maximum efficiency!

Video Link: https://www.youtube.com/watch?v=eSkm_wZw8_8

## Prereqs
- In order to run Kraftkit, download with this command: `curl https://get.kraftkit.sh | sh`
- Ensure that Docker is installed on your system and running in order to run the demo successfully. This demo was executed in a WSL environment.

## Getting Started
- Once downloaded, open terminal, and execute the following set of commands:
```sh
kraft build
kraft run -p 28723:28723
```
This will forward the servers port to the host machine in order to allow clients to connect to the host. This runs the Dockerfile rootfs version. To run the Unikraft pure version, run the following in the main directory of the project:
```sh
cd archive
kraft build
kraft run -p 28723:28723
```
This will run a server in a Unikraft instance. After that, run `make client` in the root directory of the project. To run the benchmarking software, run `make program` to compile the main client_server program and the benchmark program (called "benchmark"), then run `./benchmark` in your terminal. 

