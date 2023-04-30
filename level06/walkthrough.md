# Walkthrough

After connecting to `level06` user, we can see that we have a binary file called `level06`. After executing it, we can see that it asks me to enter a login and then a serial.

```sh
level06@OverRide:~$ ./level06 
***********************************
*  level06    *
***********************************
-> Enter Login: AAAAAA
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 1337
```

To better understand its behavior, I tried to debug it using `gdb`.

The `main` function displays some messages and then it asks me to enter a login using the `fgets` function.

```sh
   0x080488cf <+86>: mov    eax,ds:0x804a060
   0x080488d4 <+91>: mov    DWORD PTR [esp+0x8],eax
   0x080488d8 <+95>: mov    DWORD PTR [esp+0x4],0x20
   0x080488e0 <+103>: lea    eax,[esp+0x2c]
   0x080488e4 <+107>: mov    DWORD PTR [esp],eax
   0x080488e7 <+110>: call   0x8048550 <fgets@plt>
```

Then, it calls the `scanf` function to get the serial.

```s
   0x0804891d <+164>: mov    eax,0x8048a60 # "%u"
   0x08048922 <+169>: lea    edx,[esp+0x28]
   0x08048926 <+173>: mov    DWORD PTR [esp+0x4],edx
   0x0804892a <+177>: mov    DWORD PTR [esp],eax
   0x0804892d <+180>: call   0x80485e0 <__isoc99_scanf@plt>
```

After that, it calls the `auth` function and checks its return. If it's different from zero, it returns 1.

```s
   0x08048932 <+185>: mov    eax,DWORD PTR [esp+0x28]
   0x08048936 <+189>: mov    DWORD PTR [esp+0x4],eax
   0x0804893a <+193>: lea    eax,[esp+0x2c]
   0x0804893e <+197>: mov    DWORD PTR [esp],eax
   0x08048941 <+200>: call   0x8048748 <auth>
   0x08048946 <+205>: test   eax,eax
   0x08048948 <+207>: jne    0x8048969 <main+240>
   0x08048969 <+240>: mov    eax,0x1
```

Otherwise, it shows `"Authenticated!"` and then starts a new shell process `(/bin/sh)` within the current process using the `system` function.

```s
   0x0804894a <+209>: mov    DWORD PTR [esp],0x8048b52 # "Authenticated!"
   0x08048951 <+216>: call   0x8048590 <puts@plt>
   0x08048956 <+221>: mov    DWORD PTR [esp],0x8048b61
   0x0804895d <+228>: call   0x80485a0 <system@plt>
```

As for the `auth` function, we can see it takes two parameters: the first is the `login` and the second is the `serial` entered.

It starts by searching for `"\n"` in the first parameter using the `strcspn` function, which returns its index. This index is used in the first parameter, which is the login, to change it to zero.

```s
   0x0804874e <+6>: mov    DWORD PTR [esp+0x4],0x8048a63 # "\n"
   0x08048756 <+14>: mov    eax,DWORD PTR [ebp+0x8]
   0x08048759 <+17>: mov    DWORD PTR [esp],eax
   0x0804875c <+20>: call   0x8048520 <strcspn@plt>
   0x08048761 <+25>: add    eax,DWORD PTR [ebp+0x8]
   0x08048764 <+28>: mov    BYTE PTR [eax],0x0
```

Then, it calculates the length of the login and checks if its length is less than or equal to 5. If true, it returns 1.

```s
   0x08048767 <+31>: mov    DWORD PTR [esp+0x4],0x20
   0x0804876f <+39>: mov    eax,DWORD PTR [ebp+0x8]
   0x08048772 <+42>: mov    DWORD PTR [esp],eax
   0x08048775 <+45>: call   0x80485d0 <strnlen@plt>
   0x0804877a <+50>: mov    DWORD PTR [ebp-0xc],eax
   0x08048786 <+62>: cmp    DWORD PTR [ebp-0xc],0x5
   0x0804878a <+66>: jg     0x8048796 <auth+78>
   0x0804878c <+68>: mov    eax,0x1
   0x08048791 <+73>: jmp    0x8048877 <auth+303>
   0x08048877 <+303>: leave  
   0x08048878 <+304>: ret
```

Otherwise, it calls the `ptrace` function. If its return value is -1, the `auth` function returns 1.

```s
   0x08048796 <+78>: mov    DWORD PTR [esp+0xc],0x0
   0x0804879e <+86>: mov    DWORD PTR [esp+0x8],0x1
   0x080487a6 <+94>: mov    DWORD PTR [esp+0x4],0x0
   0x080487ae <+102>: mov    DWORD PTR [esp],0x0
   0x080487b5 <+109>: call   0x80485f0 <ptrace@plt>
   0x080487ba <+114>: cmp    eax,0xffffffff
   0x080487bd <+117>: jne    0x80487ed <auth+165>
   0x080487bf <+119>: mov    DWORD PTR [esp],0x8048a68
   0x080487c6 <+126>: call   0x8048590 <puts@plt>
   0x080487cb <+131>: mov    DWORD PTR [esp],0x8048a8c
   0x080487d2 <+138>: call   0x8048590 <puts@plt>
   0x080487d7 <+143>: mov    DWORD PTR [esp],0x8048ab0
   0x080487de <+150>: call   0x8048590 <puts@plt>
   0x080487e3 <+155>: mov    eax,0x1
   0x080487e8 <+160>: jmp    0x8048877 <auth+303>
   0x08048877 <+303>: leave  
   0x08048878 <+304>: ret
```

