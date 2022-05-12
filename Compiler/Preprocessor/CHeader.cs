
using System.Text;
public static class CHeader {
    public static List<string> HeaderCodeChunks = new();

    public static void Parse(string code, StringBuilder sb) {
        HeaderCodeChunks.Add(code);
    }
}