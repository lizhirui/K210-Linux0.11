using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace codegenerator
{
    class Program
    {
        static string generate_csrr()
        {
            var stb = new StringBuilder();

            for(var i = 0x000;i <= 0xFFF;i++)
            {
                stb.Append("case 0x" + string.Format("{0:X3}",i) + ":\r\n");
                stb.Append("    v = csr_asm_read(0x" + string.Format("{0:X3}",i) + ");\r\n    break;\r\n");
            }

            return stb.ToString();
        }

        static string generate_csrw()
        {
            var stb = new StringBuilder();

            for(var i = 0x000;i <= 0xFFF;i++)
            {
                stb.Append("case 0x" + string.Format("{0:X3}",i) + ":\r\n");
                stb.Append("    csr_asm_write(0x" + string.Format("{0:X3}",i) + ",value.value);\r\n    break;\r\n");
            }

            return stb.ToString();
        }

        [STAThread]
        static void Main(string[] args)
        {
            Clipboard.SetDataObject(generate_csrw());
        }
    }
}
