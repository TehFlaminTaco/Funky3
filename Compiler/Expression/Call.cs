using System.Text;
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
        StringBuilder sb = new StringBuilder();
        sb.AppendLine("// Call");
        sb.AppendLine($"\tVar* {vArgs} = VarNewList();");
        // Assemble arguments
        int i = 0;
        foreach(var argument in Arguments) {
            string argName = UniqueValueName($"arg{i}");
            sb.AppendLine($"\tVar* {argName} = &NIL;");
            sb.Append(argument.GenerateTabbed(argName, header));
            sb.AppendLine($"\tVarRawSet({vArgs}, VarNewNumber({i++}), {argName});");
        }
        sb.AppendLine($"\tVar* {vMethod} = &NIL;");
        sb.Append(Method.GenerateTabbed(vMethod, header));
        // Call method
        sb.AppendLine($"\t{stackName} = VarFunctionCall({vMethod}, {vArgs});");
        return sb.ToString();
    }
}