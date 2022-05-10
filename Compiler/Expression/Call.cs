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

    public override string GenerateInline(StreamWriter header, out string stackName) {
        // Impossible to inline.
        string retValue = UniqueValueName("retValue");
        string vArgs = UniqueValueName("args");
        string vMethod;
        StringBuilder sb = new StringBuilder();
        sb.AppendLine("// Call");
        sb.AppendLine($"\tVar* {retValue};");
        sb.AppendLine($"\tVar* {vArgs} = VarNewList();");
        // Assemble arguments
        int i = 0;
        foreach(var argument in Arguments) {
            sb.AppendLine($"\t// Arg {i}");
            string argName;
            string argBody = argument.GenerateInline(header, out argName);
            if(!String.IsNullOrEmpty(argBody)) {
                sb.Append(Tabbed(argBody));
            }
            sb.AppendLine($"\tVarRawSet({vArgs}, VarNewNumber({i++}), {argName});");
        }
        string methodBody = Method.GenerateInline(header, out vMethod);
        if(!String.IsNullOrEmpty(methodBody)) {
            sb.Append(Tabbed(methodBody));
        }
        // Call method
        sb.AppendLine($"\t{retValue} = VarFunctionCall({vMethod}, {vArgs});");
        stackName = retValue;
        return sb.ToString();
    }
}