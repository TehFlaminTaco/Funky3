public class Call : Expression {
    public Expression Method { get; set; }
    public List<Expression> Arguments { get; set; }

    public Call(Expression method, List<Expression> arguments) {
        Method = method;
        Arguments = arguments;
    }

    public static (Call?, int) TryParse(Expression left, List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "(") {
            return (null, index);
        }
        i++;
        var arguments = new List<Expression>();
        while(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != ")") {
            (Expression? expression, int index) result = Expression.TryParseAny(tokens, i);
            if(result.expression != null) {
                i = result.index;
                arguments.Add(result.expression);
            }else{
                return (null, index);
            }
            // Check for a comma
            if(tokens[i].Type == TokenType.Punctuation && tokens[i].Value == ",") {
                i++;
            }
        }
        i++;
        var call = new Call(left, arguments);
        return (call, i);
    }


    public override void Generate(StreamWriter body, StreamWriter header) {
    }
}