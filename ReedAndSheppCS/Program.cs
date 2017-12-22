using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace ReedAndShepp
{
    static class Program
    {
        static void Main(string[] args)
        {
            ReedAndShepp ras = new ReedAndShepp();
            ReedAndShepp.Vector3[] path;
            Console.WriteLine(ras.ComputeCurve(new ReedAndShepp.Vector3(0, 0, 0), new ReedAndShepp.Vector3(1, 1, 0), 0.1, out path));
            foreach (ReedAndShepp.Vector3 v in path)
            {
                ReedAndShepp.Vector3 v2 = v.Floor(2);
                Console.WriteLine(v2.x.ToString() + "\t" + v2.y.ToString() + "\t" + v2.z.ToString());
            }
            Console.Read();
        }
    }
}
