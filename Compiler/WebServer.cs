using System.Web;
using System.IO;
using System.Net;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;

public class WebServer {
    public static string CurrentSession = "";
    public static Dictionary<string, (string code, DateTime submitted)> UserCodes = new();
    public static Dictionary<string, (string precompiled, DateTime submitted)> PreCompiled = new();
    public static Dictionary<string, (string compiled, DateTime submitted)> CompiledHeader = new();
    public static Dictionary<string, (string compiled, DateTime submitted)> CompiledBody = new();
    public static Dictionary<string, (byte[] compiled, DateTime submitted)> CompiledWasm = new();
    public static Dictionary<string, StringBuilder> Error = new();

    public static readonly Regex SafeFile = new(@"^[a-zA-Z0-9_]+$");

    public static void Main(string[] args) {
        Console.WriteLine("Pre-compiling examples...");
        foreach(var s in Assembly.GetExecutingAssembly().GetManifestResourceNames().Where(c=>c.StartsWith("Funky3_Compiler.Examples.")).Where(c=>c.EndsWith(".fnk"))){
            var code = new StreamReader(Assembly.GetExecutingAssembly().GetManifestResourceStream(s)!).ReadToEnd();
            CurrentSession = s["Funky3_Compiler.Examples.".Length..];
            Error[CurrentSession] = new();
            UserCodes[CurrentSession] = (code, DateTime.Now);
            Console.WriteLine("|> "+CurrentSession);
            Compiler.Compile(code);
            CurrentSession = "";
        }

        Console.WriteLine("Web Server starting...");

        // Create a listener for the given port
        var listener = new HttpListener();
        if(args.Length > 0)
            listener.Prefixes.Add(args[0]);
        else
            listener.Prefixes.Add("http://localhost:8080/");
        listener.Start();

        while(true){try{
            // Wait for a connection
            var context = listener.GetContext();
            // Get the request
            var request = context.Request;
            // Get the response
            var response = context.Response;
            // Get the request URL
            var url = request.Url;
            if(url == null){
                response.StatusCode = (int)HttpStatusCode.BadRequest;
                response.Close();
                continue;
            }

            // Get the cookies
            var cookies = request.Cookies;
            string session;
            if(cookies["session"] == null){
                if(url.LocalPath == "/cookieConsent"){
                    session = Guid.NewGuid().ToString();
                    response.SetCookie(new Cookie("session", session){
                        Discard = true
                    });
                }else{
                    var f = Assembly.GetExecutingAssembly().GetManifestResourceNames().FirstOrDefault(c=>c.EndsWith("cookieconsent.html"));
                    if(f == null){
                        // Not found
                        Console.WriteLine("404: " + url.LocalPath);
                        response.StatusCode = 404;
                        response.OutputStream.Write(Encoding.UTF8.GetBytes("404 Not Found"));
                        response.Close();
                        continue;
                    }
                    // Get the file
                    using(var stream = Assembly.GetExecutingAssembly().GetManifestResourceStream(f)){
                        if(stream == null){
                            // Not found. Not sure this is possible..?
                            Console.WriteLine("404: " + url.LocalPath);
                            response.StatusCode = 404;
                            response.OutputStream.Write(Encoding.UTF8.GetBytes("404 Not Found"));
                            response.Close();
                            continue;
                        }
                        // Get the file length
                        var length = stream.Length;
                        // Read the file
                        var data = new byte[length];
                        stream.Read(data, 0, (int)length);
                        // Write the file
                        response.ContentLength64 = length;
                        response.OutputStream.Write(data, 0, (int)length);
                        response.Close();
                    }
                    continue;
                }
            }else{
                session = cookies["session"]!.Value;
            }
            Console.WriteLine($"{session} {url.AbsolutePath}");
            if(url!.LocalPath == "/postCode"){
                // Get the request body
                var body = new StreamReader(request.InputStream).ReadToEnd();

                // Clear any current user codes
                UserCodes.Remove(session);
                PreCompiled.Remove(session);
                CompiledHeader.Remove(session);
                CompiledBody.Remove(session);
                CompiledWasm.Remove(session);
                Error.Remove(session);
                Error[session] = new();

                // Store the code
                UserCodes.Add(session, (body, DateTime.Now));
                // Run the compiler
                CurrentSession = session;
                Compiler.Compile(body);
                CurrentSession = "";

                // Write the response
                response.StatusCode = 200;
                response.ContentType = "text/plain";
                response.ContentEncoding = Encoding.UTF8;
                response.OutputStream.Write(Encoding.UTF8.GetBytes($"Yeah"));
                response.Close();
                continue;
            }

            if(url.LocalPath == "/getExample"){ // Load an example into the user's session
                var query = HttpUtility.ParseQueryString(url.Query);
                if(query["file"]==null){
                    response.StatusCode = (int)HttpStatusCode.BadRequest;
                    response.Close();
                    continue;
                }
                string example = query["file"]!;
                var f = Assembly.GetExecutingAssembly().GetManifestResourceNames().Where(c=>c.StartsWith("Funky3_Compiler.Examples.")).FirstOrDefault(c=>c.EndsWith(example));
                if(f == null){
                    response.StatusCode = (int)HttpStatusCode.BadRequest;
                    response.Close();
                    continue;
                }
                UserCodes[session] = UserCodes[example];
                PreCompiled[session] = PreCompiled[example];
                CompiledHeader[session] = CompiledHeader[example];
                CompiledBody[session] = CompiledBody[example];
                CompiledWasm[session] = CompiledWasm[example];
                Error[session] = new StringBuilder(Error[example].ToString());
                response.StatusCode = 200;
                response.ContentType = "text/plain";
                response.ContentEncoding = Encoding.UTF8;
                response.OutputStream.Write(Encoding.UTF8.GetBytes($"Yeah"));
                response.Close();
                continue;
            }

            if(url.LocalPath == "/currentCode.txt"){
                if(UserCodes.TryGetValue(session, out var code)){
                    response.StatusCode = 200;
                    response.ContentType = "text/plain";
                    response.ContentEncoding = Encoding.UTF8;
                    response.OutputStream.Write(Encoding.UTF8.GetBytes(code.code));
                    response.Close();
                    continue;
                }
                response.StatusCode = 200;
                response.ContentType = "text/plain";
                response.ContentEncoding = Encoding.UTF8;
                response.OutputStream.Write(Encoding.UTF8.GetBytes(UserCodes["helloworld.fnk"].code));
                response.Close();
                continue;
            }

            if(url!.LocalPath == "/funky3.wasm"){
                if(!UserCodes.ContainsKey(session)){
                    response.ContentType = "application/wasm";
                    response.OutputStream.Write(CompiledWasm["helloworld.fnk"].compiled);
                    response.Close();
                    continue;
                }

                if(!CompiledWasm.ContainsKey(session)){
                    response.ContentType = "application/wasm";
                    response.OutputStream.Write(CompiledWasm["helloworld.fnk"].compiled);
                    response.Close();
                    continue;
                }

                response.ContentType = "application/wasm";
                response.OutputStream.Write(CompiledWasm[session].compiled);
                response.Close();

                Console.WriteLine("Sent!");
                continue;
            }
            
            if(url.LocalPath == "/save"){
                // CRC the sent file
                string sentCode = new StreamReader(request.InputStream).ReadToEnd();
                // Just store the code in the URL.
                if(sentCode.Length < 512){
                    response.StatusCode = 200;
                    response.OutputStream.Write(Encoding.UTF8.GetBytes("?code=" + HttpUtility.UrlEncode(sentCode)));
                    response.Close();
                    continue;

                // Save it to disk and use a CRC to retreive it.
                }else{
                    using System.Security.Cryptography.MD5 md5 = System.Security.Cryptography.MD5.Create();
                    byte[] hash = md5.ComputeHash(Encoding.UTF8.GetBytes(sentCode));
                    string hashString = Convert.ToHexString(hash);
                    if(!Directory.Exists("savedCode")){
                        Directory.CreateDirectory("savedCode");
                    }
                    if(File.Exists(Path.Combine("savedCode", hashString))){
                        response.StatusCode = (int)HttpStatusCode.BadRequest;
                        response.Close();
                        continue;
                    }else{
                        File.WriteAllText(Path.Combine("savedCode", hashString), sentCode);
                        response.StatusCode = 200;
                        response.OutputStream.Write(Encoding.UTF8.GetBytes("?file=" + hashString));
                        response.Close();
                        continue;
                    }
                }
            }

            if(url.LocalPath == "/error.html"){
                if(!Error.ContainsKey(session)){
                    response.ContentType = "text/html";
                    response.OutputStream.Write(Encoding.UTF8.GetBytes("<style>pre {color:white;}</style><pre>" + PreEscape(Error["helloworld.fnk"].ToString()) + "</pre>"));
                    response.Close();
                    continue;
                }

                response.ContentType = "text/html";
                response.OutputStream.Write(Encoding.UTF8.GetBytes("<style>pre {color:white;}</style><pre>" + PreEscape(Error[session].ToString()) + "</pre>"));
                response.Close();
                continue;
            }

            if(url.LocalPath == "/precompiled.html"){
                if(!PreCompiled.ContainsKey(session)){
                    response.ContentType = "text/html";
                    response.OutputStream.Write(Encoding.UTF8.GetBytes("<style>pre {color:white;}</style><pre>" + PreEscape(PreCompiled["helloworld.fnk"].precompiled) + "</pre>"));
                    response.Close();
                    continue;
                }

                response.ContentType = "text/html";
                response.OutputStream.Write(Encoding.UTF8.GetBytes("<style>pre {color:white;}</style><pre>" + PreEscape(PreCompiled[session].precompiled) + "</pre>"));
                response.Close();
                continue;
            }

            if(url.LocalPath == "/compiled.html"){
                if(!CompiledHeader.ContainsKey(session)){
                    response.ContentType = "text/html";
                    response.OutputStream.Write(Encoding.UTF8.GetBytes("<style>pre {color:white;}</style><pre>" + "// header.c\n" + PreEscape(CompiledHeader["helloworld.fnk"].compiled) + "\n\n// main.c\n" + PreEscape(CompiledBody["helloworld.fnk"].compiled) + "</pre>"));
                    response.Close();
                    continue;
                }

                response.ContentType = "text/html";
                response.OutputStream.Write(Encoding.UTF8.GetBytes("<style>pre {color:white;}</style><pre>" + "// header.c\n" + PreEscape(CompiledHeader[session].compiled) + "\n\n// main.c\n" + PreEscape(CompiledBody[session].compiled) + "</pre>"));
                response.Close();
                continue;
            }

            if(url.LocalPath == "/"){
                // Redirect to index.html
                response.Redirect("/index.html");
                response.Close();
                continue;
            }

            // Try to find the file in the Manifest
            var file = Assembly.GetExecutingAssembly().GetManifestResourceNames().FirstOrDefault(c=>c.EndsWith(url.LocalPath.Replace('/', '.')));
            if(file == null){
                // Not found
                Console.WriteLine("404: " + url.LocalPath);
                response.StatusCode = 404;
                response.OutputStream.Write(Encoding.UTF8.GetBytes("404 Not Found"));
                response.Close();
                continue;
            }

            if(url.LocalPath == "/index.html"){
                var query = HttpUtility.ParseQueryString(url.Query);
                if(query["file"] != null){
                    var loadFile = query["file"]!;
                    if(!SafeFile.IsMatch(loadFile)){
                        response.StatusCode = (int)HttpStatusCode.BadRequest;
                        response.Close();
                        continue;
                    }
                    // Load the code, and compile it.
                    if(!Directory.Exists("savedCode")){
                        Directory.CreateDirectory("savedCode");
                    }
                    if(File.Exists(Path.Combine("savedCode", loadFile))){
                        var code = File.ReadAllText(Path.Combine("savedCode",loadFile));
                        if(!(UserCodes.ContainsKey(session) && UserCodes[session].code == code)){
                            UserCodes[session] = (code, DateTime.Now);
                            CurrentSession = session;
                            Compiler.Compile(code);
                            CurrentSession = "";
                        }
                    }else{
                        response.StatusCode = (int)HttpStatusCode.BadRequest;
                        response.Close();
                        continue;
                    }
                }
            }

            // Get the file
            using(var stream = Assembly.GetExecutingAssembly().GetManifestResourceStream(file)){
                if(stream == null){
                    // Not found. Not sure this is possible..?
                    Console.WriteLine("404: " + url.LocalPath);
                    response.StatusCode = 404;
                    response.OutputStream.Write(Encoding.UTF8.GetBytes("404 Not Found"));
                    response.Close();
                    continue;
                }
                // Get the file length
                var length = stream.Length;
                // Read the file
                var data = new byte[length];
                stream.Read(data, 0, (int)length);
                // Write the file
                response.ContentLength64 = length;
                response.OutputStream.Write(data, 0, (int)length);
                response.Close();
            }
        }catch(Exception e){
            Console.WriteLine(e);
        }}
    }

    public static void WriteError(string err){
        if(String.IsNullOrEmpty(CurrentSession)){
            Console.WriteLine(err);
            return;
        }
        if(!Error.ContainsKey(CurrentSession)){
            Error[CurrentSession] = new();
        }
        Error[CurrentSession].AppendLine(err);
    }

    public static string PreEscape(string str){
        return str.Replace("<", "&lt;").Replace(">", "&gt;").Replace("&", "&amp;");
    }
}