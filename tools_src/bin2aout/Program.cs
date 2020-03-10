using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace bin2aout
{
    class Program
    {
        static void AddLong(List<byte> list,uint x)
        {
            list.Add((byte)(x & 0xFF));
            list.Add((byte)((x >> 8) & 0xFF));
            list.Add((byte)((x >> 16) & 0xFF));
            list.Add((byte)((x >> 24) & 0xFF));
        }
        static void Main(string[] args)
        {
            if(args.Count() == 3)
            {
                var src = args[0];
                var text = args[1];
                var dst = args[2];

                var text_file = File.ReadAllLines(text);

                var entry_point = 0xC0000000U;

                foreach(var line in text_file)
                {
                    if(line.Contains("<_start>"))
                    {
                        var t = line.Substring(0,line.IndexOf("<_start>")).Trim();
                        entry_point = uint.Parse(t,System.Globalization.NumberStyles.HexNumber);
                        break;
                    }
                }

                Console.WriteLine("entry_point:" + entry_point);

                var code = File.ReadAllBytes(src).ToList();

                while((code.Count & 0xFFF) != 0)
                {
                    code.Add(0);
                }

                var r = new List<byte>();

                AddLong(r,0x10B);//a_magic
                AddLong(r,(uint)code.Count);//a_text
                AddLong(r,0);//a_data
                AddLong(r,0);//a_bss
                AddLong(r,0);//a_syms
                AddLong(r,entry_point);//a_entry
                var cnt = 1024 - r.Count;

                for(var i = 0;i < cnt;i++)
                {
                    r.Add(0);
                }

                foreach(var b in code)
                {
                    r.Add(b);
                }

                File.WriteAllBytes(dst,r.ToArray());
            }
        }
    }
}
