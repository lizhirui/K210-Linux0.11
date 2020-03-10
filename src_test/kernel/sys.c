#include "common.h"
#include "errno.h"
#include "linux/sched.h"
#include "linux/tty.h"
#include "linux/kernel.h"
#include "sys/times.h"
#include "sys/utsname.h"
#include "asm/segment.h"

int64_t syscall_return_value_check(int64_t returnvalue,int64_t no)
{
	if(no == 7)
	{
		if(returnvalue > 0)
		{
			syslog_print("syscall_return_value_check:no = %d,returnvalue = %d\r\n",no,returnvalue);
		}
	}
	
	return returnvalue;
}

//Fetch time
int64_t sys_ftime()
{
    return -ENOSYS;
}

int64_t sys_break()
{
    return -ENOSYS;
}

int64_t sys_ptrace()
{
    return -ENOSYS;
}

int64_t sys_stty()
{
    return -ENOSYS;
}

int64_t sys_gtty()
{
    return -ENOSYS;
}

int64_t sys_rename()
{
    return -ENOSYS;
}

int64_t sys_prof()
{
    return -ENOSYS;
}

int64_t sys_setregid(int rgid,int egid)
{
    if(rgid > 0)
    {
        if((current -> gid == rgid) || suser())
        {
            current -> gid = rgid;
        }
        else
        {
            return -EPERM;
        }
    }

    if(egid > 0)
    {
        if((current -> gid == egid) || (current -> egid == egid) || (current -> sgid = egid) || suser())
        {
            current -> egid = egid;
        }
        else
        {
            return -EPERM;
        }
    }

    return 0;
}

int64_t sys_setgid(int gid)
{
    return sys_setregid(gid,gid);
}

int64_t sys_acct()
{
    return -ENOSYS;
}

int64_t sys_phys()
{
    return -ENOSYS;
}

int64_t sys_lock()
{
    return -ENOSYS;
}

int64_t sys_mpx()
{
    return -ENOSYS;
}

int64_t sys_ulimit()
{
    return -ENOSYS;
}

int64_t sys_time(int64_t *tloc)
{
    int i;

    i = CURRENT_TIME;

    if(tloc)
    {
        verify_area(tloc,8);
        put_fs_long(i,(uint32_t *)tloc);
    }

    return i;
}

//unprivileged users may change the real user id to the effective uid or vice versa
int64_t sys_setreuid(int ruid,uint euid)
{
    uint old_ruid = current -> uid;

    if(ruid > 0)
    {
        if((current -> euid == ruid) || (old_ruid == ruid) || suser())
        {
            current -> uid = ruid;
        }
        else
        {
            return -EPERM;
        }
    }

    if(euid > 0)
    {
        if((old_ruid == euid) || (current -> euid == euid) || suser())
        {
            current -> euid = euid;
        }
        else
        {
            current -> uid = old_ruid;
            return -EPERM;
        }
    }

    return 0;
}

int64_t sys_setuid(int uid)
{
    return sys_setreuid(uid,uid);
}

int64_t sys_stime(int64_t *tptr)
{
    if(!suser())
    {
        return -EPERM;
    }

    startup_time = get_fs_long((uint32_t *)tptr) - jiffies/HZ;
    return 0;
}

int64_t sys_times(struct tms *tbuf)
{
    if(tbuf)
    {
        verify_area(tbuf,sizeof(*tbuf));
        /*user_ptr_to_kernel(&tbuf);
        tbuf -> tms_utime = current -> utime;
        tbuf -> tms_stime = current -> stime;
        tbuf -> tms_cutime = current -> cutime;
        tbuf -> tms_cstime = current -> cstime;*/
        struct tms t;
        t.tms_utime = current -> utime;
        t.tms_stime = current -> stime;
        t.tms_cutime = current -> cutime;
        t.tms_cstime = current -> cstime;
        mem_copy_from_kernel((ulong)&t,(ulong)tbuf,sizeof(t));
    }

    return jiffies;
}

int64_t sys_brk(ulong end_data_seg)
{
    if((end_data_seg >= current -> end_code) && (end_data_seg < (current -> start_stack - 16384)))
    {
        current -> brk = end_data_seg;
    }

    return current -> brk;
}

//This needs some heave checking
//I just haven't get the stomach for it.
int64_t sys_setpgid(int pid,int pgid)
{
    int i;

    if(!pid)
    {
        pid = current -> pid;
    }

    if(!pgid)
    {
        pgid = current -> pid;
    }

    for(i = 0;i < NR_TASKS;i++)
    {
        if(task[i] && (task[i] -> pid == pid))
        {
            if(task[i] -> leader)
            {
                return -EPERM;
            }

            if(task[i] -> session != current -> session)
            {
                return -EPERM;
            }

            task[i] -> pgrp = pgid;
            return 0;
        }
    }

    return -ESRCH;
}

int64_t sys_getpgrp()
{
    return current -> pgrp;
}

int64_t sys_setsid()
{
    if(current -> leader && (!suser()))
    {
        return -EPERM;
    }

    current -> leader = 1;
    current -> session = current -> pgrp = current -> pid;
    current -> tty = -1;
    return current -> pgrp;
}

int64_t sys_uname(struct utsname *name)
{
    static struct utsname thisname = {"linux .0","lizhirui","release ","0.11    ","K210    "};
    int i;

    if(!name)
    {
        return -ERROR;
    }

    verify_area(name,sizeof(*name));
    /*user_ptr_to_kernel(&name);
    *name = thisname;*/
    mem_copy_from_kernel((ulong)&thisname,(ulong)name,sizeof(thisname));
    return 0;
}

int64_t sys_umask(int mask)
{
    int old = current -> umask;
    current -> umask = mask & 0777;
    return old;
}

/*int64_t sys_write(char *buf)
{
    ulong len = 0;

    for(ulong i = 0;;i++)
    {
        if(get_fs_byte(buf + i) == 0x00)
        {
            len = i;
        }
    }

    tty_write(0,buf,len);
    return 0;
}*/

int64_t sys_debug(ulong p)
{
    syslog_print("sys_debug:%d\r\n",p);
    return 0;
}