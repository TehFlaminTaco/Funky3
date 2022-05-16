using System.Text;
public class Function : Expression {
    public Variable? StoreVariable { get; set; }
    public List<FunctionArgument> Arguments { get; set; }
    public Expression Body { get; set; }

    public string Code { get; set; }

    public Function(Variable? storeVariable, List<FunctionArgument> arguments, Expression body, string code) {
        StoreVariable = storeVariable;
        Arguments = arguments;
        Body = body;
        Code = code;
    }

    public static (Function?, int) TryParse(List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type == TokenType.Keyword && tokens[i].Value == "function") {
            Parser.RegisterFurthest(i);
            i++;
            return TryParseKeyword(tokens, i);
        }

        if(tokens[i].Type == TokenType.Punctuation && tokens[i].Value == "(") {
            Parser.RegisterFurthest(i);
            i++;
            return TryParseParentheses(tokens, i);
        }

        // Try and get a single argument
        (FunctionArgument? argument, int index) argument = FunctionArgument.TryParse(tokens, i);
        if(argument.argument != null) {
            Parser.RegisterFurthest(argument.index);
            i = argument.index;
            return TryParseSingleArgument(argument.argument, tokens, i);
        }

        return (null, index);
    }

    public static (Function?, int) TryParseKeyword(List<Token> tokens, int index) {
        int i = index;
        // Try to get a Variable.
        Expression.PushBlock(new(){
            typeof(Call),
            typeof(UnpackVariable)
        });
        var storeVariable = Expression.TryParseAny(tokens, i);
        Expression.PopBlock();
        if(storeVariable.Item1 != null) {
            if(storeVariable.Item1 is not Variable v) {
                return (null, index);
            }
            i = storeVariable.Item2;
        }
        if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "(") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;
        var arguments = new List<FunctionArgument>();
        while(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != ")") {
            var argument = FunctionArgument.TryParse(tokens, i);
            if(argument.Item1 == null) {
                return (null, index);
            }
            arguments.Add(argument.Item1);
            i = argument.Item2;
            if(argument.Item1.Splat){
                break;
            }
            // Skip over a comma.
            if(tokens[i].Type == TokenType.Punctuation && tokens[i].Value == ",") {
                Parser.RegisterFurthest(i);
                i++;
            }
        }
        Parser.RegisterFurthest(i);
        i++;
        var body = TryParseAny(tokens, i);
        if(body.expression == null) {
            return (null, index);
        }
        int start = tokens[index-1].Index;
        int end = tokens[body.index-1].Index + tokens[body.index-1].Length;
        string code = Compiler.CurrentCode[start..end];
        return (new Function(storeVariable.Item1 as Variable, arguments, body.expression, code), body.index);
    }

    // In the form (a, b) => body
    public static (Function?, int) TryParseParentheses(List<Token> tokens, int index) {
        int i = index;
        var arguments = new List<FunctionArgument>();
        while(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != ")") {
            var argument = FunctionArgument.TryParse(tokens, i);
            if(argument.Item1 == null) {
                return (null, index);
            }
            arguments.Add(argument.Item1);
            i = argument.Item2;
            // Skip over a comma.
            if(tokens[i].Type == TokenType.Punctuation && tokens[i].Value == ",") {
                Parser.RegisterFurthest(i);
                i++;
            }
        }
        Parser.RegisterFurthest(i);
        i++;
        if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "=") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;
        if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != ">") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;
        var body = TryParseAny(tokens, i);
        if(body.expression == null) {
            return (null, index);
        }
        int start = tokens[index-1].Index;
        int end = tokens[body.index-1].Index + tokens[body.index-1].Length;
        string code = Compiler.CurrentCode[start..end];
        return (new Function(null, arguments, body.expression, code), body.index);
    }

    // In the form a => body
    public static (Function?, int) TryParseSingleArgument(FunctionArgument argument, List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "=") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;
        if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != ">") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;
        var body = TryParseAny(tokens, i);
        if(body.expression == null) {
            return (null, index);
        }
        int start = tokens[index-1].Index;
        int end = tokens[body.index-1].Index + tokens[body.index-1].Length;
        string code = Compiler.CurrentCode[start..end];
        return (new Function(null, new List<FunctionArgument> { argument }, body.expression, code), body.index);
    }

    public override string GenerateInline(StreamWriter header, out string stackName) {
        // Write the method to the header.
        string methodName = UniqueValueName("method");
        StringBuilder headerSB = new();
        headerSB.AppendLine("// Function Definition");
        headerSB.AppendLine($"Var* {methodName}(Var* scope, Var* args){{");
        headerSB.AppendLine("\tVar* _ = &NIL;"); // _ acts as a dummy value for various things.
        // Try loading "this" into the scope... Just if it exists.
        headerSB.AppendLine("\tVarRawSet(scope, VarNewString(\"this\"), VarRawGet(args, VarNewString(\"this\")));");
        if(Arguments.Count > 0) {
            string argIndex = UniqueValueName($"argIndex");
            headerSB.AppendLine($"\tint {argIndex} = 0;");
            // Add the args to the scope.
            // VarRawSet(scope, VarNewString( argument name ), args[i]);
            for(int i = 0; i < Arguments.Count; i++) {
                string argName = UniqueValueName($"arg{i}");
                headerSB.AppendLine($"\t// Argument: {Arguments[i].Name} ");
                // If it's splatted, Dump the rest of the arguments into this in particular.
                if(Arguments[i].Splat) {
                    headerSB.AppendLine($"\t\tVar* {argName} = VarListCopyLShifted(args, {argIndex});");
                    // Remove all arguments by name from the list.
                    for(int j = 0; j < Arguments.Count; j++) {
                        headerSB.AppendLine($"\t\tVarRawSet({argName}, VarNewString(\"{Arguments[j].Name}\"), &UNDEFINED);");
                    }
                }else{
                    headerSB.AppendLine($"\t\tVar* {argName} = ArgVarGet(args, {argIndex}++, \"{Arguments[i].Name}\");");
                    headerSB.AppendLine($"\t\tif(ISUNDEFINED({argName})) {{");
                    if(Arguments[i].DefaultValue != null) {
                        string argOut;
                        string argBody = Arguments[i].DefaultValue!.GenerateInline(header, out argOut);
                        if(!String.IsNullOrEmpty(argBody)){
                            headerSB.Append(Tabbed(Tabbed(Tabbed($"{argBody}"))));
                        }
                        headerSB.AppendLine($"\t\t\t{argName} = {argOut};");
                    }else{
                        headerSB.AppendLine($"\t\t\t{argName} = &NIL;");
                    }
                    headerSB.AppendLine("\t\t}");
                }
                headerSB.AppendLine($"\t\tVarRawSet(scope, VarNewString(\"{Arguments[i].Name}\"), {argName});");
            }
        }
        string body = Body.GenerateInline(header, out string retVal);
        if (!String.IsNullOrEmpty(body)) {
            headerSB.Append(Tabbed(body));
        }
        headerSB.AppendLine($"\treturn {retVal};");
        headerSB.AppendLine($"}}");
        header.Write(headerSB.ToString());
        StringBuilder sb = new();
        sb.Append('\"');
        foreach(char c in Code) {
            switch(c){
                case '\a':
                    sb.Append("\\a"); break;
                case '\b':
                    sb.Append("\\b"); break;
                case '\x1B':
                    sb.Append("\\e"); break;
                case '\f':
                    sb.Append("\\f"); break;
                case '\n':
                    sb.Append("\\n"); break;
                case '\r':
                    sb.Append("\\r"); break;
                case '\t':
                    sb.Append("\\t"); break;
                case '\v':
                    sb.Append("\\v"); break;
                case '\\':
                    sb.Append("\\\\"); break;
                case '\'':
                    sb.Append("\\'"); break;
                case '\"':
                    sb.Append("\\\""); break;
                default:
                    if(c < 32 || c > 126){
                        sb.Append($"\\x{c:X2}");
                    }else{
                        sb.Append(c);
                    }
                    break;
            }
        }
        sb.Append('\"');
        string constFunctionBody = sb.ToString();

        if(StoreVariable != null) {
            StoreVariable!.GenerateSetterInline(header, out stackName, $"VarNewFunctionWithScope({methodName}, scope, {constFunctionBody})");
        }else{
            stackName = $"VarNewFunctionWithScope({methodName}, scope,  {constFunctionBody})";
        }
        return "";
    }

}

