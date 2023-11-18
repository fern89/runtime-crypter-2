#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
//force code to diff page
#define BEGIN_ENC asm(".fill 1500, 1, 0x90"); void begin(){}
#define END_ENC asm(".fill 50, 1, 0x90"); void end(){}
void* pp=0;
int sz=0;
void begin();
void end();
unsigned char keys[10000]={0};
size_t bigness;
void populate(){
    for(int i=0;i<10000;i++) keys[i]=rand();
}
//ensure code and veh hooks diff page
int pagecheck(void* Addr1, void* Addr2){
	MEMORY_BASIC_INFORMATION mbi1;
	if (!VirtualQuery(Addr1, &mbi1, sizeof(mbi1)))
		return 1;
	MEMORY_BASIC_INFORMATION mbi2;
	if (!VirtualQuery(Addr2, &mbi2, sizeof(mbi2)))
		return 1;
	if (mbi1.BaseAddress == mbi2.BaseAddress)
		return 1;
	return 0; 
}
void fenc(){
    unsigned char* e = (unsigned char*)end;
    DWORD dwOld;
    VirtualProtect(begin, bigness, PAGE_READWRITE, &dwOld);
    unsigned char* s = (unsigned char*)begin;
    while(s<e){
        s[0]^=keys[(((long long)s)-(long long)begin)/10]; //diff key every 10 bytes
        s++;
    }
    VirtualProtect(begin, bigness, PAGE_EXECUTE_READ, &dwOld);
}
LONG WINAPI hand(EXCEPTION_POINTERS *pExceptionInfo){
    if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION){
        void* rip = (void*)(pExceptionInfo->ContextRecord->Rip);
        DWORD dwOld;
        pp = rip;
        if(rip < (void*)end && rip >= (void*)begin){
            VirtualProtect((LPVOID)begin, bigness, PAGE_READWRITE, &dwOld);
            unsigned char* codex = rip;
            for(int i=0; i<16; i+=1){
                codex[i]^=keys[(((long long)codex+i)-(long long)begin)/10]; //decrypt
            }
            VirtualProtect((LPVOID)begin, bigness, PAGE_EXECUTE_READ, &dwOld);
        }
        pExceptionInfo->ContextRecord->EFlags |= 0x100; //allow 1 step to happen
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP){
        DWORD dwOld;
        if(pp < (void*)end && pp >= (void*)begin){
            VirtualProtect((LPVOID)begin, bigness, PAGE_READWRITE, &dwOld);
            unsigned char* code = pp;
            for(int i=0; i<16; i+=1){
                code[i]^=keys[(((long long)code+i)-(long long)begin)/10]; //encrypt
            }
        }
        VirtualProtect((LPVOID)begin, bigness, PAGE_EXECUTE_READ | PAGE_GUARD, &dwOld);
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}
int hook(){
    srand(time(NULL));
    populate();
    bigness=(long long)end-(long long)begin;
    if(pagecheck(begin, hand)){
        printf("error, same page!!!\n");
        return 0;
    }
    fenc();
    PVOID VEH_Handle = AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)hand);
    DWORD oldProtection = 0;
    if(VEH_Handle && VirtualProtect((LPVOID)begin, bigness, PAGE_EXECUTE_READ | PAGE_GUARD, &oldProtection))
        return 1;
    return 0;
}
