using System.Reflection;
using System;
using System.Diagnostics;

public static class Compiler
{
    public static string CurrentCode { get; set; } = "";
    public static void Main(){
        // Generate a Temporary folder for compiling
        var temp = Path.Combine(Path.GetTempPath(), "Funky3Compiler");
        var folder = Path.Combine(temp, Path.GetRandomFileName());
        Directory.CreateDirectory(folder);
        Directory.CreateDirectory(Path.Combine(folder, "metatables"));
        try {
            // Copy all the C code into the folder
            foreach(var s in Assembly.GetExecutingAssembly().GetManifestResourceNames().Where(c=>c.EndsWith(".c") || c.EndsWith(".h"))){
                // Get a local folder name from s
                var targetFile = s["Funky3_Compiler.C.".Length..].Replace('.', Path.DirectorySeparatorChar);
                var lastSlash = targetFile.LastIndexOf(Path.DirectorySeparatorChar);
                // Replace the last Slash with a dot
                if(lastSlash > 0){
                    targetFile = string.Concat(targetFile.AsSpan(0, lastSlash), ".", targetFile.AsSpan(lastSlash + 1));
                }
                var file = Path.Combine(folder, targetFile);
                Console.WriteLine(file);
                using(var stream = Assembly.GetExecutingAssembly().GetManifestResourceStream(s)){
                    using(var fileStream = File.Create(file)){
                        stream!.CopyTo(fileStream);
                    }
                }
            }
            
            using (StreamWriter bodyStream = new StreamWriter(Path.Combine(folder, "main.c"))) {
                using (StreamWriter headerStream = new StreamWriter(Path.Combine(folder, "header.c"))) {
                    Compile("test.fnk", bodyStream, headerStream);
                }
            }
            
            // And also the header.c
            using (StreamReader reader = new StreamReader(Path.Combine(folder, "header.c"))) {
                Console.WriteLine(reader.ReadToEnd());
            }
            // For debugging, print the main.c
            using (StreamReader reader = new StreamReader(Path.Combine(folder, "main.c"))) {
                Console.WriteLine(reader.ReadToEnd());
            }

            // Compile the C code
            var compiler = new Process();
            compiler.StartInfo.FileName = "gcc";
            compiler.StartInfo.Arguments = "-w -O3 -o program.exe funky3.c";
            compiler.StartInfo.WorkingDirectory = folder;
            compiler.StartInfo.UseShellExecute = false;
            compiler.StartInfo.RedirectStandardOutput = true;
            compiler.StartInfo.RedirectStandardError = true;
            compiler.Start();
            var output = compiler.StandardOutput.ReadToEnd();
            var error = compiler.StandardError.ReadToEnd();
            compiler.WaitForExit();
            if(compiler.ExitCode != 0){
                Console.WriteLine(output);
                Console.WriteLine(error);
                throw new Exception("Compilation failed");
            }
            // Copy result to the output folder
            if(File.Exists(Path.Combine("Funky3.exe"))){
                File.Delete(Path.Combine("Funky3.exe"));
            }
            File.Copy(Path.Combine(folder, "program.exe"), Path.Combine("Funky3.exe"));
            Console.WriteLine("Compilation successful");
        }finally{
            Directory.Delete(temp, true);
        }
    }

    public static void Compile(string codePath, StreamWriter bodyOutput, StreamWriter headerOutput){
        using (StreamReader codeStream = new StreamReader(codePath)) {
            CurrentCode = codeStream.ReadToEnd();
            var tokens = Tokenizer.Tokenize(CurrentCode);
            Parser.Parse(tokens, bodyOutput, headerOutput);
            

        }
    }
}