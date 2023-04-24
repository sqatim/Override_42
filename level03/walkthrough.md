# Walkthrough

After successfully logging in as the `level03` user, I discovered a program named `"level03"`. Upon executing the program, it prompted me to enter a password to gain access. In order to understand how the program worked, I used the `objdump` program to list the functions within it and then used `gdb` to reverse engineer it.

```sh
level03@OverRide:~$ objdump -t ./level03  | grep .text
08048540 l    d  .text 00000000              .text
08048570 l     F .text 00000000              __do_global_dtors_aux
080485d0 l     F .text 00000000              frame_dummy
08048970 l     F .text 00000000              __do_global_ctors_aux
08048960 g     F .text 00000002              __libc_csu_fini
08048660 g     F .text 000000e7              decrypt
08048962 g     F .text 00000000              .hidden __i686.get_pc_thunk.bx
080485f4 g     F .text 00000023              clear_stdin
080488f0 g     F .text 00000061              __libc_csu_init
08048617 g     F .text 00000038              get_unum
0804864f g     F .text 00000011              prog_timeout
08048540 g     F .text 00000000              _start
0804885a g     F .text 00000087              main
08048747 g     F .text 00000113              test
```

I began by disassembling the `main` function, which started by initializing the random number generator using the `srand` function. The program then prompted the user to enter a password using the `scanf` function, and passed the entered value along with the number `322424845` as parameters to the `test` function.

```s
(gdb) disas main
Dump of assembler code for function main:
   0x0804885a <+0>: push   ebp
   0x0804885b <+1>: mov    ebp,esp
   0x0804885d <+3>: and    esp,0xfffffff0
   0x08048860 <+6>: sub    esp,0x20
   0x08048863 <+9>: push   eax
   0x08048864 <+10>: xor    eax,eax
   0x08048866 <+12>: je     0x804886b <main+17>
   0x08048868 <+14>: add    esp,0x4
   0x0804886b <+17>: pop    eax
   0x0804886c <+18>: mov    DWORD PTR [esp],0x0
   0x08048873 <+25>: call   0x80484b0 <time@plt>
   0x08048878 <+30>: mov    DWORD PTR [esp],eax
   0x0804887b <+33>: call   0x8048500 <srand@plt>
   0x08048880 <+38>: mov    DWORD PTR [esp],0x8048a48
   0x08048887 <+45>: call   0x80484d0 <puts@plt>
   0x0804888c <+50>: mov    DWORD PTR [esp],0x8048a6c
   0x08048893 <+57>: call   0x80484d0 <puts@plt>
   0x08048898 <+62>: mov    DWORD PTR [esp],0x8048a48
   0x0804889f <+69>: call   0x80484d0 <puts@plt>
   0x080488a4 <+74>: mov    eax,0x8048a7b
   0x080488a9 <+79>: mov    DWORD PTR [esp],eax
   0x080488ac <+82>: call   0x8048480 <printf@plt>
   0x080488b1 <+87>: mov    eax,0x8048a85
   0x080488b6 <+92>: lea    edx,[esp+0x1c]
   0x080488ba <+96>: mov    DWORD PTR [esp+0x4],edx
   0x080488be <+100>: mov    DWORD PTR [esp],eax
   0x080488c1 <+103>: call   0x8048530 <__isoc99_scanf@plt>
   0x080488c6 <+108>: mov    eax,DWORD PTR [esp+0x1c]
   0x080488ca <+112>: mov    DWORD PTR [esp+0x4],0x1337d00d
   0x080488d2 <+120>: mov    DWORD PTR [esp],eax
   0x080488d5 <+123>: call   0x8048747 <test>
   0x080488da <+128>: mov    eax,0x0
   0x080488df <+133>: leave
   0x080488e0 <+134>: ret
```

The `test` function started by subtracting the password passed to it from the second parameter `(322424845)` and checked whether the result was greater than `21`. If false, it called the `decrypt` function with the calculated result as the parameter. Otherwise, it called the `decrypt` function with the result of the `rand` function as the parameter.

```s
   0x0804874d <+6>: mov    eax,DWORD PTR [ebp+0x8]
   0x08048750 <+9>: mov    edx,DWORD PTR [ebp+0xc]
   0x08048753 <+12>: mov    ecx,edx
   0x08048755 <+14>: sub    ecx,eax
   0x08048757 <+16>: mov    eax,ecx
   0x08048759 <+18>: mov    DWORD PTR [ebp-0xc],eax
   0x0804875c <+21>: cmp    DWORD PTR [ebp-0xc],0x15
   0x08048760 <+25>: ja     0x804884a <test+259>
   0x08048766 <+31>: mov    eax,DWORD PTR [ebp-0xc]
   0x08048769 <+34>: shl    eax,0x2
   0x0804876c <+37>: add    eax,0x80489f0
   0x08048771 <+42>: mov    eax,DWORD PTR [eax]
   0x08048773 <+44>: jmp    eax
   0x08048775 <+46>: mov    eax,DWORD PTR [ebp-0xc]
   0x08048778 <+49>: mov    DWORD PTR [esp],eax
   0x0804877b <+52>: call   0x8048660 <decrypt>
   0x08048780 <+57>: jmp    0x8048858 <test+273>
--------------------------------------------------------
   0x0804884a <+259>: call   0x8048520 <rand@plt>
   0x0804884f <+264>: mov    DWORD PTR [esp],eax
   0x08048852 <+267>: call   0x8048660 <decrypt>
```

