using System.Text.RegularExpressions;
public static class Undef
{
    public static readonly Regex MatchReplaceTemplate = new(@"^\/(?<Match>(?:\\.|.)*)$");
    public static readonly Regex MatchReplaceSimple = new(@"^\s+(?<Match>\S+)$");
    public static void Parse(string body)
    {
        var match = MatchReplaceTemplate.Match(body);
        if (match.Success)
        {
            Define.Macros.RemoveAll(c => c.Match.ToString() == match.Groups["Match"].Value);
            return;
        }

        // $undef <match> <replace>
        match = MatchReplaceSimple.Match(body);
        if (match.Success)
        {
            Define.Macros.RemoveAll(c => c.Match.ToString() == match.Groups["Match"].Value);
            return;
        }

        throw new Exception($"Invalid define: {body}");
    }
}