//#include "math.h"
//#include <csignal>
//#include <iomanip>
#include "runtime.h"
#include <cmath>

extern "C"
{
    #include "shutdown.h"
}

#define S(x) # x

using namespace std;

//vector<uint> guards;

#if JIT_LEVEL == JIT_DISABLED
rtinternal findInternal(Runtime *rt, Function* f)
{
    switch (f->sign[0]) {
        case 'a':
            if(!strcmp(f->sign, "atod"))
            {
                if(f->argc == 1)
                    return &internalAtod;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "atoi"))
            {
                if(f->argc == 1)
                    return &internalAtoi;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
        case 'c':
            if(!strcmp(f->sign, "count"))
            {
                if(f->argc == 1)
                    return &internalCount;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
        case 'd':
            if(!strcmp(f->sign, "dtoa"))
            {
                if(f->argc == 1)
                    return &internalDtoa;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
        case 'g':
            if(!strcmp(f->sign, "getdebuginfo"))
            {
                if(f->argc == 1)
                    return &internalDebug;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "getName"))
            {
                if(f->argc == 1)
                    return &internalGetName;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "getText"))
            {
                if(f->argc == 1)
                    return &internalGetText;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "getColor"))
            {
                if(f->argc == 1)
                    return &internalGetColor;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "getTextColor"))
            {
                if(f->argc == 1)
                    return &internalGetTextColor;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "getPos"))
            {
                if(f->argc == 1)
                    return &internalGetPos;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
        case 'f':
            if(!strcmp(f->sign, "fabs"))
            {
                if(f->argc == 1)
                    return &internalFabs;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "findElement"))
            {
                if(f->argc == 1)
                    return &internalFindElement;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
        case 'e':
            if(!strcmp(f->sign, "exit"))
            {
                if(f->argc == 0 || (f->argc == 1 && (Type)*f->args[f->argc-1].type == Type::I32))
                    return &internalExit;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "exp"))
            {
                if(f->argc == 0 || (f->argc == 1 && (Type)*f->args[f->argc-1].type == Type::DOUBLE))
                    return &internalExp;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "endGuard"))
            {
                if(f->argc == 0)
                    return &internalEndGuard;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "exec"))
            {
                if(f->argc == 0)
                    return &internalExec;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
        case 'l':
            if(!strcmp(f->sign, "loadUI"))
            {
                if(f->argc == 1)
                    return &internalLoadUI;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
        case 'i':
            if(!strcmp(f->sign, "init"))
            {
                if(f->argc == 2)
                    return &internalInit;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "itoa"))
            {
                if(f->argc == 1)
                    return &internalItoa;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
        case 'p': //P for PRINT
            if(!strcmp(f->sign, "print"))
            {
                Type t = (Type)*f->args[f->argc-1].type;
                if(t == Type::DOUBLE || t == Type::I32 || t == Type::UI32
                        || t == Type::UTF8 || t == Type::CHAR)
                    return &internalPrint;
                else
                    rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "printStackTrace") && f->argc == 0)
            {
                return &internalPrintStackTrace;
            }
            if(!strcmp(f->sign, "pow") && f->argc == 2)
            {
                Type t1 = (Type)*f->args[0].type;
                Type t2 = (Type)*f->args[1].type;
                if(t1 == Type::DOUBLE && t2 == Type::I32)
                    return &internalPow;
                else
                    rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            break;
        case 'P':
            if(!strcmp(f->sign, "Pi") && f->argc == 0)
            {
                return &internalPi;
            }
            rt->rtThrow(Runtime::InternalFunctionMissing);
        case 'r': //R for READ
            if(!memcmp(f->sign, "read", 4) && f->argc == 0) /// @badcode FIXME
                return &internalRead;
            else if(memcmp(f->sign, "raise", 5) && f->argc == 1)
                return &internalRaise;            
            else if(memcmp(f->sign, "rand", 4) && f->argc == 0)
                return &internalRand;
            else
                rt->rtThrow(Runtime::InternalFunctionMissing);
        case 't':
            if(!strcmp(f->sign, "terminate"))
            {
                if(f->argc == 0)
                    return &internalTerminate;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
        case 's':
            if(!strcmp(f->sign, "sqrt"))
            {
                if(f->argc == 1)
                    return &internalSqrt;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "splitToChars"))
            {
                if(f->argc == 1)
                    return &internalSplitToChars;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "startGuard"))
            {
                if(f->argc == 0)
                    return &internalStartGuard;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "setText"))
            {
                if(f->argc == 2)
                    return &internalSetText;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "setColor"))
            {
                if(f->argc == 2)
                    return &internalSetColor;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "setTextColor"))
            {
                if(f->argc == 2)
                    return &internalSetTextColor;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "setPos"))
            {
                if(f->argc == 3)
                    return &internalSetPos;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            /*
            void internalAtod(Runtime* rt, Function *f, byte* fargs);
            void internalAtoi(Runtime* rt, Function *f, byte* fargs);
            void internalDtoa(Runtime* rt, Function *f, byte* fargs);
            void internalExec(Runtime* rt, Function *f, byte* fargs);
            void internalFindElement(Runtime* rt, Function *f, byte* fargs);
            void internalGetName(Runtime* rt, Function *f, byte* fargs);
            void internalGetColor(Runtime* rt, Function *f, byte* fargs);
            void internalGetTextColor(Runtime* rt, Function *f, byte* fargs);
            void internalGetPos(Runtime* rt, Function *f, byte* fargs);
            void internalLoadUI(Runtime* rt, Function *f, byte* fargs);
            void internalInit(Runtime* rt, Function *f, byte* fargs);
            void internalItoa(Runtime* rt, Function *f, byte* fargs);
            void internalSetText(Runtime* rt, Function *f, byte* fargs);
            void internalSetColor(Runtime* rt, Function *f, byte* fargs);
            void internalSetTextColor(Runtime* rt, Function *f, byte* fargs);
            void internalSetPos(Runtime* rt, Function *f, byte* fargs);
            */
        default:
            rt->rtThrow(Runtime::InternalFunctionMissing);
            break;
    }
    return nullptr;
}
#else
rtinternal findInternal(Runtime *rt, Function* f)
{
    switch (f->sign[0]) {
        case 'c':
            if(!strcmp(f->sign, "count"))
            {
                if(f->argc == 1)
                    return &internalCount;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
        case 'g':
            if(!strcmp(f->sign, "getdebuginfo"))
            {
                if(f->argc == 1)
                    return &internalDebug;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
        case 'e':
            if(!strcmp(f->sign, "exit"))
            {
                if(f->argc == 0 || (f->argc == 1 && (Type)*f->args[f->argc-1].type == Type::I32))
                    return &internalExit;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "exp"))
            {
                if(f->argc == 0 || (f->argc == 1 && (Type)*f->args[f->argc-1].type == Type::DOUBLE))
                    return &internalExp;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "endGuard"))
            {
                if(f->argc == 0)
                    return &internalEndGuard;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
        case 'p': //P for PRINT
            if(!strcmp(f->sign, "printHex"))
            {
                Type t = (Type)*f->args[f->argc-1].type;
                if(t == Type::I32 || t == Type::UI32 || t == Type::CHAR)
                    return &internalPrintHex;
                else
                    rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "print"))
            {
                Type t = (Type)*f->args[f->argc-1].type;
                if(t == Type::DOUBLE || t == Type::I32 || t == Type::UI32
                        || t == Type::UTF8 || t == Type::CHAR)
                    return &internalPrint;
                else
                    rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "printStackTrace") && f->argc == 0)
            {
                return &internalPrintStackTrace;
            }
            if(!strcmp(f->sign, "pow") && f->argc == 2)
            {
                Type t1 = (Type)*f->args[0].type;
                Type t2 = (Type)*f->args[1].type;
                if(t1 == Type::DOUBLE && t2 == Type::I32)
                    return &internalPow;
                else
                    rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            break;
        case 'P':
            if(!strcmp(f->sign, "Pi") && f->argc == 0)
            {
                return &internalPi;
            }
            rt->rtThrow(Runtime::InternalFunctionMissing);
        case 'r': //R for READ
            if(strstr(f->sign, "read") && f->argc == 0) /// @badcode FIXME
                return &internalRead;
            else if(strstr(f->sign, "raise") && f->argc == 1)
                return &internalRaise;
            else if(strstr(f->sign, "rand") && f->argc == 0)
                return &internalRand;
            else
                rt->rtThrow(Runtime::InternalFunctionMissing);
        case 't':
            if(!strcmp(f->sign, "terminate"))
            {
                if(f->argc == 0)
                    return &internalTerminate;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
        case 's':
            if(!strcmp(f->sign, "sqrt"))
            {
                if(f->argc == 1)
                    return &internalSqrt;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "splitToChars"))
            {
                if(f->argc == 1)
                    return &internalSplitToChars;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
            if(!strcmp(f->sign, "startGuard"))
            {
                if(f->argc == 0)
                    return &internalStartGuard;
                rt->rtThrow(Runtime::InternalFunctionMissing);
            }
        default:
            rt->rtThrow(Runtime::InternalFunctionMissing);
            break;
    }
    return nullptr;
}
#endif

void internalPrint(Runtime* rt, Function *f, byte* fargs)
{
    Type arg = (Type)*f->args[f->argc-1].type;
    switch (arg) {
        case Type::UTF8:
            {
                if(*(size_t*)fargs == 0)
                {
                    cout << "{null}";
                    return;
                }
                char* str = (char*)(*(size_t*)fargs + rt->ARRAY_METADATA_SIZE);
                cout << str;
            }
            break;
        case Type::I32:
            cout << *(int*)fargs;
            break;
        case Type::UI32:
            cout << *(uint*)fargs;
            break;
        case Type::DOUBLE:            
            cout << *(double*)fargs;
            break;
        case Type::CHAR:
            cout << *(char*)fargs;
            break;
        default:
            break;
    }
}

void internalPrintHex(Runtime* rt, Function *f, byte* fargs)
{
    internalPrint(rt, f, fargs);
    /*Type arg = (Type)*f->args[f->argc-1].type;
    switch (arg) {
        case Type::I32:
            cout << hex << setfill('0') << setw(8) << *(int*)fargs << dec;
            break;
        case Type::UI32:
            cout << hex << setfill('0') << setw(8) << *(uint*)fargs << dec;
            break;
        case Type::CHAR:
            cout << hex << setfill('0') << setw(8) << *(char*)fargs << dec;
            break;
        default:
            break;
    }*/
}

void internalExit(Runtime* rt, Function *f, byte* fargs)
{
    if(f->argc == 0)
    {
        rt->Unload();
        rt->returnCode = 0;
    }
    else
    {
        rt->returnCode = *(int*)fargs;
        rt->Unload();
    }
    //exit(rt->GetReturnCode());
    panic("exited with code:", 2, rt->GetReturnCode());
}

void internalTerminate(Runtime *rt, Function *f, byte *fargs) //Checked
{
    //abort();
    panic("aborted", 2, -1);
    //terminate();
}

void internalRaise(Runtime* rt, Function* f, byte* fargs) //Checked
{
    /*string sig = (char*)(*(size_t*)fargs + rt->ARRAY_METADATA_SIZE);
    if(sig == "SIGSEGV")
        raise(SIGSEGV);
    else if(sig == "SIGKILL")
        raise(SIGKILL);
    else if(sig == "SIGTERM")
        raise(SIGTERM);
    else if(sig == "SIGABRT")
        raise(SIGABRT);*/
    cerr << "raise: not implemented" << endl;

    return;
}

void internalRead(Runtime* rt, Function *f, byte* fargs) //Checked
{
    //allocMem(len);

    //strcpy((char*)mem_first_free, ptr);

    //stackalloc(5);
    Type ret = (Type)*f->ret;
    switch (ret) {
        case Type::UTF8:
        {
            //*(size_t*)++rt->stack_ptr = (size_t)(char*)rt->mem_first_free;

            char buf[1024];

            cin >> buf;//(char*)(rt->mem_first_free + rt->ARRAY_METADATA_SIZE);
            uint len = strlen(buf)+1;
            byte* addr = rt->memoryManager.Allocate(len + rt->ARRAY_METADATA_SIZE);
            *addr = (byte)Type::UTF8;
            *(uint*)(addr+1) = len;
            *(size_t*)++rt->stack_ptr = (uint)addr;
            strcpy((char*)(addr + rt->ARRAY_METADATA_SIZE), buf);

            //rt->mem_first_free += strlen((char*)rt->mem_first_free) + 1 + rt->ARRAY_METADATA_SIZE;

            rt->stack_ptr += 4;
            *rt->stack_ptr = (byte)Type::UTF8;
        }
            break;
        case Type::I32:
            cin >> *(int*)++rt->stack_ptr;

            rt->stack_ptr += 4;
            *rt->stack_ptr = (byte)Type::I32;
            break;
        case Type::UI32:
            cin >> *(uint*)++rt->stack_ptr;

            rt->stack_ptr += 4;
            *rt->stack_ptr = (byte)Type::UI32;
            break;
        case Type::CHAR:
            cin >> *(char*)++rt->stack_ptr;

            rt->stack_ptr += 4;
            *rt->stack_ptr = (byte)Type::CHAR;
            break;
        case Type::DOUBLE:
            //cerr << "readd: not implemented" << endl;
            cin >> *(double*)++rt->stack_ptr;

            rt->stack_ptr += 8;
            *rt->stack_ptr = (byte)Type::DOUBLE;
            break;
        default:
            break;
    }
}

void internalSqrt(Runtime* rt, Function *f, byte* fargs) //Checked
{
    //panic("sqrt: not implemented", 2, -1);
    //return;
    double d = *(double*)fargs;

    *(double*)++rt->stack_ptr = sqrt(d);

    rt->stack_ptr += 8;
    *rt->stack_ptr = (byte)Type::DOUBLE;
}

void internalFabs(Runtime* rt, Function *f, byte* fargs) //Checked
{
    double d = *(double*)fargs;

    *(double*)++rt->stack_ptr = fabs(d);

    rt->stack_ptr += 8;
    *rt->stack_ptr = (byte)Type::DOUBLE;
}

void internalCount(Runtime* rt, Function *f, byte* fargs) //Checked
{
    byte* addr = (byte*)*(uint*)fargs;

    *(uint*)++rt->stack_ptr = *(uint*)(addr+1);

    rt->stack_ptr += 4;
    *rt->stack_ptr = (byte)Type::UI32;
}

void internalDebug(Runtime* rt, Function *f, byte* fargs) //Checked
{
    rt->log.SetType(Log::Info);
    char* key = (char*)(*(size_t*)fargs + rt->ARRAY_METADATA_SIZE);
    if(!strcmp(key, "all"))
    {
        rt->log << "Toolchain version: " << TOOLCHAIN_VERSION << "\n";
        rt->log << "Executable name: kernel\n";
        rt->log << "Internal structs:\n"
             << "\t" << S(Function) << ": " << sizeof(Function) << " bytes\n"
             << "\t" << S(OpCode) << ": " << sizeof(OpCode) << " bytes\n"
             << "\t" << S(Log) << ": " << sizeof(Log) << " bytes\n"
             << "\t" << S(Runtime) << ": " << sizeof(Runtime) << " bytes\n"
             << "\t" << S(Runtime::MemoryManager) << ": " << sizeof(Runtime::MemoryManager) << " bytes\n"
             << "\t" << S(Type) << ": " << sizeof(Type) << " bytes\n"
             << "\t" << S(LocalVar) << ": " << sizeof(LocalVar) << " bytes\n"
             << "\t" << S(GlobalVar) << ": " << sizeof(GlobalVar) << " bytes\n"
             << "\t" << S(Header) << ": " << sizeof(Header) << " bytes\n"
             << "\t" << S(Metadata) << ": " << sizeof(Metadata) << " bytes\n";
        rt->log << "Memory:\n"
             << "\tHeap used: " << (uint)(rt->memoryManager.mem_l1_ptr - rt->memoryManager.memory_l1) << "\n"
             << "\tStack used: " << (uint)(rt->stack_ptr - rt->program_stack) << "\n"
#ifdef FW_DEBUG
             << "\tAlloc counts: " << rt->memoryManager.alloc_count << ", gc count: " << rt->memoryManager.gc_count << "\n"
#endif
             ;
        rt->printStackTrace();
        rt->log << "Loaded modules:\n";
        rt->log << "Entry module:\n" << modinfo(&rt->main_module);
        rt->log << "Loaded modules:\n";
        for(Module* mod : rt->imported)
        {
            rt->log << modinfo(mod);
        }
    }
}

string modinfo(Module* mod)
{
    string res = "";
    res += mod->file + "\n"
            + "Headers:\n";
    for(Header& h : mod->headers)
    {
        res += "\t" + h.name + "\n";
    }

    return res;
}

void internalSplitToChars(Runtime* rt, Function *f, byte* fargs) //Checked
{
    byte* addr = (byte*)*(uint*)fargs;
    uint size = *(uint*)(addr+1) + rt->ARRAY_METADATA_SIZE-1;

    byte* array = rt->memoryManager.Allocate(size);

    memcpy(array, addr, size);

    *array = (byte)Type::CHAR;
    *(uint*)++rt->stack_ptr = (uint)array;
    rt->stack_ptr += 4;
    *rt->stack_ptr = (byte)Type::ARRAY;

}

void internalStartGuard(Runtime* rt, Function *f, byte* fargs) //Checked
{
    //guards.push_back((uint)rt->stack_ptr);
}

void internalEndGuard(Runtime* rt, Function *f, byte* fargs) //Checked
{
    //rt->log << "Guard #" << (guards.size()-1) << ": " << (uint)(rt->stack_ptr - guards[guards.size()-1]) << "\n";
    //guards.pop_back();
}

void internalExp(Runtime* rt, Function *f, byte* fargs) //Checked
{
    panic("exp: not implemented", 2, -1);
    return;
    /*double d = *(double*)fargs;

    *(double*)++rt->stack_ptr = exp(d);

    rt->stack_ptr += 8;
    *rt->stack_ptr = (byte)Type::DOUBLE;*/
}

void internalPi(Runtime* rt, Function *f, byte* fargs) //Checked
{
    *(double*)++rt->stack_ptr = 3.14159265359;

    rt->stack_ptr += 8;
    *rt->stack_ptr = (byte)Type::DOUBLE;
}

void internalRand(Runtime* rt, Function *f, byte* fargs) //Checked
{
    //panic("rand: not implemented", 2, -1);
    //rand();
    //return;
    *(int*)++rt->stack_ptr = rand();

    rt->stack_ptr += 4;
    *rt->stack_ptr = (byte)Type::I32;
}

void internalPow(Runtime* rt, Function *f, byte* fargs) //Checked
{
    panic("pow: not implemented", 2, -1);
    return;
    /*double d = *(double*)fargs;
    int p = *(double*)(fargs + 9);

    *(double*)++rt->stack_ptr = pow(d, p);

    rt->stack_ptr += 8;
    *rt->stack_ptr = (byte)Type::DOUBLE;*/
}

void internalPrintStackTrace(Runtime* rt, Function *f, byte* fargs)
{
    rt->printStackTrace();
}

void internalCrash(Runtime* rt, Function *f, byte* fargs)
{
    rt->Crash();
}

void internalItoa(Runtime* rt, Function *f, byte* fargs)
{
    int number = *(int*)fargs;

    *(uint*)++rt->stack_ptr = (uint)rt->memoryManager.AllocateString(itoa(number));
    rt->stack_ptr += 4;
    *rt->stack_ptr = (byte)Type::UTF8;
}

void internalDtoa(Runtime* rt, Function *f, byte* fargs)
{
    double number = *(double*)fargs;

    *(uint*)++rt->stack_ptr = (uint)rt->memoryManager.AllocateString(dtoa(number));
    rt->stack_ptr += 4;
    *rt->stack_ptr = (byte)Type::UTF8;
}

void internalAtoi(Runtime* rt, Function *f, byte* fargs)
{
    char* number = (char*)(*(size_t*)fargs + rt->ARRAY_METADATA_SIZE);

    *(int*)++rt->stack_ptr = atoi(number);
    rt->stack_ptr += 4;
    *rt->stack_ptr = (byte)Type::I32;
}

void internalAtod(Runtime* rt, Function *f, byte* fargs)
{
    char* number = (char*)(*(size_t*)fargs + rt->ARRAY_METADATA_SIZE);

    *(double*)++rt->stack_ptr = atof(number);
    rt->stack_ptr += 8;
    *rt->stack_ptr = (byte)Type::DOUBLE;
}

void internalReboot(Runtime* rt, Function *f, byte* fargs)
{
    reboot();
}

void internalShutdown(Runtime* rt, Function *f, byte* fargs)
{
    shutdown();
}

/*void internalWm(Runtime* rt, Function *f, byte* fargs)
{
    Type arg = (Type)*f->args[f->argc-1].type;

    uint addr = *(uint*)fargs;

    switch (arg) {
        case Type::UI32:
            *(uint*)addr = *(uint*)(fargs-4);
            break;
        case Type::I32:
            *(int*)addr = *(int*)(fargs-4);
            break;
        case Type::I16:
            *(int16_t*)addr = *(int16_t*)(fargs-4);
            break;
        case Type::UI8:
            *(byte*)addr = *(byte*)(fargs-4);
            break;
        default:
            break;
    }
}*/
