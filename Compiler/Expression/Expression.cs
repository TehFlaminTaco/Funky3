public abstract class Expression {
    public abstract void Generate(StreamWriter body, StreamWriter header);

    public static (Expression? expression, int index) TryParseAny(List<Token> tokens, int index) {
        if(index == tokens.Count) {
            return (null, index);
        }
        (Expression? expression, int index) result;

        // Try to parse a variable
        result = Variable.TryParse(tokens, index);
        if(result.expression != null) { return RightParse(result, tokens, index);}

        // Try to parse a string literal
        result = StringLiteral.TryParse(tokens, index);
        if(result.expression != null) { return RightParse(result, tokens, index); }

        return (null, index);
    }

    public static (Expression? expression, int index) RightParse((Expression? expression, int index) result, List<Token> tokens, int index) {
        if(result.expression == null)
            return result;
        
        (Expression? expression, int index) result2;
        result2 = Call.TryParse(result.expression, tokens, result.index);
        if(result2.expression != null) { return RightParse(result2, tokens, result2.index); }

        return result;

    }
}