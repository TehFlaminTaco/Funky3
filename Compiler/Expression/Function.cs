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
        int start = tokens[index].Index;
        int end = tokens[body.index-1].Index + tokens[body.index-1].Length;
        string code = Compiler.CurrentCode[start..end];
        return (new Function(storeVariable.Item1, arguments, body.expression, code), body.index);
    }

    public override string GenerateInline(StreamWriter header, out string stackName) {
        // Write the method to the header.
        string methodName = UniqueValueName("method");
        string argIndex = UniqueValueName($"argIndex");
        header.WriteLine("// Function Definition");
        header.WriteLine($"Var* {methodName}(Var* scope, Var* args){{");
        header.WriteLine($"\tint {argIndex} = 0;");
        // Add the args to the scope.
        // VarRawSet(scope, VarNewString( argument name ), args[i]);
        for(int i = 0; i < Arguments.Count; i++) {
            string argName = UniqueValueName($"arg{i}");
            header.WriteLine($"\t// Argument: {Arguments[i].Name} ");
            // If it's splatted, Dump the rest of the arguments into this in particular.
            if(Arguments[i].Splat) {
                header.WriteLine($"\t\tVar* {argName} = VarListCopyLShifted(args, {argIndex});");
                // Remove all arguments by name from the list.
                for(int j = 0; j < Arguments.Count; j++) {
                    header.WriteLine($"\t\tVarRawSet({argName}, VarNewString(\"{Arguments[j].Name}\"), &UNDEFINED);");
                }
            }else{
                header.WriteLine($"\t\tVar* {argName} = VarRawGet(args, VarNewNumber({argIndex}++));");
                header.WriteLine($"\t\tif(ISUNDEFINED({argName})) {{");
                if(Arguments[i].DefaultValue != null) {
                    string argOut;
                    string argBody = Arguments[i].DefaultValue!.GenerateInline(header, out argOut);
                    if(!String.IsNullOrEmpty(argBody)){
                        header.Write(Tabbed(Tabbed(Tabbed($"{argBody}"))));
                    }
                    header.WriteLine($"\t\t\t{argName} = {argOut};");
                }else{
                    header.WriteLine($"\t\t\t{argName} = &NIL;");
                }
                header.WriteLine("\t\t}");
            }
            header.WriteLine($"\t\tVarRawSet(scope, VarNewString(\"{Arguments[i].Name}\"), {argName});");
        }
        string retVal;
        string body = Body.GenerateInline(header, out retVal);
        if(!String.IsNullOrEmpty(body)) {
            header.Write(Tabbed(body));
        }
        header.WriteLine($"\treturn {retVal};");
        header.WriteLine($"}}");

        StringBuilder sb = new StringBuilder();
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
            stackName = $"VarSet(scope, VarNewString(\"{StoreVariable!.Name}\"), VarNewFunctionWithScope({methodName}, scope, {constFunctionBody}))";
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
        if(tokens[i].Type == TokenType.Punctuation && tokens[i].Value == "=") {
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