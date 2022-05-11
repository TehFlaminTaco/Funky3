using System.Text.RegularExpressions;
using System.Text;
public class Preprocessor {
    public static readonly Regex IsDirective = new(@"^\s*\$(?<directive>\w+)(?<body>.*)$");
    public static string Process(StreamReader reader) {
        StringBuilder sb = new();
        string? line;
        while((line = reader.ReadLine()) != null) {
            if(IsDirective.IsMatch(line)) {
                var match = IsDirective.Match(line);
                var directive = match.Groups["directive"].Value;
                var body = match.Groups["body"].Value;
                switch(directive) {
                    case "cInlineStart":
                        CInline.Parse(reader, sb);
                        break;
                    case "cHeaderStart":
                        CHeader.Parse(reader, sb);
                        break;
                    case "define":
                        Define.Parse(body);
                        break;
                    default:
                        sb.AppendLine(line);
                        break;
                }
            } else {
                sb.AppendLine(line);
            }
        }
        return sb.ToString();
    }
}