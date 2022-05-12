using System.Text;
using System.Text.RegularExpressions;
public abstract class Expression {
    public abstract string GenerateInline(StreamWriter header, out string stackName);
    protected static Regex Lines = new Regex(@".+", RegexOptions.Multiline);
    public static string Tabbed(string text){
        return Lines.Replace(text,  "\t$0");
    }

    public static HashSet<string> TakenNames = new HashSet<string>();
    public static string UniqueValueName(string prefix){
        string name = prefix + "_" + Guid.NewGuid().ToString("N")[..6];
        while(TakenNames.Contains(name)) {
            name = prefix + "_" + Guid.NewGuid().ToString("N")[..6];
        }
        TakenNames.Add(name);
        return name;
    }

    public static (Expression? expression, int index) TryParseAny(List<Token> tokens, int index) {
        if(index == tokens.Count) {
            return (null, index);
        }

        switch (tokens[index].Type) {
            case TokenType.Number:      return RightParse(NumberLiteral.TryParse(tokens, index), tokens);
            case TokenType.String:      return RightParse(StringLiteral.TryParse(tokens, index), tokens);
            case TokenType.Identifier:
                // Check if this is a function first.
                if(tokens.Count > index+3 && tokens[index+1].Type==TokenType.Punctuation && tokens[index+1].Value=="=" && tokens[index+2].Type==TokenType.Punctuation && tokens[index+2].Value==">") {
                    (Expression? expression, int index) result = Function.TryParse(tokens, index);
                    if(result.expression != null) {
                        return result;
                    }
                }
                // Otherwise, it's a variable.
                return RightParse(Variable.TryParse(tokens, index), tokens);
            case TokenType.Keyword:
                switch (tokens[index].Value) {
                    case "var": case "local":   return RightParse(Variable.TryParse(tokens, index), tokens);
                    case "not":                 return RightParse(Math.TryParse(tokens, index), tokens);
                    case "function":            return RightParse(Function.TryParse(tokens, index), tokens);
                    case "with":                return RightParse(With.TryParse(tokens, index), tokens);
                    case "$CodeChunk":          return RightParse(CodeChunk.TryParse(tokens, index), tokens);
                    default:                    return (null, index);
                }
            case TokenType.Punctuation:
                switch (tokens[index].Value) {
                    case "{":   return RightParse(Block.TryParse(tokens, index), tokens);
                    case "(":
                        // Try a function first
                        var function = Function.TryParse(tokens, index);
                        if(function.Item1 != null) {
                            return RightParse(function, tokens);
                        }
                        // Otherwise, try a parenthesized expression
                        return RightParse(Parentheses.TryParse(tokens, index), tokens);
                    default:    return (null, index);
                }
            default:
                return (null, index);
        }
    }

    public static (Expression? expression, int index) RightParse((Expression? expression, int index) result, List<Token> tokens) {
        if(result.expression == null)
            return result;
        if(result.index >= tokens.Count)
            return result;
        
        switch (tokens[result.index].Type) {
            case TokenType.Punctuation:
                switch (tokens[result.index].Value) {
                    case "(":
                        return RightParse(Call.TryParse(result.expression, tokens, result.index), tokens);
                    case "=":
                        // Try and get math first.
                        (Expression? expression, int index) math = Math.TryParse(result.expression, tokens, result.index);
                        if(math.expression != null) {
                            return RightParse(Assignment.TryParse(math.expression, tokens, math.index), tokens);
                        }
                        // Otherwise, get an assignment.
                        return RightParse(Assignment.TryParse(result.expression, tokens, result.index), tokens);
                    case "+": case "-":
                        // Either a Crementor or math.
                        // Crementors need to be implemented later.
                        (Expression? expression, int index) math2 = Math.TryParse(result.expression, tokens, result.index);
                        if(math2.expression != null)
                            return RightParse(math2, tokens);
                        return result;
                    case "*": case "/": case "%": case "^": case "&": case "|": case "~": case "<": case ">":
                        // Math only punctuation
                        return RightParse(Math.TryParse(result.expression, tokens, result.index), tokens);
                    case ".":
                        // Try and get math first
                        (Expression? expression, int index) math3 = Math.TryParse(result.expression, tokens, result.index);
                        if(math3.expression != null) 
                            return RightParse(math3, tokens);
                        // try to get an indexer
                        (Expression? expression, int index) indexer = IndexVariable.TryParse(result.expression, tokens, result.index);
                        if(indexer.expression != null)
                            return RightParse(indexer, tokens);
                        return result;
                }
                return result;
            case TokenType.Keyword:
                switch (tokens[result.index].Value) {
                    case "and": case "or": 
                        // Math only keywords
                        return RightParse(Math.TryParse(result.expression, tokens, result.index), tokens);
                }
                return result;
            default:
                return result;
        }
    }
}