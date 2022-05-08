public class Function : Expression {
    public Variable? StoreVariable { get; set; }
    public List<Expression> Arguments { get; set; }
    public Expression Body { get; set; }

    public Function(Variable? storeVariable, List<Expression> arguments, Expression body) {
        StoreVariable = storeVariable;
        Arguments = arguments;
        Body = body;
    }

    public static (Function?, int) TryParse(List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.Keyword || tokens[i].Value != "function") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;
        if(tokens[i].Type != TokenType.Identifier) {
            return (null, index);
        }
        // Try to get a Variable.
        var storeVariable = Variable.TryParse(tokens, i);
        if(storeVariable.Item1 != null) {
            i = storeVariable.Item2;
        }
        if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "(") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;
        var arguments = new List<Expression>();
        while(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != ")") {
            var argument = Expression.TryParseAny(tokens, i);
            if(argument.expression == null) {
                return (null, index);
            }
            arguments.Add(argument.expression);
            i = argument.index;
            // Skip over a comma.
            if(tokens[i].Type == TokenType.Punctuation && tokens[i].Value == ",") {
                i++;
            }
            Parser.RegisterFurthest(i);
            i++;
        }
        Parser.RegisterFurthest(i);
        i++;
        var body = TryParseAny(tokens, i);
        if(body.expression == null) {
            return (null, index);
        }
        return (new Function(storeVariable.Item1, arguments, body.expression), body.index);
    }

    public override string Generate(string stackName, StreamWriter header) {
        // Write the method to the header.
        string methodName = UniqueValueName("method");
        header.WriteLine($"Var* {methodName}(Var* scope, Var* args){{");

        header.WriteLine($"}}");
    }

}