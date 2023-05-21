# override_42

## About

Over Ride, is a programming project that aims to teach developers how to exploit an elf-like binary in the i386 system. The project consists of mandatory and bonus parts, which are designed to challenge the developer's logic and problem-solving skills. 

The mandatory/bonus parts of the project involves finding and exploiting vulnerabilities in a provided binary file. The developer must use various techniques such as buffer overflow, format string vulnerability, and return-to-libc attack to gain control of the program's execution flow. 

Throughout the project, developers are encouraged to think critically and independently when facing challenges. The goal is not only to learn how to exploit binaries but also to develop good programming practices that prioritize security from the start.

## Subject
+ [Override Subject](/Override_subject.pdf)

## Explanation of Key Terms Used in Override Project.

- Elf-like binary: An ELF (Executable and Linkable Format) binary is a type of file format used for executables, object code, shared libraries, and core dumps. The term "elf-like" refers to a binary file that follows the same structure and conventions as an ELF file.

- Binary exploitation: Binary exploitation is the process of finding and exploiting vulnerabilities in a compiled program or binary file. This can involve techniques such as buffer overflow, format string vulnerability, and return-to-libc attack.

- i386 system: The i386 architecture is a 32-bit architecture used by Intel processors. It is commonly used in desktop and laptop computers running Windows or Linux operating systems. The i386 system refers to any system that uses this architecture.

- Vulnerability: A vulnerability is a weakness or flaw in a system that can be exploited by an attacker to gain unauthorized access or perform malicious actions.

- Buffer overflow: A buffer overflow is a type of vulnerability where an attacker sends more data than a program can handle, causing it to overwrite adjacent memory locations and potentially execute arbitrary code.

- Format string vulnerability: A format string vulnerability is a type of vulnerability where an attacker can control the format string argument passed to a function, allowing them to read or write arbitrary memory locations.

- Return-to-libc attack: A return-to-libc attack is a type of exploit where an attacker overwrites the return address on the stack with the address of a libc function, allowing them to execute arbitrary code without injecting any shellcode. This technique is often used when stack protection mechanisms such as stack canaries are enabled.

- Shellcode: A shellcode  is a small piece of code that is used as the payload in an exploit. It is typically written in assembly language and designed to be injected into a vulnerable program's memory space. Once executed, the shellcode can perform various actions such as opening a shell, downloading and executing additional malware, or modifying system files.

## Content

Each folder at every level contains the following files:

- `flag`: The password needed for the next user.
- `source`: Pseudocode illustrating the binary code.
- `walkthrough`: A document providing instructions on how to discover the password.


---
### PS
`Level08` and `Level09` were successfully completed by [Ayoub Ben Brahim](https://github.com/AyoubBenBrahim)