
using System.Text;
public static class CHeader {
    public static List<string> HeaderCodeChunks = new();

    public static void Parse(StreamReader reader, StringBuilder sb) {
        string? line;
        StringBuilder headerCode = new();
        while((line = reader.ReadLine()) != null) {
            if(Preprocessor.IsDirective.IsMatch(line)) {
                var match = Preprocessor.IsDirective.Match(line);
                var directive = match.Groups["directive"].Value;
                if(directive == "cHeaderEnd") {
                    break;
                }else{
                    headerCode.AppendLine(line);
                }
            }else{
                headerCode.AppendLine(line);
            }
        }
        HeaderCodeChunks.Add(headerCode.ToString());
    }
}