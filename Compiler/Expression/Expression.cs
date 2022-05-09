using System.Text;
using System.Text.RegularExpressions;
public abstract class Expression {
    public abstract string Generate(string stackName, StreamWriter header);

    protected static Regex Lines = new Regex(@".+", RegexOptions.Multiline);
    public virtual string GenerateTabbed(string stackName, StreamWriter header) {
        // Repeated tabs
        return Lines.Replace(Generate(stackName, header),  "\t$0");
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
        return tokens[index].Type switch
        {
            TokenType.Number => RightParse(NumberLiteral.TryParse(tokens, index), tokens),
            TokenType.String => RightParse(StringLiteral.TryParse(tokens, index), tokens),
            TokenType.Identifier => RightParse(Variable.TryParse(tokens, index), tokens),
            TokenType.Keyword => tokens[index].Value switch {
                "var" or "local" => RightParse(Variable.TryParse(tokens, index), tokens),
                "not" => RightParse(Math.TryParse(tokens, index), tokens),
                "function" => RightParse(Function.TryParse(tokens, index), tokens),
                _ => (null, index),
            },
            TokenType.Punctuation => tokens[index].Value switch {
                "{" => RightParse(Block.TryParse(tokens, index), tokens),
                "(" => RightParse(Parenthesis.TryParse(tokens, index), tokens),
                _ => (null, index),
            },
            _ => (null, index),
        };
    }

    public static (Expression? expression, int index) RightParse((Expression? expression, int index) result, List<Token> tokens) {
        if(result.expression == null)
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
                        // TODO: Indexer
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