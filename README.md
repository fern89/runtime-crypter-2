# Instruction-level runtime crypter POC
Basic POC for instruction-level runtime crypting. By abusing pageguard VEH hooking, we are able to make it so that a maximum of 16 bytes of unencrypted code (max asm instruction length + 1 byte) exists at any point in time in the memory. This can be reduced to single asm instructions, but it would be too complicated to implement in this basic POC. Causes EXTREME performance loss in code, demo code slows down ~6000x compared to base level for unobfuscated code. Built for x64, but should be pretty easy to convert to x86.

## Update
I originally removed this repo as it was intended as a solution against dynamic reversing, but then I realized you could just have it break on every exception, leading to it doing the step-by-step execution FOR the debugger, making it essentially useless. But then I came across https://github.com/lem0nSec/ShellGhost, and realized this has some use against scanners like Moneta, so I republish it.

### Differences from ShellGhost
- Does not use breakpoints, uses trap flag instead
- As a consequence, uses guard pages to not step kernel32/other win32 dlls
- Decrypts 16 bytes at a time instead of 1 calculated instruction (simplifies code greatly)
