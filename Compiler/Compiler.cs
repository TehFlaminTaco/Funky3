using System.Net;
using System.Reflection;
using System;
using System.Diagnostics;

public static class Compiler
{
    public static string CurrentCode { get; set; } = "";
    public static void Compile(string code)
    {
        // Generate a Temporary folder for compiling
        var temp = Path.Combine(Path.GetTempPath(), "Funky3Compiler");
        var folder = Path.Combine(temp, Path.GetRandomFileName());
        Directory.CreateDirectory(folder);
        Directory.CreateDirectory(Path.Combine(folder, "metatables"));
        Directory.CreateDirectory(Path.Combine(folder, "libs"));
        Block.ResetScopes();
        Expression.TakenNames.Clear();
        DeOperator.KnownOperators.Clear();
        try
        {
            // Copy all the C code into the folder
            foreach (var s in Assembly.GetExecutingAssembly().GetManifestResourceNames().Where(c => c.StartsWith("Funky3_Compiler.C.")).Where(c => c.EndsWith(".c") || c.EndsWith(".h") || c.EndsWith("preamble.js")))
            {
                // Get a local folder name from s
                var targetFile = s["Funky3_Compiler.C.".Length..].Replace('.', Path.DirectorySeparatorChar);
                var lastSlash = targetFile.LastIndexOf(Path.DirectorySeparatorChar);
                // Replace the last Slash with a dot
                if (lastSlash > 0)
                {
                    targetFile = string.Concat(targetFile.AsSpan(0, lastSlash), ".", targetFile.AsSpan(lastSlash + 1));
                }
                var file = Path.Combine(folder, targetFile);
                //Console.WriteLine(file);
                using (var stream = Assembly.GetExecutingAssembly().GetManifestResourceStream(s))
                {
                    using (var fileStream = File.Create(file))
                    {
                        stream!.CopyTo(fileStream);
                    }
                }
            }

            using (StreamWriter bodyStream = new StreamWriter(Path.Combine(folder, "main.c")))
            {
                using (StreamWriter headerStream = new StreamWriter(Path.Combine(folder, "header.c")))
                {
                    Compile(code, bodyStream, headerStream);
                }
            }

            // And also the header.c
            using (StreamReader reader = new StreamReader(Path.Combine(folder, "header.c")))
            {
                WebServer.CompiledHeader[WebServer.CurrentSession] = (reader.ReadToEnd(), DateTime.Now);
            }
            // For debugging, print the main.c
            using (StreamReader reader = new StreamReader(Path.Combine(folder, "main.c")))
            {
                WebServer.CompiledBody[WebServer.CurrentSession] = (reader.ReadToEnd(), DateTime.Now);
            }

            Directory.CreateDirectory(Path.Combine(folder, "output"));
            // TODO: Check if this is windows or linux and run accordingly.
            ProcessStartInfo compilerStart;
            // If Windows
            if (Environment.OSVersion.Platform == PlatformID.Win32NT)
            {
                File.WriteAllText(Path.Combine(folder, "build.bat"), "emcc funky3.c -w -fweb --pre-js 'preamble.js' -s LLD_REPORT_UNDEFINED -s NO_EXIT_RUNTIME -sINITIAL_MEMORY=1gb -sEXPORTED_FUNCTIONS=[\"_main\",\"_DrawHook\"] -o output/funky3.js");
                compilerStart = new("cmd", "/c build.bat");
            }
            else
            {
                File.WriteAllText(Path.Combine(folder, "build.sh"), "emcc funky3.c -W -Wno-everything -fweb --pre-js 'preamble.js' -s LLD_REPORT_UNDEFINED -s NO_EXIT_RUNTIME -sINITIAL_MEMORY=1gb -sEXPORTED_FUNCTIONS='[\"_main\",\"_DrawHook\"]' -o output/funky3.js");
                compilerStart = new("bash", "build.sh");
            }
            compilerStart.WorkingDirectory = folder + Path.DirectorySeparatorChar;
            compilerStart.UseShellExecute = false;
            compilerStart.RedirectStandardOutput = true;
            compilerStart.RedirectStandardError = true;
            compilerStart.CreateNoWindow = true;
            Console.WriteLine("Compiling...");
            Process compiler = Process.Start(compilerStart)!;
            compiler.WaitForExit();
            if (compiler.ExitCode != 0)
            {
                WebServer.WriteError(compiler.StandardOutput.ReadToEnd());
                WebServer.WriteError(compiler.StandardError.ReadToEnd());
                throw new Exception("Compilation failed");
            }
            Console.WriteLine("Done!");

            // Copy result to the output folder
            if (Directory.Exists("Funky3Compiled2"))
            {
                Directory.Delete("Funky3Compiled2", true);
            }
            Directory.CreateDirectory("Funky3Compiled2");
            CopyDirectory(Path.Combine(folder, "output"), Path.Combine("Funky3Compiled2"), true);
            WebServer.CompiledWasm[WebServer.CurrentSession] = (File.ReadAllBytes(Path.Combine(folder, "output", "funky3.wasm")), DateTime.Now);
            WebServer.CompiledJS[WebServer.CurrentSession] = (File.ReadAllText(Path.Combine(folder, "output", "funky3.js")), DateTime.Now);
            WebServer.WriteError("Compilation successful");
        }
        catch (Exception e)
        {
            WebServer.WriteError(e.ToString());
        }
        finally
        {
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

    public static void Compile(string sourceCode, StreamWriter bodyOutput, StreamWriter headerOutput)
    {
        CHeader.HeaderCodeChunks.Clear();
        CInline.InlineCodeChunks.Clear();
        Define.Macros.Clear();
        Console.WriteLine("Preprocessing...");
        string preprocessedCode = Preprocessor.Process(sourceCode);
        WebServer.PreCompiled[WebServer.CurrentSession] = (preprocessedCode, DateTime.Now);
        CurrentCode = preprocessedCode;
        Console.WriteLine("Tokenizing...");
        var tokens = Tokenizer.Tokenize(preprocessedCode);

        foreach (var headCode in CHeader.HeaderCodeChunks)
        {
            headerOutput.WriteLine(headCode);
        }

        Console.WriteLine("Parsing...");
        Parser.Parse(tokens, bodyOutput, headerOutput);
    }
}