Otherwise, it continues. Then, it does some operation and stores its result.

```s
   0x080487ed <+165>: mov    eax,DWORD PTR [ebp+0x8]
   0x080487f0 <+168>: add    eax,0x3
   0x080487f3 <+171>: movzx  eax,BYTE PTR [eax]
   0x080487f6 <+174>: movsx  eax,al
   0x080487f9 <+177>: xor    eax,0x1337
   0x080487fe <+182>: add    eax,0x5eeded
   0x08048803 <+187>: mov    DWORD PTR [ebp-0x10],eax
   0x08048806 <+190>: mov    DWORD PTR [ebp-0x14],0x0
   0x0804880d <+197>: jmp    0x804885b <auth+275>
   0x0804880f <+199>: mov    eax,DWORD PTR [ebp-0x14]
   0x08048812 <+202>: add    eax,DWORD PTR [ebp+0x8]
   0x08048815 <+205>: movzx  eax,BYTE PTR [eax]
   0x08048818 <+208>: cmp    al,0x1f
   0x0804881a <+210>: jg     0x8048823 <auth+219>
   0x0804881c <+212>: mov    eax,0x1
   0x08048821 <+217>: jmp    0x8048877 <auth+303>
   0x08048823 <+219>: mov    eax,DWORD PTR [ebp-0x14]
   0x08048826 <+222>: add    eax,DWORD PTR [ebp+0x8]
   0x08048829 <+225>: movzx  eax,BYTE PTR [eax]
   0x0804882c <+228>: movsx  eax,al
   0x0804882f <+231>: mov    ecx,eax
   0x08048831 <+233>: xor    ecx,DWORD PTR [ebp-0x10]
   0x08048834 <+236>: mov    edx,0x88233b2b
   0x08048839 <+241>: mov    eax,ecx
   0x0804883b <+243>: mul    edx
   0x0804883d <+245>: mov    eax,ecx
   0x0804883f <+247>: sub    eax,edx
   0x08048841 <+249>: shr    eax,1
   0x08048843 <+251>: add    eax,edx
   0x08048845 <+253>: shr    eax,0xa
   0x08048848 <+256>: imul   eax,eax,0x539
   0x0804884e <+262>: mov    edx,ecx
   0x08048850 <+264>: sub    edx,eax
   0x08048852 <+266>: mov    eax,edx
   0x08048854 <+268>: add    DWORD PTR [ebp-0x10],eax
   0x08048857 <+271>: add    DWORD PTR [ebp-0x14],0x1
   0x0804885b <+275>: mov    eax,DWORD PTR [ebp-0x14]
   0x0804885e <+278>: cmp    eax,DWORD PTR [ebp-0xc]
   0x08048861 <+281>: jl     0x804880f <auth+199>
```

Finally, it checks if this result is equal to the serial passed in the `auth` function as the second argument. If they are equal, it returns 0. Otherwise, it returns 1.

```s
   0x08048863 <+283>: mov    eax,DWORD PTR [ebp+0xc]
   0x08048866 <+286>: cmp    eax,DWORD PTR [ebp-0x10]
   0x08048869 <+289>: je     0x8048872 <auth+298>
   0x0804886b <+291>: mov    eax,0x1
   0x08048870 <+296>: jmp    0x8048877 <auth+303>
   0x08048872 <+298>: mov    eax,0x0
   0x08048877 <+303>: leave  
   0x08048878 <+304>: ret 
```

If we analyze the program, we can see that we only need to provide a login with a length greater than 5 and a serial number, and then examine the result of the last condition checked in the `auth` function. Then, we can simply relaunch the program using the same login and update the serial number with the last result stored in the `auth` function. To do this, we can set a breakpoint at `0x080487ba` to change the value of the `eax` register, which will be different from zero because we are running the program with `gdb`. After that, we can set another breakpoint at `0x08048866` to view the value of `ebp-0x10`, which we can use as the serial number.

```
(gdb) b *0x080487ba
Breakpoint 6 at 0x80487ba
(gdb) b *0x08048866
Breakpoint 7 at 0x8048866
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/users/level06/level06 
***********************************
*  level06    *
***********************************
-> Enter Login: AAAAAA
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 1337
(gdb) set $eax=1
(gdb) c
Continuing.
(gdb) x/d $ebp-0x10
0xffffd5a8: 6229050

--------------------------------------------------------------

level06@OverRide:~$ ./level06 
***********************************
*		level06		  *
***********************************
-> Enter Login: AAAAAA
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 6229050
Authenticated!
$ whoami
level07
$ cd ../level07	
$ ls -la
total 21
dr-xr-x---+ 1 level07 level07    80 Sep 13  2016 .
dr-x--x--x  1 root    root      260 Oct  2  2016 ..
-rw-r--r--  1 level07 level07   220 Sep 10  2016 .bash_logout
lrwxrwxrwx  1 root    root        7 Sep 13  2016 .bash_profile -> .bashrc
-rw-r--r--  1 level07 level07  3533 Sep 10  2016 .bashrc
-rw-r--r--+ 1 level07 level07    41 Oct 19  2016 .pass
-rw-r--r--  1 level07 level07   675 Sep 10  2016 .profile
-rwsr-s---+ 1 level08 users   11744 Sep 10  2016 level07
```
