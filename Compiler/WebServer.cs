using System.IO;
using System.Net;
using System.Reflection;
using System.Text;

public class WebServer {
    static Dictionary<string, (string code, DateTime submitted)> UserCodes = new();
    public static void Main(){
        Console.WriteLine("Web Server starting...");

        // Create a listener for the given port
        var listener = new HttpListener();
        listener.Prefixes.Add("http://a-ta.co/");
        //listener.Prefixes.Add("http://a-ta.co/");
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
            // Get the cookies
            var cookies = request.Cookies;
            string session;
            if(cookies["session"] == null){
                session = Guid.NewGuid().ToString();
                response.SetCookie(new Cookie("session", session));
            }else{
                session = cookies["session"]!.Value;
            }
            Console.WriteLine($"{session} {url.AbsolutePath}");
            if(url!.LocalPath == "/postCode"){
                // Get the request body
                var body = new StreamReader(request.InputStream).ReadToEnd();
                // Write the response
                response.StatusCode = 200;
                response.ContentType = "text/plain";
                response.ContentEncoding = Encoding.UTF8;
                response.OutputStream.Write(Encoding.UTF8.GetBytes($"Yeah"));
                response.Close();

                UserCodes[session] = (body, DateTime.Now);
                continue;
            }

            if(url!.LocalPath == "/funky3.wasm"){
                string code = UserCodes.ContainsKey(session) ? UserCodes[session].code : "print(\"Write some code to get started!\")";
                Compiler.Compile(code);
                using var f = new FileStream("Funky3Compiled/funky3.wasm", FileMode.Open);
                response.ContentType = "application/wasm";
                response.ContentLength64 = new FileInfo("Funky3Compiled/funky3.wasm").Length;
                f.CopyTo(response.OutputStream);
                Console.WriteLine("Sent!");
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
            // Get the file
            using(var stream = Assembly.GetExecutingAssembly().GetManifestResourceStream(file)){
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
}