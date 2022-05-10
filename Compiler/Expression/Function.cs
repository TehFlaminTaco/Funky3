using System.Text;
public class Function : Expression {
    public Variable? StoreVariable { get; set; }
    public List<FunctionArgument> Arguments { get; set; }
    public Expression Body { get; set; }

    public Function(Variable? storeVariable, List<FunctionArgument> arguments, Expression body) {
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
        return (new Function(storeVariable.Item1, arguments, body.expression), body.index);
    }

    public override string Generate(string stackName, StreamWriter header) {
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
                header.WriteLine($"\tVar* {argName} = VarListCopyLShifted(args, {argIndex});");
                // Remove all arguments by name from the list.
                for(int j = 0; j < Arguments.Count; j++) {
                    header.WriteLine($"\tVarRawSet({argName}, VarNewString(\"{Arguments[j].Name}\"), &UNDEFINED);");
                }
            }else{
                header.WriteLine($"\tVar* {argName} = VarRawGet(args, VarNewNumber({argIndex}++));");
                header.WriteLine($"\tif(ISUNDEFINED({argName})) {{");
                if(Arguments[i].DefaultValue != null) {
                    header.WriteLine("\t" + Arguments[i].DefaultValue!.GenerateTabbed(argName, header));
                }else{
                    header.WriteLine($"\t{argName} = &NIL;");
                }
                header.WriteLine("\t}");
            }
            header.WriteLine($"\tVarRawSet(scope, VarNewString(\"{Arguments[i].Name}\"), {argName});");
        }
        header.WriteLine("\tVar* result = &NIL;");
        header.WriteLine(Body.GenerateTabbed("result", header));
        header.WriteLine($"}}");

        string functionScope = UniqueValueName("functionScope");
        string functionHolder = UniqueValueName("functionHolder");
        StringBuilder sb = new StringBuilder();
        sb.AppendLine("// Add Function to Scope");
        sb.AppendLine($"\tVar* {functionScope} = VarSubScope(scope);");
        sb.AppendLine($"\tVar* {functionHolder} = VarNewFunction({methodName});"); 
        sb.AppendLine($"\t((VarFunction*)({functionHolder} -> value)) -> scope = {functionScope};");
        sb.AppendLine($"\tDoReferenceBy({functionScope}, {functionHolder});");
        if(StoreVariable != null) {
            sb.AppendLine($"\tVarRawSet(scope, VarNewString(\"{StoreVariable.Name}\"), {functionHolder});");
        }
        sb.AppendLine($"\t{stackName} = {functionHolder};");
        return sb.ToString();
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