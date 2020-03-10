#include "common.h"
#include "errno.h"
#include "sys/stat.h"
#include "a.out.h"
#include "linux/fs.h"
#include "linux/sched.h"
#include "linux/kernel.h"
#include "linux/mm.h"
#include "asm/segment.h"

extern ulong sys_exit(int exit_code);
extern ulong sys_close(int fd);

extern trap_info_t trap_info;

//MAX_ARG_PAGES defines the number of pages allocated for arguments and envelope for the new program
//32 should suffice,this gives a maximum env + arg of 128kB!

#define MAX_ARG_PAGES 32



//create_tables() parses the env- and arg-strings in new user memory
//and creates the pointer table from them,and puts their addresses
//on the "stack",returning the new stack pointer value.
static ulong *create_tables(char *p,int argc,int envc)
{
    ulong *argv,*envp;
    ulong *sp;
    
    sp = (ulong *)((~(8 - 1)) & (ulong)p);
    sp -= envc + 1;
    envp = sp;
    sp -= argc + 1;
    argv = sp;
    //put_fs_64long((uint64_t)envp,--sp);
    //put_fs_64long((uint64_t)argv,--sp);
    //put_fs_64long((uint64_t)argc,--sp);
    trap_info.regs[reg_a0] = argc;
    trap_info.regs[reg_a1] = argv;
    trap_info.regs[reg_a2] = envp;
    //syslog_print("argc = %d\r\n",argc);
    
    while(argc-- > 0)
    {
        put_fs_64long((uint64_t)p,argv++);
        while(get_fs_byte((const uint8_t *)(p++)));
    }
    
    put_fs_64long(0,argv);

    while(envc-- > 0)
    {
        put_fs_64long((uint64_t)p,envp++);
        while(get_fs_byte((const uint8_t *)(p++)));
    }

    put_fs_64long(0,envp);
    return sp;
}

static int count(char **argv)
{
    int i = 0;
    char **tmp;

    if(tmp = argv)
    {
        while(get_fs_64long((uint64_t *)(tmp++)))
        {
            i++;
        }
    }

    return i;
}

static ulong copy_strings(int argc,char **argv,ulong *page,ulong p,int from_kmem)
{
    char *tmp,*pag = NULL;
    int len,offset = 0;
    ulong old_fs,new_fs;

    if(!p)
    {
        return 0;
    }

    new_fs = get_ds();
    old_fs = get_fs();

    if(from_kmem == 2)
    {
        set_fs(new_fs);
    }

    while(argc-- > 0)
    {
        if(from_kmem == 1)
        {
            set_fs(new_fs);
        }

        if(!(tmp = (char *)get_fs_64long(((ulong *)argv) + argc)))
        {
            panic("argc is wrong");
        }

        if(from_kmem == 1)
        {
            set_fs(old_fs);
        }

        len = 0;//remember zero-padding
        
        do
        {
            len++;
        }while(get_fs_byte((const uint8_t *)(tmp++)));

        //this shouldn't happen - 128kB
        if((((long)p) - len) < 0)
        {
            set_fs(old_fs);
            return 0;
        }

        while(len)
        {
            --p;
            --tmp;
            --len;

            if((--offset) < 0)
            {
                offset = p % PAGE_SIZE;

                if(from_kmem == 2)
                {
                    set_fs(old_fs);
                }

                //int flag = !page[(p - USER_START_ADDR) / PAGE_SIZE];

                if((!(pag = (char *)page[(p - USER_START_ADDR) / PAGE_SIZE])) && (!(pag = (char *)(page[(p - USER_START_ADDR) / PAGE_SIZE] = (ulong)get_free_page()))))
                {
                    return 0;
                }

                /*if(flag)
                {
                    syslog_print("put_page:%p\r\n",(p >> 12) << 12);

                    if(!put_page(pag,(p >> 12) << 12))
                    {
                        panic("put page failed\r\n");
                    }
                }*/

                if(from_kmem == 2)
                {
                    set_fs(new_fs);
                }
            }

            *(pag + offset) = get_fs_byte((const uint8_t *)tmp);
            //syslog_print("offset = %p\r\n",offset);
        }

        //syslog_print("\r\n");
    }

    if(from_kmem == 2)
    {
        set_fs(old_fs);
    }
    
    return p;
}

static ulong change_ldt(ulong text_size,ulong *page)
{
    ulong code_limit,data_limit,code_base,data_base;
    int i;

    code_limit = text_size + PAGE_SIZE - 1;
    code_limit &= 0xFFFFF000;
    data_limit = 0x4000000;
    //code_base = current -> code_base;
    code_base = 0xC0000000;
    data_base = code_base;
    current -> code_base = code_base;
    current -> code_limit = code_limit;
    current -> data_base = data_base;
    current -> data_limit = data_limit;
    data_base += data_limit;

    for(i = MAX_ARG_PAGES - 1;i >= 0;i--)
    {
        data_base -= PAGE_SIZE;

        if(page[i])
        {
            //syslog_print("change_ldt:put_page %p\r\n",data_base);
            put_page(page[i],data_base);
        }
    }

    return data_limit;
}

