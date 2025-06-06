# Development Logs
Summary file for any and all work that is done on this project. Separated by dates and progress made on the project. 
Overarching goals:
1. Use AS LITTLE generative AI as possible. When used, will be noted in the dev logs. 

    Reasoning: Due to how fast AI is moving, would like to build a project that I can say I built from scratch. Additionally, partially trying a new language without any assistance might be refreshing. 

2. Use VIM. Currently running a VIM extension in VSCode, and wanting to brush up on those skills that were trained prior to the addiction to VSCode that commenced.
3. Learn C++. This isn't going to be a crash course on C++, but at the very least I want to be able to say that I have some exposure to the language. 

    Reasoning: I've used Rust for Senior Design project, been trained in C for far too long, so want to take advantage of the more flexible scope (and hopefully not too complicated project) to build it in C++.   

## 4/6 
- Built a hello world program in C++ (wow, for the millionth time). 
- Tested passing in arguments to C++ program. 
- Built foundational framework (Makefile, etc). 
- Tested to determine if the user entered server or client (needed standard library and string library)
- Got a decent workflow of client and server from the main function (better to start earlier than later)
![image](https://github.com/user-attachments/assets/e602d008-b9d8-47b5-a435-a574c75e691e)

## 4/8 
- Got to work on implementing server side. Still debating on whether to use domain sockets or the regular IP sockets that are necessary. 

## 4/14
- Initial simple client (may refactor as necessary) that can send messages to server
- Refactored server to echo messages back to client (was initially typing messages back, but simpler for testing)
- Implementation of using epoll, non blocking. Need to do more testing

## 4/19
- Got a sample hello_world running when trying to build from source (no docker, no rootfs). 
- Rewrote client and server into two distinct files
    - Rewrote them in C since C++ was giving me a headache in Unikraft. 
- Porting to Unikraft failed. It doesn't port forward, and networking is finicky at best, so compiling to ELF is safer bet than having to build unikernel from scratch. 
