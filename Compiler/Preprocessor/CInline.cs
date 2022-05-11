
using System.Text;
public static class CInline {
    public static Dictionary<string, string> InlineCodeChunks = new();

    public static void Parse(StreamReader reader, StringBuilder sb) {
        string? line;
        StringBuilder inlineCode = new();
        while((line = reader.ReadLine()) != null) {
            if(Preprocessor.IsDirective.IsMatch(line)) {
                var match = Preprocessor.IsDirective.Match(line);
                var directive = match.Groups["directive"].Value;
                if(directive == "cInlineEnd") {
                    break;
                }else{
                    inlineCode.AppendLine(line);
                }
            }else{
                inlineCode.AppendLine(line);
            }
        }
        var inlineCodeName = $"{InlineCodeChunks.Count}";
        InlineCodeChunks[inlineCodeName] = inlineCode.ToString();
        sb.AppendLine($"$CodeChunk{inlineCodeName}");

    }
}