public class FunctionArgument{
    public string Name { get; set; }
    public Expression? DefaultValue { get; set; }
    public bool Splat { get; set; }

    public FunctionArgument(string name, Expression? defaultValue, bool splat) {
        Name = name;
        DefaultValue = defaultValue;
        Splat = splat;
    }

    public static (FunctionArgument?, int) TryParse(List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.Identifier) {
            return (null, index);
        }
        var argument = new FunctionArgument(tokens[i].Value, null, false);
        Parser.RegisterFurthest(i);
        i++;
        if(tokens[i].Type == TokenType.Punctuation && tokens[i].Value == "=" && (tokens[i+1].Type != TokenType.Punctuation || tokens[i+1].Value != ">")) {
            Parser.RegisterFurthest(i);
            i++;
            var defaultValue = Expression.TryParseAny(tokens, i);
            if(defaultValue.expression == null) {
                return (null, index);
            }
            argument.DefaultValue = defaultValue.expression;
            i = defaultValue.index;
        }
        // Splat is defined by having three .'s after the argument.
        if( argument.DefaultValue == null &&
            tokens[i].Type == TokenType.Punctuation && tokens[i].Value == "." &&
            tokens[i+1].Type == TokenType.Punctuation && tokens[i+1].Value == "." &&
            tokens[i+2].Type == TokenType.Punctuation && tokens[i+2].Value == "."
            ) {
            argument.Splat = true;
            Parser.RegisterFurthest(i+2);
            i += 3;
        }
        return (argument, i);
    }

    public override string ToString() {
        return $"{Name}{(DefaultValue != null ? " = " + DefaultValue.ToString() : "")}{(Splat ? "..." : "")}";
    }
}