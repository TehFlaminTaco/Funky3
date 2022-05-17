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

    private static Stack<HashSet<Type>> BlockedTypes = new();
    public static void PushBlock(HashSet<Type> types) {
        BlockedTypes.Push(types);
    }
    public static void PopBlock() {
        BlockedTypes.Pop();
    }
    public static bool IsBlocked(Type type) {
        if(!BlockedTypes.Any()) {
            return false;
        }
        return BlockedTypes.Peek().Contains(type);
    }
    public static bool IsBlocked<T>() {
        return IsBlocked(typeof(T));
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
                    case "for":
                        // Try a forIn first
                        (Expression? expression, int index) result = ForIn.TryParse(tokens, index);
                        if(result.expression != null) {
                            return result;
                        }
                        // Otherwise, it's a for loop.
                        return RightParse(For.TryParse(tokens, index), tokens);
                    case "function":            return RightParse(Function.TryParse(tokens, index), tokens);
                    case "if":                  return RightParse(If.TryParse(tokens, index), tokens);
                    case "while":               return RightParse(While.TryParse(tokens, index), tokens);
                    case "with":                return RightParse(With.TryParse(tokens, index), tokens);
                    case "return": case "break":return RightParse(Return.TryParse(tokens, index), tokens);
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
                        // Then, try an unpack
                        if(!IsBlocked<UnpackVariable>()){
                            var unpack = UnpackVariable.TryParse(tokens, index);
                            if(unpack.Item1 != null) {
                                return RightParse(unpack, tokens);
                            }
                        }
                        // Otherwise, try a parenthesized expression
                        if(!IsBlocked(typeof(Parentheses))){
                            return RightParse(Parentheses.TryParse(tokens, index), tokens);
                        }
                        return (null, index);
                    case "[":
                        // List literal
                        return RightParse(ListLiteral.TryParse(tokens, index), tokens);
                    case "+": case "-":
                        // Maybe a crementor, otherwise math.
                        if(!IsBlocked<Crementor>()){
                            var crementor = Crementor.TryParse(tokens, index);
                            if(crementor.Item1 != null) {
                                return RightParse(crementor, tokens);
                            }
                        }
                        return RightParse(Math.TryParse(tokens, index), tokens);
                    case "!": case "~": case "#":
                        return RightParse(Math.TryParse(tokens, index), tokens);
                    case "@":
                        return RightParse(DeOperator.TryParse(tokens, index), tokens);
                    default:    return (null, index);
                }
            default:
                return (null, index);
        }
    }

    public static (Expression? expression, int index) RightParse((Expression? expression, int index) result, List<Token> tokens, (Expression? expression, int index) left = default) {
        if(result.expression == null)
            return left;
        if(result.index >= tokens.Count)
            return left;

        if(result.expression is IExpressionProvider iep){
            result.expression = IExpressionProvider.BalanceEither(iep);
        }

        switch (tokens[result.index].Type) {
            case TokenType.Punctuation:
                switch (tokens[result.index].Value) {
                    case "(":
                        if(IsBlocked<Call>())
                            return result;
                        return RightParse(Call.TryParse(result.expression, tokens, result.index), tokens, result);
                    case "=":
                        // Try and get math first.
                        if(!IsBlocked<Math>()) {
                            (Expression? expression, int index) math = Math.TryParse(result.expression, tokens, result.index);
                            if(math.expression != null) {
                                return RightParse(math, tokens, result);
                            }
                        }
                        // Otherwise, get an assignment.
                        if(IsBlocked<Assignment>())
                            return result;
                        return RightParse(Assignment.TryParse(result.expression, tokens, result.index), tokens, result);
                    case "+": case "-":
                        // Either a Crementor or math.
                        if(!IsBlocked<Crementor>() && result.expression is Variable variable) {
                            (Expression? expression, int index) crementor = Crementor.TryParse(variable, tokens, result.index);
                            if(crementor.expression != null) {
                                return RightParse(crementor, tokens, result);
                            }
                        }
                        if(!IsBlocked<Assignment>()) {
                            (Expression? expression, int index) assignment = Assignment.TryParse(result.expression, tokens, result.index);
                            if(assignment.expression != null) {
                                return RightParse(assignment, tokens, result);
                            }
                        }

                        if(!IsBlocked<Math>()) {
                            (Expression? expression, int index) math2 = Math.TryParse(result.expression, tokens, result.index);
                            if(math2.expression != null)
                                return RightParse(math2, tokens, result);
                        }
                        return result;
                    case "*": case "/": case "%": case "^": case "&": case "|": case "~": case "<": case ">":
                        // Might be an assignment. Check just in case. +=, -=, etc.
                        if(!IsBlocked<Assignment>()) {
                            (Expression? expression, int index) assignment = Assignment.TryParse(result.expression, tokens, result.index);
                            if(assignment.expression != null) {
                                return RightParse(assignment, tokens, result);
                            }
                        }
                        // Math only punctuation
                        if(IsBlocked<Math>())
                            return result;
                        return RightParse(Math.TryParse(result.expression, tokens, result.index), tokens, result);
                    case ".":
                        // Incase it's a concat assignemnt. Check first a..=3
                        if(!IsBlocked<Assignment>()) {
                            (Expression? expression, int index) concat = Assignment.TryParse(result.expression, tokens, result.index);
                            if(concat.expression != null) {
                                return RightParse(concat, tokens, result);
                            }
                        }
                        // Try and get math first
                        if(!IsBlocked<Math>()) {
                            (Expression? expression, int index) math3 = Math.TryParse(result.expression, tokens, result.index);
                            if(math3.expression != null) 
                                return RightParse(math3, tokens, result);
                        }
                        // try to get an indexer
                        if(!IsBlocked<IndexVariable>()){
                            (Expression? expression, int index) indexer = IndexVariable.TryParse(result.expression, tokens, result.index);
                            if(indexer.expression != null)
                                return RightParse(indexer, tokens, result);
                        }
                        return result;
                    case ":":
                        // Indexer only
                        if(IsBlocked<IndexVariable>())
                            return result;
                        return RightParse(IndexVariable.TryParse(result.expression, tokens, result.index), tokens, result);
                    case "[":
                        // Indexer Only
                        if(IsBlocked<IndexVariable>())
                            return result;
                        return RightParse(IndexVariable.TryParse(result.expression, tokens, result.index), tokens, result);
                    case "@":
                        // Deoperator call
                        if(IsBlocked<Call>() || IsBlocked<DeOperator>())
                            return result;
                        return RightParse(Call.TryParse(result.expression, tokens, result.index), tokens, result);
                    case "?":
                        // If statement, Ternary style.
                        if(IsBlocked<If>())
                            return result;
                        return RightParse(If.TryParse(result.expression, tokens, result.index), tokens, result);
                }
                return result;
            case TokenType.Keyword:
                switch (tokens[result.index].Value) {
                    case "and": case "or": 
                        // Try an assignment first, Just in case
                        if(!IsBlocked<Assignment>()) {
                            (Expression? expression, int index) assignment = Assignment.TryParse(result.expression, tokens, result.index);
                            if(assignment.expression != null) {
                                return RightParse(assignment, tokens, result);
                            }
                        }
                        // Math only keywords
                        if(IsBlocked<Math>())
                            return result;
                        return RightParse(Math.TryParse(result.expression, tokens, result.index), tokens, result);
                }
                return result;
            case TokenType.String:
                // String call
                if(IsBlocked<Call>() || IsBlocked<StringLiteral>())
                    return result;
                return RightParse(Call.TryParse(result.expression, tokens, result.index), tokens, result);
            default:
                return result;
        }
    }
}