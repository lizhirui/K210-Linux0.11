using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace run
{
    [System.Runtime.InteropServices.StructLayout(LayoutKind.Sequential)]
    public class SECURITY_ATTRIBUTES
    {
        public int nLength;
        public string lpSecurityDescriptor;
        public bool bInheritHandle;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct STARTUPINFO
    {
        public int cb;
        public string lpReserved;
        public string lpDesktop;
        public int lpTitle;
        public int dwX;
        public int dwY;
        public int dwXSize;
        public int dwYSize;
        public int dwXCountChars;
        public int dwYCountChars;
        public int dwFillAttribute;
        public int dwFlags;
        public int wShowWindow;
        public int cbReserved2;
        public byte lpReserved2;
        public IntPtr hStdInput;
        public IntPtr hStdOutput;
        public IntPtr hStdError;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PROCESS_INFORMATION
    {
        public IntPtr hProcess;
        public IntPtr hThread;
        public int dwProcessId;
        public int dwThreadId;
    }

    class Program
    {
        [DllImport("Kernel32.dll", CharSet = CharSet.Ansi)]
        public static extern bool CreateProcess(
            StringBuilder lpApplicationName, StringBuilder lpCommandLine,
            SECURITY_ATTRIBUTES lpProcessAttributes,
            SECURITY_ATTRIBUTES lpThreadAttributes,
            bool bInheritHandles,
            int dwCreationFlags,
            StringBuilder lpEnvironment,
            StringBuilder lpCurrentDirectory,
            ref STARTUPINFO lpStartupInfo,
            ref PROCESS_INFORMATION lpProcessInformation
            );

        [DllImport("Kernel32.dll")]
        public static extern uint WaitForSingleObject(System.IntPtr hHandle, uint dwMilliseconds);
        
        //关闭一个内核对象,释放对象占有的系统资源。其中包括文件、文件映射、进程、线程、安全和同步对象等
        [DllImport("Kernel32.dll")]
        public static extern bool CloseHandle(System.IntPtr hObject);  
        
        //获取一个已中断进程的退出代码,非零表示成功，零表示失败。
        //参数hProcess，想获取退出代码的一个进程的句柄，参数lpExitCode，用于装载进程退出代码的一个长整数变量。
        [DllImport("Kernel32.dll")]
        static extern bool GetExitCodeProcess(System.IntPtr hProcess, ref uint lpExitCode);


        static void Main(string[] args)
        {
            var process = new Process();
            var path = Environment.CurrentDirectory;

            if(path[path.Length - 1] == '\\')
            {
                path = path.Substring(0,path.Length - 1);
            }

            if(path.Substring(path.Length - 5,5) != "tools")
            {
                path += "\\tools";
                Environment.CurrentDirectory = path;
            }

            if(args.Length == 0)
            {
                /*process.StartInfo.FileName = Process.GetCurrentProcess().MainModule.FileName;
                process.StartInfo.Arguments = "" + Process.GetCurrentProcess().Id;
                process.StartInfo.RedirectStandardOutput = true;
                process.StartInfo.RedirectStandardInput = true;
                process.StartInfo.UseShellExecute = false;
                process.Start();
                process.WaitForExit();*/
                STARTUPINFO sInfo = new STARTUPINFO();
                PROCESS_INFORMATION pInfo = new PROCESS_INFORMATION();

                if(!CreateProcess(null,new StringBuilder(Process.GetCurrentProcess().MainModule.FileName + " " + Process.GetCurrentProcess().Id),null,null,false,0,null,null,ref sInfo,ref pInfo))
                {
                    throw new Exception("调用失败");
                }

                WaitForSingleObject(pInfo.hProcess,int.MaxValue);
                CloseHandle(pInfo.hProcess);
                CloseHandle(pInfo.hThread);
            }
            else
            {
                var pid = int.Parse(args[0]);
                STARTUPINFO sInfo = new STARTUPINFO();
                PROCESS_INFORMATION pInfo = new PROCESS_INFORMATION();

                if(!CreateProcess(null,new StringBuilder("python kflash.py -B goE -b 1500000 -s -p COM9 ..\\src_test\\image.bin"),null,null,false,0,null,null,ref sInfo,ref pInfo))
                {
                    throw new Exception("调用失败");
                }
                
                try
                {
                    process = Process.GetProcessById(pInfo.dwProcessId);
                }
                catch
                {
                    goto next;
                }

                while(true)
                {
                    try
                    {
                        if(process.HasExited)
                        {
                            goto next;
                        }
                
                        var p = Process.GetProcessById(pid);

                        if((p is null) || (p.HasExited))
                        {
                            break;
                        }
                    }
                    catch
                    {
                        break;
                    }
                }

                if(!process.HasExited)
                {
                    process.Kill();
                }

                next:
                CloseHandle(pInfo.hProcess);
                CloseHandle(pInfo.hThread);

                
                sInfo = new STARTUPINFO();
                pInfo = new PROCESS_INFORMATION();

                if(!CreateProcess(null,new StringBuilder("plink -serial COM9 -sercfg 115200,8,1,N,N -mem"),null,null,false,0,null,null,ref sInfo,ref pInfo))
                {
                    throw new Exception("调用失败");
                }
                
                try
                {
                    process = Process.GetProcessById(pInfo.dwProcessId);
                }
                catch
                {
                    goto final;
                }

                while(true)
                {
                    try
                    {
                        if(process.HasExited)
                        {
                            goto final;
                        }
                
                        var p = Process.GetProcessById(pid);

                        if((p is null) || (p.HasExited))
                        {
                            break;
                        }
                    }
                    catch
                    {
                        break;
                    }
                }

                if(!process.HasExited)
                {
                    process.Kill();
                }    

                final:
                CloseHandle(pInfo.hProcess);
                CloseHandle(pInfo.hThread);
            }
        }
    }
}