The `decrypt` function stored the string **'Q}|u\`sfg~sf{}|a3'** in a buffer and used a loop to change each character in the buffer. It did this by calculating the `XOR` operator between the parameter and the value of the current index of the buffer and using it to extract the least significant byte.

```s
   0x080486bb <+91>: mov    DWORD PTR [ebp-0x24],eax
   0x080486be <+94>: mov    DWORD PTR [ebp-0x28],0x0
   0x080486c5 <+101>: jmp    0x80486e5 <decrypt+133>
   0x080486c7 <+103>: lea    eax,[ebp-0x1d]
   0x080486ca <+106>: add    eax,DWORD PTR [ebp-0x28]
   0x080486cd <+109>: movzx  eax,BYTE PTR [eax]
   0x080486d0 <+112>: mov    edx,eax
   0x080486d2 <+114>: mov    eax,DWORD PTR [ebp+0x8]
   0x080486d5 <+117>: xor    eax,edx
   0x080486d7 <+119>: mov    edx,eax
   0x080486d9 <+121>: lea    eax,[ebp-0x1d]
   0x080486dc <+124>: add    eax,DWORD PTR [ebp-0x28]
   0x080486df <+127>: mov    BYTE PTR [eax],dl
   0x080486e1 <+129>: add    DWORD PTR [ebp-0x28],0x1
   0x080486e5 <+133>: mov    eax,DWORD PTR [ebp-0x28]
   0x080486e8 <+136>: cmp    eax,DWORD PTR [ebp-0x24]
   0x080486eb <+139>: jb     0x80486c7 <decrypt+103>
```

After that, it compared the last buffer changed with the string `"Congratulations!"`. If they were equal, the program started a new `shell` process (/bin/sh) within the current process. Otherwise, it displayed the message `"\nInvalid Password".`

```s
   0x080486ed <+141>:	lea    eax,[ebp-0x1d]
   0x080486f0 <+144>:	mov    edx,eax
   0x080486f2 <+146>:	mov    eax,0x80489c3
   0x080486f7 <+151>:	mov    ecx,0x11
   0x080486fc <+156>:	mov    esi,edx
   0x080486fe <+158>:	mov    edi,eax
   0x08048700 <+160>:	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
   0x08048702 <+162>:	seta   dl
   0x08048705 <+165>:	setb   al
   0x08048708 <+168>:	mov    ecx,edx
   0x0804870a <+170>:	sub    cl,al
   0x0804870c <+172>:	mov    eax,ecx
   0x0804870e <+174>:	movsx  eax,al
   0x08048711 <+177>:	test   eax,eax
   0x08048713 <+179>:	jne    0x8048723 <decrypt+195>
   0x08048715 <+181>:	mov    DWORD PTR [esp],0x80489d4
   0x0804871c <+188>:	call   0x80484e0 <system@plt>
   0x08048721 <+193>:	jmp    0x804872f <decrypt+207>
   0x08048723 <+195>:	mov    DWORD PTR [esp],0x80489dc
   0x0804872a <+202>:	call   0x80484d0 <puts@plt>
   0x0804872f <+207>:	mov    esi,DWORD PTR [ebp-0xc]
   0x08048732 <+210>:	xor    esi,DWORD PTR gs:0x14
   0x08048739 <+217>:	je     0x8048740 <decrypt+224>
   0x0804873b <+219>:	call   0x80484c0 <__stack_chk_fail@plt>
   0x08048740 <+224>:	add    esp,0x40
   0x08048743 <+227>:	pop    esi
   0x08048744 <+228>:	pop    edi
   0x08048745 <+229>:	pop    ebp
   0x08048746 <+230>:	ret
```

In order to solve the issue, we need to determine the appropriate number to use in the `XOR` operator with the first character of the buffer ('Q') that is stored in the `decrypt` function, so that it becomes the character 'C'. It's important to note that this number must be less than `21`.

```
0001 0010 ==> 18
0101 0001 ==> 'Q'
---------
0100 0011 ==> 'C'
```

After finding the appropriate number `(18)` to use in the `XOR` operator, we just need to subtract it from `322424845` in order to obtain the correct value to pass to the `test` function as a parameter, and that's it.

```sh
level03@OverRide:~$ python2
Python 2.7.3 (default, Jun 22 2015, 19:33:41)
[GCC 4.6.3] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>> 322424845 - 18
322424827
>>>
level03@OverRide:~$ ./level03
***********************************
*		level03		**
***********************************
Password:322424827
$ whoami
level04
$ cd ../level04
$ ls -la
total 17
dr-xr-x---+ 1 level04 level04   80 Sep 13  2016 .
dr-x--x--x  1 root    root     260 Oct  2  2016 ..
-rw-r--r--  1 level04 level04  220 Sep 10  2016 .bash_logout
lrwxrwxrwx  1 root    root       7 Sep 13  2016 .bash_profile -> .bashrc
-rw-r--r--  1 level04 level04 3533 Sep 10  2016 .bashrc
-rwsr-s---+ 1 level05 users   7797 Sep 10  2016 level04
-rw-r--r--+ 1 level04 level04   41 Oct 19  2016 .pass
-rw-r--r--  1 level04 level04  675 Sep 10  2016 .profile
```