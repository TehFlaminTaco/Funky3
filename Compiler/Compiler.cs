using System.IO;
using System;

public static class Compiler
{
    public static void Main(){
        using (StreamWriter stream = new(Console.OpenStandardOutput())){
            Compile(null, stream);
        }
    }

    public static void Compile(StreamReader code, StreamWriter output){
        output.Write("Hello, World!");
    }
}