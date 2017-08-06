/*
          __________    _______     ___
         /  _____   /  /  __   /   /  /
        /  /    /  /  /  /__/ /   /  /
       /  /    /  /  /  _____/   /  /
      /  /    /  /  /  /        /  /
     /  /    /  /  /  /        /  /
    /__/    /__/  /__/        /____

   _________    _______    __     ___    ______     _____
  |   ___   |  |   __  |  |  |   /   |  |   __ \
  |  |   |  |  |  |__| |  |  |  /    |  |  |__| |   |  |___
  |  |   |  |  |   ____|  |  | /  /  |  |   ___ \   |   ___
  |  |   |  |  |  |       |  |/  /|  |  |  |   | |  |   ___|
  |  |   |  |  |  |       |  |  / |  |  |  |___| |  |  |___
  |__|   |__|  |__|       |____/  |__|  |________/  |______|





 */

#include <cstring>
#include <new>
#include "runtime.h"

Module::Module(Runtime *rt)
{
    this->rt = rt;
    headers.reserve(3);
}
string Module::GetFile() const
{
    return file;
}

Module::Module()
{
    //headers.reserve(3);
}

void Module::Unload()
{
    //Function* onunload = moduleEvents[Module::OnUnload];
    //if(onunload != nullptr)
    //{
    //    byte* locals = new byte [onunload->local_mem_size];
    //    rt->execFunction(moduleEvents[Module::OnUnload], nullptr, locals);
    //    delete locals;
    //}
    if(functions != nullptr)
    {
        for(uint i = 0; i < func_count; ++i)
        {
            if(functions[i] != nullptr)
            {
                Function* f = functions[i];
                //freeArrAndNull(functions[i]->locals);
                if(functions[i]->locals != nullptr)
                {
                    kfree(functions[i]->locals);
                    functions[i]->locals = nullptr;
                }
                if(!(functions[i]->flags & FFlags::RTINTERNAL))
                {
                    if(functions[i]->bytecode != nullptr)
                    {
                        kfree(functions[i]->bytecode);
                        functions[i]->bytecode = nullptr;
                    }
                }
                //delete [] functions[i]->locals;
                //delete [] functions[i]->bytecode;
                //delete functions[i];
                //functions[i] = nullptr;
            }
        }
    }
    if(globals != nullptr)
    {
        kfree(globals);
        globals = nullptr;
    }
    if(functions != nullptr)
    {
        kfree(functions);
        functions = nullptr;
    }
    if(types != nullptr)
    {
        kfree(types);
        types = nullptr;
    }
    if(strings != nullptr)
    {
        kfree(strings);
        strings = nullptr;
    }
    //freeArrAndNull(functions);
    //delete [] functions;
}

void Module::Load(const string& f)
{
    cout << "module: " << f << endl;

    char file[255];
    strcpy(file, f.c_str()+1);

    this->file = file;
    //cout << "  copied\n";

    ifstream ifs(f.c_str());
    //cout << "  ifed\n";
    if(!ifs)
        rt->rtThrow(Runtime::InvalidModule, "no such file `" + this->file + "`");

    uint mg;

    ifs.read((char*)&mg, 4);
    if(!IsValid(mg))
        rt->rtThrow(Runtime::InvalidModule, "file `" + this->file + "` is not valid ulang module");
    //if(mg != MOD_MAGIC)
    //    throw InvalidMagicException(mg);
    uint headers_end;
    ifs.read((char*)&headers_end, 4);
    ifs.read((char*)&mflags, sizeof(ModuleFlags));

    readSegmentHeaders(ifs, headers_end);
    //cout << "Headers loaded\n";
    //while (true);
    readSegments(ifs);
    //cout << "Segments loaded\n";

    if(!mflags.no_globals_bit)
    {
        if(__global_constructor__ == nullptr)
            rt->rtThrow(Runtime::MissingGlobalConstructor);
        rt->execFunction(__global_constructor__, nullptr, nullptr);
    }

    ifs.close();
}

void Module::readSegmentHeaders(ifstream &ifs, uint hend) {
    //cout << "Base loaded\n";
    while(ifs.tellg() != hend) {
        //cout << ifs.tellg() << endl;
        Header h;

        char segName[80];
        char* ptr = &segName[0];
        while(true) {
            *ptr = ifs.get();
            if(*ptr == '\0') break;
            ptr++;
        }
        h.type = (Header::Type)(byte)ifs.get();
        ifs.read((char*)&h.begin, 4);
        ifs.read((char*)&h.end, 4);

        h.name = segName;
        //cout << "{";
        this->headers.push_back(h);
        //cout << "}";
    }
}

