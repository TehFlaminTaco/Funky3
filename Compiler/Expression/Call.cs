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
        Parser.RegisterFurthest(i);
        i++;
        var arguments = new List<Expression>();
        while(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != ")") {
            Parser.RegisterFurthest(i);
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
        Parser.RegisterFurthest(i);
        i++;
        var call = new Call(left, arguments);
        return (call, i);
    }


    public override string Generate(string stackName, StreamWriter header) {
        string vArgs = UniqueValueName("args");
        string vMethod = UniqueValueName("method");

        string s = $"{{Var* {vArgs} = VarNewList();\n";
        // Assemble arguments
        int i = 0;
        foreach(var argument in Arguments) {
            string argName = UniqueValueName($"arg{i}");
            s += $"Var* {argName} = &NIL;\n";
            s += argument.Generate(argName, header);
            s += $"VarRawSet({vArgs}, VarNewNumber({i++}), {argName});\n";
        }
        s += $"Var* {vMethod} = &NIL;\n";
        s += Method.Generate(vMethod, header);
        // Call method
        s += $"{stackName} = VarFunctionCall({vMethod}, {vArgs});}}\n";
        return s;
    }
}