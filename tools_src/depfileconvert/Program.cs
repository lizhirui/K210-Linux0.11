using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace depfileconvert
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                if(args != null && args.Length > 1)
                {
                    var rootpath = args[0];
                    var filepath = args[1];
                    var text = File.ReadAllText(filepath).Replace(" \\\r\n"," ").Replace("\r\n"," ");

                    if(rootpath.Length > 0 && rootpath.Substring(rootpath.Length - 1,1) != "\\")
                    {
                        rootpath += "\\";
                    }

                    while(text.Contains("  "))
                    {
                        text = text.Replace("  "," ");
                    }

                    var list = Regex.Split(text,": ");

                    if(list.Length > 1)
                    {
                        var r = new StringBuilder();
                        var objfilename = filepath.Replace(".d",".o").Replace(rootpath,"./").Replace("\\","/");

                        if(objfilename.Length < 2 || objfilename.Substring(0,2) != "./")
                        {
                            objfilename = "./" + objfilename;
                        }

                        r.Append(objfilename + ":");

                        var deplist = list[1].Split(' ');

                        foreach(var item in deplist)
                        {
                            var t = item.Trim().Replace(rootpath,"./");
                            t = t.Replace("\\","/");

                            if(t == "")
                            {
                                continue;
                            }

                            if(t.Length < 2 || t.Substring(0,2) != "./")
                            {
                                t = "./" + t;
                            }

                            r.Append(" " + t);
                        }

                        File.WriteAllText(filepath,r.ToString() + "\r\n");
                    }
                }
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
                Console.WriteLine(ex.StackTrace);
                Environment.Exit(-1);
            }
        }
    }
}
