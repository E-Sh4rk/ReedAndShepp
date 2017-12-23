﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace ReedAndShepp
{
    public class ReedAndShepp
    {
        static class X86
        {
            // double reed_shepp(double x1, double y1, double t1, double x2, double y2, double t2, int* numero, double* tr, double* ur, double* vr)
            [DllImport("ReedAndSheppDll.dll", SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
            public static extern double reed_shepp(double x1, double y1, double t1, double x2, double y2, double t2,
                out int numero, out double tr, out double ur, out double vr);

            // int constRS(int num, double t, double u, double v, double x1, double y1, double t1, double delta, double* pathx, double* pathy, double* patht)
            [DllImport("ReedAndSheppDll.dll", SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
            public static extern int constRS(int num, double t, double u, double v, double x1, double y1, double t1, double delta,
                double[] pathx, double[] pathy, double[] patht);

            // void change_radcurv(double radcurv)
            [DllImport("ReedAndSheppDll.dll", SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
            public static extern void change_radcurv(double radcurv);
        }
        static class X64
        {
            // double reed_shepp(double x1, double y1, double t1, double x2, double y2, double t2, int* numero, double* tr, double* ur, double* vr)
            [DllImport("ReedAndSheppDll64.dll", SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
            public static extern double reed_shepp(double x1, double y1, double t1, double x2, double y2, double t2,
                out int numero, out double tr, out double ur, out double vr);

            // int constRS(int num, double t, double u, double v, double x1, double y1, double t1, double delta, double* pathx, double* pathy, double* patht)
            [DllImport("ReedAndSheppDll64.dll", SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
            public static extern int constRS(int num, double t, double u, double v, double x1, double y1, double t1, double delta,
                double[] pathx, double[] pathy, double[] patht);

            // void change_radcurv(double radcurv)
            [DllImport("ReedAndSheppDll64.dll", SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
            public static extern void change_radcurv(double radcurv);
        }

        delegate double RS(double x1, double y1, double t1, double x2, double y2, double t2,
                out int numero, out double tr, out double ur, out double vr);
        delegate int cRS(int num, double t, double u, double v, double x1, double y1, double t1, double delta,
                double[] pathx, double[] pathy, double[] patht);

        RS reed_shepp;
        cRS constRS;

        public ReedAndShepp(double radius) : this(radius, null) { }
        public ReedAndShepp(double radius, string folder)
        {
            if (folder != null)
                Environment.SetEnvironmentVariable("PATH", Environment.GetEnvironmentVariable("PATH") + ";" + Path.GetFullPath(folder));
            if (/*Environment.Is64BitProcess*/IntPtr.Size == 8)
            {
                reed_shepp = X64.reed_shepp;
                constRS = X64.constRS;
                X64.change_radcurv(radius);
            }
            else
            {
                reed_shepp = X86.reed_shepp;
                constRS = X86.constRS;
                X86.change_radcurv(radius);
            }
        }

        public struct Vector3
        {
            public Vector3(float x, float y, float z)
            {
                this.x = x; this.y = y; this.z = z;
            }
            public float x;
            public float y;
            public float z;
            public Vector3 Floor(int nb_dec)
            {
                float mult = (int)Math.Pow(10, nb_dec);
                int _x = (int)(x * mult);
                int _y = (int)(y * mult);
                int _z = (int)(z * mult);
                return new Vector3(_x/mult, _y/mult, _z/mult);
            }
        }

        public double ComputeCurve(Vector3 init, Vector3 target, double delta, out Vector3[] path)
        {
            int num;
            double tr, ur, vr;
            double length = reed_shepp(init.x, init.y, init.z, target.x, target.y, target.z, out num, out tr, out ur, out vr);
            int maxlen = (int)(length / delta) + 5;
            double[] pathx = new double[maxlen];
            double[] pathy = new double[maxlen];
            double[] pathz = new double[maxlen];
            int pathlen = constRS(num, tr, ur, vr, init.x, init.y, init.z, delta, pathx, pathy, pathz);
            path = new Vector3[pathlen];
            for (int i = 0; i < pathlen; i++)
                path[i] = new Vector3((float)pathx[i], (float)pathy[i], (float)pathz[i]);
            return length;
        }
    }
}