void Module::readSegments(ifstream &ifs) {
    for(uint i = 0; i < headers.size(); ++i) {
        Header &h = headers[i];
        //cout << "Found header: " << h.name << endl;
        switch (h.type) {
            case Header::Types:
            {
                //cout << "Type segment\n";
                ifs.seekg(h.begin);
                uint size = h.end - h.begin;
                //types = rt->allocator.RawAllocate(size);
                types = (byte*)kmalloc(size); /// #OLDNEW
                ifs.read((char*)types, size);
            }
                break;
            case Header::Functions:
                {
                    //cout << "Function segment\n";
                    ifs.seekg(h.begin);
                    //uint fcount;
                    ifs.read((char*)&this->func_count, 4);
                    rt->function_list.reserve(func_count);

                    //this->functions = rt->allocator.Allocate<Function*>(this->func_count);
                    this->functions = (Function**)kmalloc(sizeof(Function**)*this->func_count); //#OLDNEW

                    uint fc = 0;

                    while (ifs.tellg() != h.end) {
                        //if(this->file != "/kernel.sem")
                        //    printf("Function (%d, %d): ", ifs.tellg(), h.end);
                        //cout << "Function: ";
                        this->functions[fc] = new Function;//(Function*)kmalloc(sizeof(Function)); //(Function*)kmalloc(sizeof(Function)); //#OLDNEW
                        Function* fun = this->functions[fc];

                        ifs.read((char*)&fun->ret, 4);
                        fun->ret += (uint)types;

                        ifs.read(fun->sign, SIGN_LENGTH, '\0');//.ignore();
                        //if(this->file != "/kernel.sem")
                        //    cout << fun->sign << "\n";
                        uint idx;
                        ifs.read((char*)&idx, 4);
                        uint arg_addr = 0;
                        fun->argc = 0;
                        while(idx != 0xFFFFFFFF) {
                            fun->args[fun->argc].type = types + idx;
                            fun->args[fun->argc].addr = arg_addr;
                            arg_addr += Runtime::Sizeof(fun->args[fun->argc].type) + 1; /// TODO: FIX
                            ++fun->argc;
                            //ch = ifs.get(); // <--- OLD
                            ifs.read((char*)&idx, 4);
                        }
                        fun->args_size = arg_addr;

                        fun->isPrivate = (bool)ifs.get();
                        fun->flags = (byte)ifs.get();
                        //cout << "--next\n";
                        if(fun->flags & FFlags::IMPORTED) {
                            //cout << "  imported\n";
                            char mod[128];
                            ifs.read(mod, 128, '\0');//.ignore();
                            for(Module* m : rt->imported)
                            {
                                //cout << "'" << m->file << "' =?= '" << mod << "'\n";
                                if(m != nullptr && !strcmp(mod, m->file.c_str()))
                                {
                                    for(uint t = 0; t < m->func_count; t++)
                                    {
                                        Function* f = m->functions[t];
                                        //cout << m->file << "::" << f->sign << " found\n";
                                        if(!strcmp(fun->sign, f->sign) && fun->argc == f->argc)
                                        {
                                            for(uint k = 0; k < f->argc; ++k)
                                            {
                                                if(!Module::IsTypesEquals(fun->args[k].type, f->args[k].type))
                                                    goto out_1;
                                            }
                                            //cout << "[";
                                            //cout << "Function [mod] useless alloc -> dealloc\n";
                                            delete this->functions[fc];
                                            //kfree(this->functions[fc]); ///FIXME: useless alloc -> dealloc
                                            //cout << "]\n";
                                            this->functions[fc] = f;
                                            goto loop_out;
out_1: ;
                                        }
                                    }
                                }
                            }
                            //cout << fun->sign << " not found\n";
                            Module* m = new Module(rt);//(Module*)kmalloc(sizeof(Module));  //#OLDNEW
                            //new (m) Module();
                            //cout << "  loading...\n";
                            m->Load("/" + string(mod));
                            //cout << "  loaded\n";
                            rt->imported.push_back(m);
                            for(uint t = 0; t < m->func_count; t++) /// OPTIMIZE!!!
                            {
                                Function* f = m->functions[t];
                                if(!strcmp(fun->sign, f->sign) && fun->argc == f->argc)
                                {
                                    for(uint k = 0; k < f->argc; ++k)
                                    {
                                        if(!Module::IsTypesEquals(fun->args[k].type, f->args[k].type))
                                            goto out_2;
                                    }
                                    //cout << "{";
                                    //cout << "Function useless alloc -> dealloc\n";
                                    delete this->functions[fc];
                                    //kfree(this->functions[fc]); ///FIXME: useless alloc -> dealloc
                                    //cout << "}\n";
                                    this->functions[fc] = f;
                                    goto loop_out;
out_2: ;
                                }
                            }
                            //}
                        }
                        else if(!(fun->flags & FFlags::RTINTERNAL))
                        {
                            //cout << "--impl\n";
                            //uint size;
                            ifs.read((char*)&fun->locals_size, 4);
                            if(fun->locals_size > 0) {
                                //fun->locals = rt->allocator.Allocate<LocalVar>(fun->locals_size);
                                fun->locals = knew(LocalVar, fun->locals_size); //#OLDNEW

                                //uint local_mem_size;
                                ifs.read((char*)&fun->local_mem_size, 4);
                                //fun->locals_mem = new byte[local_mem_size];

                                uint cur_addr = 0;
                                for(uint k = 0; k < fun->locals_size; ++k) {
                                    fun->locals[k].addr = cur_addr;
                                    //fun->locals[i].type = (Type)ifs.get(); /// OLD        <-------------------------------------------
                                    uint addr = 0;
                                    ifs.read((char*)&addr, 4);
                                    fun->locals[k].type = (byte*)addr;
                                    //cout << "TYPE ADDR " << hex << ifs.tellg() << endl;
                                    fun->locals[k].type += (uint)types;
                                    cur_addr += Runtime::Sizeof(fun->locals[k].type);
                                }
                                //ifs.read((char*)fun->locals, fun->locals_size);
                            }
                            ifs.read((char*)&fun->bc_size, 4);
                            //fun->bytecode = (OpCode*)rt->allocator.RawAllocate(fun->bc_size * sizeof(OpCode));
                            fun->bytecode = (OpCode*)kmalloc(fun->bc_size); //#OLDNEW
                            ifs.read((char*)fun->bytecode, fun->bc_size);

                            //cout << "--impl\n";
                            rt->function_list.push_back(fun);                            
                            //cout << "--impl-o\n";
                        }
                        else
                        {
                            //cout << "--internal\n";
                            rt->function_list.push_back(fun);
                            //cout << "--internal-o\n";
                        }
                        //cout << "--end\n";
                        if(!strcmp(fun->sign, Function::INIT) && fun->argc == 0)
                            this->__global_constructor__ = fun;
                        fun->module = this;
loop_out:

                        ++fc;
                    }
                }
                break;
            case Header::Strings:
                {
                    //cout << "String segment\n";
                    //cout << "Strings size: " << h.end - h.begin << endl;
                    uint size = h.end - h.begin;
                    //this->strings = (char*)rt->allocator.RawAllocate(size * sizeof(char));
                    this->strings = (char*)kmalloc(size); //#OLDNEW
                    ifs.seekg(h.begin);
                    ifs.read(this->strings, size);
                }
                break;
            case Header::Globals:
                {
                    //cout << "Globals segment\n";
                    ifs.seekg(h.begin);
                    //uint gcount;
                    ifs.read((char*)&this->globals_count, 4);
                    if(this->globals_count == 0) {
                        this->globals = nullptr;
                        continue;
                    }
                    //this->globals = rt->allocator.Allocate<GlobalVar>(this->globals_count);
                    this->globals = knew(GlobalVar,this->globals_count); //#OLDNEW
                    GlobalVar* gptr = globals;
                    for(uint t = 0; (ifs.tellg() != h.end) && (t < globals_count); ++t) {
                        ///gptr->type = (Type)ifs.get(); ///   OLD <==========================================
                        ifs.read((char*)&gptr->type, 4);
                        gptr->type += (uint)(types);
                        gptr->isPrivate = (bool)ifs.get();
                        ifs.read(gptr->name, SIGN_LENGTH, '\0');//.ignore();

                        gptr->addr = rt->globals_ptr;
                        uint newsize = Runtime::Sizeof(gptr->type) + 4; ///???
                        uint sz = size_t(rt->globals_ptr - rt->global_var_mem);
                        if(sz + newsize > rt->global_size)
                            rt->allocGlobalMem();
                        rt->globals_ptr += newsize;
                        ++gptr;
                    }
                }
                break;
            case Header::Metadata:
            {
                ifs.seekg(h.begin);
                int count = 0;
                ifs.read((char*)&count, 4);
                for(int idx = 0; idx < count; ++idx)
                {
                    char key[SIGN_LENGTH];
                    ifs.read(key, SIGN_LENGTH, '\0');//.ignore();
                    //cout << "Meta " << key << endl;
                    Metadata::Type t;
                    t = (Metadata::Type)ifs.get();
                    if(!strcmp(key, "__FW_VERS"))
                    {
                        if(t != Metadata::Float)
                            rt->rtThrow(Runtime::MetaIncorrectType, "occured in module " + this->file);
                        double vers = 0;
                        ifs.read((char*)&vers, 8);
                        //cout << "FRAMEWORK VERSION " << vers << endl;
                        if(vers < MINIMAL_VERSION)
                            rt->rtThrow(Runtime::OldVersion, "occured in module " + this->file);
                        //cout << "Version is " << vers << endl;
                        break;
                    }
                    else
                    {
                        switch (t) {
                            case ::Metadata::Boolean:
                                ifs.ignore(1);
                                break;
                            case ::Metadata::Integer:
                                ifs.ignore(4);
                                break;
                            case ::Metadata::Float:
                                ifs.ignore(8);
                                break;
                            case ::Metadata::String:
                            case ::Metadata::Raw:
                            {
                                uint size = 0;
                                ifs.read((char*)&size, 4);
                                ifs.ignore(size);
                            }
                                break;
                            default:
                                break; ///@todo: @test test TEST
                        }
                    }
                }
            }
                break;
            default:
                break;
        }
    }
}
