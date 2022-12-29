using System.Text.RegularExpressions;
public class Define
{
    public Regex Match { get; set; }
    public string Replace { get; set; }

    public static readonly Regex MatchReplaceTemplate = new(@"^\/(?<Match>(?:\\.|.)*)\/(?<Replace>(?:\\.|.)*)$");
    public static readonly Regex MatchReplaceSimple = new(@"^\s+(?<Match>\S+)(?:\s+(?<Replace>.*))?$"); // TODO: C style macro arguments?

    public Define(string match, string replace)
    {
        Match = new Regex(match);
        Replace = replace;
    }

    public static readonly List<Define> Macros = new();

    public static void Parse(string body)
    {
        // A define takes the form of 
        // $define/someregex/somereplacement

        // body is just the /someregex/somereplacement part
        var match = MatchReplaceTemplate.Match(body);
        if (match.Success)
        {
            Macros.RemoveAll(c => c.Match.ToString() == match.Groups["Match"].Value);
            Macros.Add(new Define(match.Groups["Match"].Value, match.Groups["Replace"].Value));
            return;
        }

        // $define <match> <replace>
        match = MatchReplaceSimple.Match(body);
        if (match.Success)
        {
            Macros.RemoveAll(c => c.Match.ToString() == match.Groups["Match"].Value);
            Macros.Add(new Define(match.Groups["Match"].Value, match.Groups["Replace"].Value));
            return;
        }

        throw new Exception($"Invalid define: {body}");
    }
}