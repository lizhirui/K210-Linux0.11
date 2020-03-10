using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace syscall_generator
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender,EventArgs e)
        {
            var mustsyscalllist = new Dictionary<string,int>();
            var syscalllist = new List<string>();
            var idlist = new bool[2048];
            var syscallstringlist = new string[2048];
            var cnt = 0;

            syscalllist = textBox_syscalllist.Text.Split(',').ToList();

            for(var i = 0;i < syscalllist.Count;i++)
            {
                syscalllist[i] = syscalllist[i].Trim();
            }

            var arr = textBox_mustsyscallidlist.Text.Split('\n');

            foreach(var item in arr)
            {
                var xx = item.Trim().Split('|');
                mustsyscalllist[xx[0].ToLower()] = int.Parse(xx[1]);
                idlist[int.Parse(xx[1])] = true;
            }

            for(var i = 0;i < syscallstringlist.Length;i++)
            {
                syscallstringlist[i] = "NULL";
            }

            foreach(var item in syscalllist)
            {
                if(!mustsyscalllist.ContainsKey(item))
                {
                    while(idlist[cnt])
                    {
                        cnt++;
                    }

                    mustsyscalllist[item] = cnt;
                    idlist[cnt] = true;
                    cnt++;
                }

                syscallstringlist[mustsyscalllist[item]] = item;
            }
            
            var r1 = new StringBuilder();
            var r2 = new StringBuilder();

            for(var i = 0;i < syscallstringlist.Length;i++)
            {
                if(syscalllist.Contains(syscallstringlist[i]) || syscallstringlist[i] == "NULL")
                {
                    r1.Append(syscallstringlist[i] + ",");

                    if(syscallstringlist[i] != "NULL")
                    {
                        r2.Append("#define __NR_" + syscallstringlist[i].Substring(4,syscallstringlist[i].Length - 4) + " " + i + "\r\n");
                    }
                }
            }

            textBox_resultarr.Text = r1.ToString();
            textBox_resultconst.Text = r2.ToString();
        }
    }
}
