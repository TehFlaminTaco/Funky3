
using System.Text;
public static class CInline {
    public static Dictionary<string, string> InlineCodeChunks = new();

    public static void Parse(string code, StringBuilder sb) {
        var inlineCodeName = $"{InlineCodeChunks.Count}";
        InlineCodeChunks[inlineCodeName] = code.ToString();
        sb.AppendLine($"$CodeChunk{inlineCodeName}");

    }
}