# UES
Non-blocking echo event server ported to Unikernel for maximum efficiency!

## Prereqs
- In order to run Kraftkit, download with this command: `curl https://get.kraftkit.sh | sh`
- Ensure that Docker is installed on your system and running in order to run the demo successfully. This demo was executed in a WSL environment.

## Getting Started
- Once downloaded, open terminal, and execute the following set of commands:
```sh
kraft build
kraft run -p 28723:28723
```
This will forward the servers port to the host machine in order to allow clients to connect to the host. 