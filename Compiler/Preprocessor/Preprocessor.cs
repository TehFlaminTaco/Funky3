using System.Text.RegularExpressions;
using System.Text;
public class Preprocessor
{
    public static readonly Regex IsDirective = new(@"^\s*\$(?<directive>\w+)(?<body>.*)$");

    // This is used to quickly chunk up the code for preprocessing.
    public static readonly Regex SuperTokenizer = new(@"
(?<newline>          [\n\r]                             )| (?#Newlines...)
(?<multilineComment> \$\*(.|\s)*?\*\$                   )| (?#Multiline Comment)
(?<string>           (?<qoute>[\""'`])(\\.|.)*?\<qoute> )| (?#Strings)
(?<singleineComment> \$\$.*                             )| (?#Singleline Comment)
(?<preprocessor>     \s*\$(?<directive>\w+)(?<body>.*)  )| (?#Preprocessor)
(?<code>             [^$\""'\r\n]+                      )| (?#Regular, Boring code)
(?<unknown>          .                                  )  (?#Incase I missed something)",
RegexOptions.Multiline | RegexOptions.IgnorePatternWhitespace);

    public static bool Skipping = false;
    public static StringBuilder PreprocBuffer = new();
    public static string Process(string allCode)
    {
        StringBuilder sb = new();
        int ifDepth = 0;
        // For each chunk of code
        foreach (Match m in SuperTokenizer.Matches(allCode))
        {
            if (Skipping && !m.Groups["preprocessor"].Success)
            {
                PreprocBuffer.Append(m.Value);
                continue;
            }
            // If it's a preprocessor directive
            if (m.Groups["preprocessor"].Success)
            {
                var directive = m.Groups["directive"].Value;
                var body = m.Groups["body"].Value;
                switch (directive)
                {
                    case "cInlineStart":
                    case "cHeaderStart":
                        if (ifDepth > 0) break;
                        Skipping = true;
                        PreprocBuffer = new();
                        break;
                    case "cHeaderEnd":
                        if (ifDepth > 0) break;
                        CHeader.Parse(PreprocBuffer.ToString(), sb);
                        Skipping = false;
                        break;
                    case "cInlineEnd":
                        if (ifDepth > 0) break;
                        CInline.Parse(PreprocBuffer.ToString(), sb);
                        Skipping = false;
                        break;
                    case "define":
                        if (Skipping) break;
                        Define.Parse(body);
                        break;
                    case "undef":
                        if (Skipping) break;
                        Undef.Parse(body);
                        break;
                    case "ifdef":
                        if (Skipping && ifDepth == 0) break;
                        if (ifDepth > 0) ifDepth++;
                        else if (
                            body.StartsWith("/") ? !Define.Macros.Any(c => c.Match.ToString() == body.Substring(1)) : !Define.Macros.Any(c => c.Match.ToString() == body.TrimStart()))
                        {
                            Skipping = true;
                            ifDepth++;
                        }
                        break;
                    case "ifndef":
                        if (Skipping && ifDepth == 0) break;
                        if (ifDepth > 0) ifDepth++;
                        else if (
                            body.StartsWith("/") ? Define.Macros.Any(c => c.Match.ToString() == body.Substring(1)) : Define.Macros.Any(c => c.Match.ToString() == body.TrimStart()))
                        {
                            Skipping = true;
                            ifDepth++;
                        }
                        break;
                    case "elif":
                        if (Skipping && ifDepth == 0) break;
                        if (ifDepth == 0)
                        {
                            Skipping = true;
                            ifDepth++;
                        }
                        else if (ifDepth == 1)
                        {
                            Skipping = false;
                            ifDepth = 0;
                        }
                        break;
                    case "endif":
                        if (Skipping && ifDepth == 0) break;
                        if (ifDepth > 0)
                        {
                            ifDepth--;
                            if (ifDepth == 0) Skipping = false;
                        }
                        break;
                    default:
                        sb.Append(m.Value);
                        break;
                }
                // Comments are ignored.
            }
            else if (m.Groups["multilineComment"].Success || m.Groups["singleineComment"].Success)
            {
                continue;

                // Strings and Newlines are left in tact
            }
            else if (m.Groups["string"].Success || m.Groups["newline"].Success)
            {
                sb.Append(m.Value);

                // Code or unknown is ran through the define system
            }
            else if (m.Groups["code"].Success || m.Groups["unknown"].Success)
            {
                string body = m.Value;
                foreach (var define in Define.Macros)
                {
                    body = define.Match.Replace(body, define.Replace);
                }
                sb.Append(body);
            }
            else
            {
                // We shouldn't be here.
                throw new("Unknown match: " + m.Value);
            }
        }
        return sb.ToString();
    }
}