int do_execve(char *filename,char **argv,char **envp)
{
    struct m_inode *inode;
    struct buffer_head *bh;
    struct exec ex;
    ulong page[MAX_ARG_PAGES];
    ulong i;
    int argc,envc;
    int e_uid,e_gid;
    int retval;
    int sh_bang = 0;
    //syslog_print("do_execve\r\n");

    ulong p = 0xC0000000 + PAGE_SIZE * MAX_ARG_PAGES - 8;
    
    //clear page-table
    for(i = 0;i < MAX_ARG_PAGES;i++)
    {
        page[i] = 0;
    }
    
    //get executables inode
    if(!(inode = namei(filename)))
    {
        return -ENOENT;
    }
    
    argc = count(argv);
    envc = count(envp);
    
    restart_interp:
        //must be regular file
        if(!S_ISREG(inode -> i_mode))
        {
            retval = -EACCES;
            goto exec_error2;
        }

        i = inode -> i_mode;
        e_uid = (i & S_ISUID) ? inode -> i_uid : current -> euid;
        e_gid = (i & S_ISGID) ? inode -> i_gid : current -> egid;

        if(current -> euid == inode -> i_uid)
        {
            i >>= 6;
        }
        else if(current -> egid == inode -> i_gid)
        {
            i >>= 3;
        }

        if((!(i & 1)) && (!((inode -> i_mode & 0111) && suser())))
        {
            retval = -ENOEXEC;
            goto exec_error2;
        }

        if(!(bh = bread(inode -> i_dev,inode -> i_zone[0])))
        {
            retval = -EACCES;
            goto exec_error2;
        }

        ex = *((struct exec *)bh -> b_data);

        if((bh -> b_data[0] == '#') && (bh -> b_data[1] == '!') && (!sh_bang))
        {
            //This section does the #! interpretation
            //Sorta complicated,but hopefully it will work.-TYT
            char buf[1023],*cp,*interp,*i_name,*i_arg;
            ulong old_fs;
            strncpy(buf,bh -> b_data + 2,1022);
            brelse(bh);
            iput(inode);
            buf[1022] = '\0';

            if(cp = strchr(buf,'\n'))
            {
                *cp = '\0';

                for(cp = buf;(*cp == ' ') || (*cp == '\t');cp++)
                {
                    
                }
            }

            if((!cp) || (*cp == '\0'))
            {
                retval = -ENOEXEC;//No interpreter name found
                goto exec_error1;
            }

            interp = i_name = cp;
            i_arg = 0;

            for(;*cp && (*cp != ' ') && (*cp != '\t');cp++)
            {
                if(*cp == '/')
                {
                    i_name = cp + 1;
                }
            }

            if(*cp)
            {
                *cp++ = '\0';
                i_arg = cp;
            }

            //OK,we've parsed out the interpreter name and (optional)argument
            if(sh_bang++ == 0)
            {
                p = copy_strings(envc,envp,page,p,0);
                p = copy_strings(--argc,argv + 1,page,p,0);
            }

            p = copy_strings(1,&filename,page,p,1);
            argc++;

            if(i_arg)
            {
                p = copy_strings(1,&i_arg,page,p,2);
                argc++;
            }

            p = copy_strings(1,&i_name,page,p,2);
            argc++;

            if(!p)
            {
                retval = -ENOMEM;
                goto exec_error1;
            }

            old_fs = get_fs();
            set_fs(get_ds());

            if(!(inode = namei(interp)))
            {
                set_fs(old_fs);
                retval = -ENOENT;
                goto exec_error1;
            }

            set_fs(old_fs);
            goto restart_interp;
        }

        brelse(bh);

        if(N_MAGIC(ex) != ZMAGIC || ex.a_trsize || ex.a_drsize || ((ex.a_text + ex.a_data + ex.a_bss) > 0x3000000) || (inode -> i_size < (ex.a_text + ex.a_data + ex.a_syms + N_TXTOFF(ex))))
        {
            retval = -ENOEXEC;
            goto exec_error2;
        }

        if(N_TXTOFF(ex) != BLOCK_SIZE)
        {
            printk("%s: N_TXTOFF != BLOCK_SIZE. See a.out.h",filename);
            retval = -ENOEXEC;
            goto exec_error2;
        }

        if(!sh_bang)
        {
            p = copy_strings(envc,envp,page,p,0);
            p = copy_strings(argc,argv,page,p,0);

            if(!p)
            {
                retval = -ENOMEM;
                goto exec_error2;
            }
        }

        //OK,this is the point of no return
        if(current -> executable)
        {
            iput(current -> executable);
        }
        
        current -> executable = inode;

        for(i = 0;i < 32;i++)
        {
            current -> sigaction[i].sa_handler = NULL;
        }

        for(i = 0;i < NR_OPEN;i++)
        {
            if((current -> close_on_exec >> i) & 1)
            {
                sys_close(i);
            }
        }
        
        current -> close_on_exec = 0;
        free_page_tables(current -> code_base,current -> page_dir_table,current -> code_limit);
        free_page_tables(current -> data_base,current -> page_dir_table,current -> data_limit);
        p += change_ldt(ex.a_text,page) - MAX_ARG_PAGES * PAGE_SIZE;
        p = (ulong)create_tables((char *)p,argc,envc);
        current -> start_code = 0xC0000000;
        current -> brk = ex.a_bss + (current -> end_data = ex.a_data + (current -> end_code = 0xC0000000 + ex.a_text));
        current -> start_stack = p & 0xFFFFF000;
        current -> euid = e_uid;
        current -> egid = e_gid;
        /*i = 0xC0000000 + ex.a_text + ex.a_data;

        while(i & 0xFFF)
        {
            put_fs_byte(0,(uint8_t *)(i++));
        }*/
        pte_common_disable_user((volatile pte_64model *)&page_root_table[2]);
        trap_info.regs[reg_sp] = p;
        trap_info.newepc = ex.a_entry;
        //syslog_print("newepc = %p\r\n",trap_info.newepc);
        //return 0;
        return argc;//assign argc to reg_a0
    exec_error2:
        iput(inode);
    exec_error1:
        for(i = 0;i < MAX_ARG_PAGES;i++)
        {
            free_page(page[i]);
        }

        return retval;
}