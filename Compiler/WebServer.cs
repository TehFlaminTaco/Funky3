using System.Net;

public class WebServer {
    public static void Main(){
        Console.WriteLine("Web Server starting...");

        // Create a listener for the given port
        var listener = new HttpListener();
        listener.Prefixes.Add("http://localhost:8080/");
        listener.Prefixes.Add("http://a-ta.co/");
        listener.Start();

    }
}