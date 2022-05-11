using System.Text.RegularExpressions;
public static class Tokenizer {
    // To be expanded as keywords are added/reserved
    private static readonly string[] keywords = new string[] {
        "for", "if", "function",
        "var", "local",
        "and", "or", "not",
    };

    private static readonly Regex NUMBER = new(@"^(?<negative>-?)(?:(?<integer>0(?:x(?<hex_val>[0-9A-Fa-f]+)|b(?<bin_val>[01]+)))|(?:(?<float>(?<int_comp>\d*)\.(?<float_comp>\d+))|(?<int>\d+))(?:e(?<expon>-?\d+))?)");

    public static List<Token> Tokenize(string code) {
        var tokens = new List<Token>();
        int k = 0;

        while(k < code.Length) {
            char firstChar = code[k];
            
            // Keywords or Identifiers
            if (char.IsLetter(firstChar) || firstChar == '_') {
                var matchedKeywords = keywords.Where(c=>c.StartsWith(firstChar.ToString())).OrderByDescending(c=>c.Length).Where(c=>code.Substring(k).StartsWith(c));
                if (matchedKeywords.Any()) {
                    // Return the *longest* matched keyword
                    int line = code[..k].Split('\n').Length;
                    int column = k - code[..k].LastIndexOf('\n');
                    tokens.Add(new Token(TokenType.Keyword, matchedKeywords.First(), line, column, k, matchedKeywords.First().Length));
                    k += matchedKeywords.First().Length;
                }else{
                    // Identifier
                    // Continue until the next non-letter, digit, or underscore
                    int end = k;
                    while (end < code.Length && (char.IsLetterOrDigit(code[end]) || code[end] == '_')) {
                        end++;
                    }
                    // Generate the token
                    int line = code[..k].Split('\n').Length;
                    int column = k - code[..k].LastIndexOf('\n');
                    tokens.Add(new Token(TokenType.Identifier, code[k..end], line, column, k, end - k));
                    // Move the index to the end of the identifier
                    k = end;
                }


            // Comments
            } else if (firstChar == '$'){
                if(code[k+1] == '*'){ // Multiline comment
                    // Find next instance of *$
                    int end = code.IndexOf("*$", k + 2);
                    if(end == -1)
                        end = code.Length - 2;
                    // Generate the token
                    //int line = code[..k].Split('\n').Length;
                    //int column = k - code[..k].LastIndexOf('\n');
                    //tokens.Add(new Token(TokenType.Comment, code.Substring(k, end - k + 2), line, column));
                    // Move the index to the end of the comment
                    k = end + 2;
                }else if(code[k+1] == '$'){ // Singleline Comment
                    // Find the next newline
                    int end = code.IndexOf('\n', k + 2);
                    if(end == -1)
                        end = code.Length - 1;
                    // Generate the token
                    //int line = code[..k].Split('\n').Length;
                    //int column = k - code[..k].LastIndexOf('\n');
                    //tokens.Add(new Token(TokenType.Comment, code.Substring(k, end - k + 1), line, column));
                    // Move the index to the end of the comment
                    k = end + 2;
                }else{
                    // Generate the token
                    int line = code[..k].Split('\n').Length;
                    int column = k - code[..k].LastIndexOf('\n');
                    tokens.Add(new Token(TokenType.Punctuation, firstChar.ToString(), line, column, k, 1));
                    // Move the index to the end of the punctuation
                    k++;
                }
            
            // Ignore Whitespace
            }else if (char.IsWhiteSpace(firstChar)) {
                k++;
            
            // Number
            // No easy way to do this with how complex I like my numbers
            // Going to use an Old Regular Expression.
            } else if (NUMBER.IsMatch(code[k..])) {
                var match = NUMBER.Match(code[k..]);
                // Generate the token
                int line = code[..k].Split('\n').Length;
                int column = k - code[..k].LastIndexOf('\n');
                tokens.Add(new Token(TokenType.Number, match.Value, line, column, k, match.Value.Length));
                // Move the index to the end of the number
                k += match.Value.Length;

            // String
            } else if (firstChar == '"' || firstChar == '\'') {
                // Find the next instance of the same quote, not after an unescaped \
                int end = k+1;
                while (end < code.Length && code[end] != firstChar) {
                    if (code[end] == '\\' && code[end + 1] == firstChar) {
                        end += 2;
                    } else {
                        end++;
                    }
                }
                // Generate the token
                int line = code[..k].Split('\n').Length;
                int column = k - code[..k].LastIndexOf('\n');
                tokens.Add(new Token(TokenType.String, code.Substring(k, end - k + 1), line, column, k, end + 1 - k));
                // Move the index to the end of the string
                k = end + 1;

            // Interpolated strings are currently in the Naughty Corner for crimes against my brain.
            // I'm not sure if I'll ever get around to implementing them.
            // Interpolated String
            } else if (firstChar == '`') {
                // Find the next instance of the same quote, not after an unescaped \
                int end = k+1;
                while (end < code.Length && code[end] != '`') {
                    if (code[end] == '\\' && code[end + 1] == '`') {
                        end += 2;
                    } else {
                        end++;
                    }
                }
                // Generate the token
                int line = code[..k].Split('\n').Length;
                int column = k - code[..k].LastIndexOf('\n');
                tokens.Add(new Token(TokenType.InterpolatedStringSTART, code.Substring(k, end - k + 1), line, column, k, end + 1 - k));
                // Move the index to the end of the string
                k = end + 1;
                throw new NotImplementedException("Interpolated Strings are not yet implemented.");
            // Punctuation
            } else {
                // Generate the token
                int line = code[..k].Split('\n').Length;
                int column = k - code[..k].LastIndexOf('\n');
                tokens.Add(new Token(TokenType.Punctuation, firstChar.ToString(), line, column, k, 1));
                // Move the index to the end of the punctuation
                k++;
            }
        }
        return tokens;
    }
}

public enum TokenType {
    Comment,
    Keyword,
    Identifier,
    Number,
    String,
    InterpolatedStringSTART,
    InterpolatedStringTEXT,
    InterpolatedStringEND,
    Punctuation
}

public class Token {
    public TokenType Type { get; set; }
    public string Value { get; set; }
    public int Line { get; set; }
    public int Column { get; set; }
    public int Index { get; set; }
    public int Length { get; set; }

    public override string ToString() {
        return $"{Type} {Value}";
    }

    public Token(TokenType type, string value, int line, int column, int index, int length) {
        Type = type;
        Value = value;
        Line = line;
        Column = column;
        Index = index;
        Length = length;
    }
}