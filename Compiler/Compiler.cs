using System.Reflection;
using System;
using System.Diagnostics;

public static class Compiler
{
    public static string CurrentCode { get; set; } = "";
    public static void Compile(string code){
        // Generate a Temporary folder for compiling
        var temp = Path.Combine(Path.GetTempPath(), "Funky3Compiler");
        var folder = Path.Combine(temp, Path.GetRandomFileName());
        Directory.CreateDirectory(folder);
        Directory.CreateDirectory(Path.Combine(folder, "metatables"));
        try {
            // Copy all the C code into the folder
            foreach(var s in Assembly.GetExecutingAssembly().GetManifestResourceNames().Where(c=>c.StartsWith("Funky3_Compiler.C.")).Where(c=>c.EndsWith(".c") || c.EndsWith(".h"))){
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
                    Compile(code, bodyStream, headerStream);
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

            Directory.CreateDirectory(Path.Combine(folder, "output"));
            File.WriteAllText(Path.Combine(folder, "build.sh"), "emcc funky3.c -w -fweb -o output/funky3.js");
            ProcessStartInfo compilerStart = new("bash", "build.sh");
            compilerStart.WorkingDirectory = folder + Path.DirectorySeparatorChar;
            compilerStart.UseShellExecute = false;
            compilerStart.RedirectStandardOutput = true;
            compilerStart.RedirectStandardError = true;
            compilerStart.CreateNoWindow = true;
            Process compiler = Process.Start(compilerStart)!;
            compiler.WaitForExit();
            if(compiler.ExitCode != 0){
                Console.WriteLine(compiler.StandardOutput.ReadToEnd());
                Console.WriteLine(compiler.StandardError.ReadToEnd());
                throw new Exception("Compilation failed");
            }

            // Copy result to the output folder
            if(Directory.Exists("Funky3Compiled")){
                Directory.Delete("Funky3Compiled", true);
            }
            Directory.CreateDirectory("Funky3Compiled");
            CopyDirectory(Path.Combine(folder, "output"), Path.Combine("Funky3Compiled"), true);
            Console.WriteLine("Compilation successful");
        }finally{
            Directory.Delete(temp, true);
        }
    }

    static void CopyDirectory(string sourceDir, string destinationDir, bool recursive)
    {
        // Get information about the source directory
        var dir = new DirectoryInfo(sourceDir);

        // Check if the source directory exists
        if (!dir.Exists)
            throw new DirectoryNotFoundException($"Source directory not found: {dir.FullName}");

        // Cache directories before we start copying
        DirectoryInfo[] dirs = dir.GetDirectories();

        // Create the destination directory
        Directory.CreateDirectory(destinationDir);

        // Get the files in the source directory and copy to the destination directory
        foreach (FileInfo file in dir.GetFiles())
        {
            string targetFilePath = Path.Combine(destinationDir, file.Name);
            file.CopyTo(targetFilePath);
        }

        // If recursive and copying subdirectories, recursively call this method
        if (recursive)
        {
            foreach (DirectoryInfo subDir in dirs)
            {
                string newDestinationDir = Path.Combine(destinationDir, subDir.Name);
                CopyDirectory(subDir.FullName, newDestinationDir, true);
            }
        }
    }

    public static void Compile(string sourceCode, StreamWriter bodyOutput, StreamWriter headerOutput){
        string preprocessedCode = Preprocessor.Process(sourceCode);
        CurrentCode = preprocessedCode;
        var tokens = Tokenizer.Tokenize(preprocessedCode);

        foreach(var headCode in CHeader.HeaderCodeChunks){
            headerOutput.WriteLine(headCode);
        }

        Parser.Parse(tokens, bodyOutput, headerOutput);
    }
}