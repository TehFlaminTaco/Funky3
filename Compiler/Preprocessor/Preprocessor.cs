using System.Text.RegularExpressions;
using System.Text;
public class Preprocessor {
    public static readonly Regex IsDirective = new(@"^\s*\$(?<directive>\w+)(?<body>.*)$");

    // This is used to quickly chunk up the code for preprocessing.
    public static readonly Regex SuperTokenizer = new(@"
(?<newline>          [\n\r]                             )| (?#Newlines...)
(?<multilineComment> \$\*(.|\s)*?\*\$                   )| (?#Multiline Comment)
(?<string>           (?<qoute>[\""'`])(\\.|.)*?\<qoute> )| (?#Strings)
(?<singleineComment> \$\$.*                             )| (?#Singleline Comment)
(?<preprocessor>     \s*\$(?<directive>\w+)(?<body>.+)  )| (?#Preprocessor)
(?<code>             [^$\""'\r\n]+                      )| (?#Regular, Boring code)
(?<unknown>          .                                  )  (?#Incase I missed something)",
RegexOptions.Multiline | RegexOptions.IgnorePatternWhitespace);

    public static string Process(StreamReader reader) {
        StringBuilder sb = new();
        string allCode = reader.ReadToEnd();
        // For each chunk of code
        foreach (Match m in SuperTokenizer.Matches(allCode)) {
            // If it's a preprocessor directive
            if (m.Groups["preprocessor"].Success) {
                var directive = m.Groups["directive"].Value;
                var body = m.Groups["body"].Value;
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
                        sb.Append(m.Value);
                        break;
                }
            // Comments are ignored.
            }else if(m.Groups["multilineComment"].Success || m.Groups["singleineComment"].Success) {
                continue;
            
            // Strings and Newlines are left in tact
            }else if(m.Groups["string"].Success || m.Groups["newline"].Success) {
                sb.Append(m.Value);
            
            // Code or unknown is ran through the define system
            }else if(m.Groups["code"].Success || m.Groups["unknown"].Success) {
                string body = m.Value;
                foreach(var define in Define.Macros) {
                    body = define.Match.Replace(body, define.Replace);
                }
                sb.Append(body);
            }else{
                // We shouldn't be here.
                throw new("Unknown match: " + m.Value);
            }
        }
        return sb.ToString();
    }
}