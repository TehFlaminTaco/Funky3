using System.ComponentModel;
using System.Text;

public class Variable : Expression {
    public string? Name { get; set; }
    public bool IsLocal { get; set; }

    public Variable(string name, bool isLocal) {
        Name = name;
        IsLocal = isLocal;
    }

    public Variable() {
    }

    public static (Variable?, int) TryParse(List<Token> tokens, int index) {
        int i = index;
        bool isLocal = false;
        if(tokens[index].Type == TokenType.Keyword) {
            if(tokens[index].Value == "var" || tokens[index].Value == "local") {
                Parser.RegisterFurthest(i);
                i++;
                isLocal = true;
            }else{
                return (null, index);
            }
        }
        if(tokens[i].Type != TokenType.Identifier) {
            return (null, index);
        }
        var variable = new Variable(tokens[i].Value, isLocal);
        i++;
        return (variable, i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName) {
        if(IsLocal){
            StringBuilder sb = new StringBuilder();
            string varHolder = UniqueValueName("var");
            sb.AppendLine($"// Get local:{Name}");
            sb.AppendLine($"Var* {varHolder} = VarRawGet(scope, VarNewString(\"{Name}\"));");
            sb.AppendLine($"if(ISUNDEFINED({varHolder})) {{");
            sb.AppendLine($"\t{varHolder} = VarRawSet(scope, VarNewString(\"{Name}\"), &NIL);");
            sb.AppendLine($"}}");
            stackName = varHolder;
            return sb.ToString();
        }else{
            // Can always be inlined.
            stackName = $"/* Get {Name} */ VarGet(scope, VarNewString(\"{Name}\"))";
            return "";
        }
    }

    public virtual string GenerateSetterInline(StreamWriter header, out string stackName, string value) {
        // Can always be inlined.
        // The setter never needs to make the default check, because it always sets a value.
        if(IsLocal)
            stackName = $"/* Set local:{Name} */ VarRawSet(scope, VarNewString(\"{Name}\"), {value})";
        else
            stackName = $"/* Set {Name} */ VarSet(scope, VarNewString(\"{Name}\"), {value})";
        return "";
    }
}

public class IndexVariable : Variable {
    public Expression? Index { get; set; }
    public string? Identifier { get; set; }
    public Expression Value { get; set; }
    public bool IsCurry { get; set; }

    public IndexVariable(Expression index, Expression value) : base() {
        Index = index;
        Value = value;
        IsCurry = false;
    }

    public IndexVariable(string identifier, Expression value, bool isCurry = false) : base() {
        Identifier = identifier;
        Value = value;
        IsCurry = isCurry;
    }


    // Indexing, one of `[index]`, `.name`, or `:name`
    public static (Variable?, int) TryParse(Expression left, List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.Punctuation) {
            return (null, index);
        }
        if(tokens[i].Value == "[") {
            Parser.RegisterFurthest(i);
            i++;
            {
                (Expression? expression, int index) result = Expression.TryParseAny(tokens, i);
                if(result.expression == null) {
                    return (null, index);
                }
                i = result.index;
                // Skip a right bracket
                if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "]") {
                    return (null, index);
                }
                Parser.RegisterFurthest(i);
                i++;
                var indexVariable = new IndexVariable(result.expression, left);
                return (indexVariable, i);
            }
        }
        if(tokens[i].Value == ".") {
            Parser.RegisterFurthest(i);
            i++;
            if(tokens[i].Type != TokenType.Identifier) {
                return (null, index);
            }
            var indexVariable = new IndexVariable(tokens[i].Value, left);
            Parser.RegisterFurthest(i);
            i++;
            return (indexVariable, i);
        }
        if(tokens[i].Value == ":") {
            Parser.RegisterFurthest(i);
            i++;
            if(tokens[i].Type != TokenType.Identifier) {
                return (null, index);
            }
            var indexVariable = new IndexVariable(tokens[i].Value, left, true);
            Parser.RegisterFurthest(i);
            i++;
            return (indexVariable, i);
        }
        return (null, index);
    }

    public override string GenerateInline(StreamWriter header, out string stackName) {
        // VarGet(Value, Index)
        if(Identifier != null) {
            string valueBody = Value.GenerateInline(header, out string valueStackName);
            if(!String.IsNullOrEmpty(valueBody)) {
                string resultHolder = UniqueValueName("get");
                stackName = resultHolder;
                StringBuilder sb = new StringBuilder();
                sb.AppendLine($"// Get index: {Identifier}");
                sb.AppendLine(valueBody);
                // Use VarCurryGet if curried
                if(IsCurry) {
                    sb.AppendLine($"{resultHolder} = VarCurryGet({valueStackName}, VarNewString(\"{Identifier}\"));");
                }else{
                    sb.AppendLine($"Var* {resultHolder} = VarGet({valueStackName}, VarNewString(\"{Identifier}\"));");
                }
                return sb.ToString();
            }else{
                // Use VarCurryGet if curried
                if(IsCurry) {
                    stackName = $"/* Get index: {Identifier} */ VarCurryGet({valueStackName}, VarNewString(\"{Identifier}\"))";
                }else{
                    stackName = $"/* Get index: {Identifier} */ VarGet({valueStackName}, VarNewString(\"{Identifier}\"))";
                }
                return "";
            }
        }else if(Index != null){
            string indexBody = Index.GenerateInline(header, out string indexStackName);
            string valueBody = Value.GenerateInline(header, out string valueStackName);
            if(!String.IsNullOrEmpty(indexBody) && !String.IsNullOrEmpty(valueBody)) {
                stackName = $"/* Get index: {indexStackName} */ VarGet({valueStackName}, {indexStackName})";
                return $"{indexBody}\n{valueBody}";
            }
            string resultHolder = UniqueValueName("get");
            stackName = resultHolder;
            StringBuilder sb = new StringBuilder();
            sb.AppendLine($"// Get index: {Identifier}");
            sb.AppendLine(valueBody);
            sb.AppendLine(indexBody);
            sb.AppendLine($"Var* {resultHolder} = VarGet({valueStackName}, {indexStackName});");
            return sb.ToString();
        }else{
            throw new Exception("Invalid indexing");
        }
    }

    public override string GenerateSetterInline(StreamWriter header, out string stackName, string value) {
        if(Identifier != null) {
            string valueBody = Value.GenerateInline(header, out string valueStackName);
            if(!String.IsNullOrEmpty(valueBody)) {
                string resultHolder = UniqueValueName("set");
                stackName = resultHolder;
                StringBuilder sb = new StringBuilder();
                sb.AppendLine($"// Set index: {Identifier}");
                sb.AppendLine(valueBody);
                // Use VarCurrySet if curried
                if(IsCurry) {
                    sb.AppendLine($"Var* {resultHolder} = VarCurrySet({valueStackName}, VarNewString(\"{Identifier}\"), {value});");
                }else{
                    sb.AppendLine($"Var* {resultHolder} = VarRawSet({valueStackName}, VarNewString(\"{Identifier}\"), {value});");
                }
                return sb.ToString();
            }else{
                // Use VarCurrySet if curried
                if(IsCurry) {
                    stackName = $"/* Set index: {Identifier} */ VarCurrySet({valueStackName}, VarNewString(\"{Identifier}\"), {value})";
                }else{
                    stackName = $"/* Set index: {Identifier} */ VarRawSet({valueStackName}, VarNewString(\"{Identifier}\"), {value})";
                }
                return "";
            }
        }else if(Index != null){
            string indexBody = Index.GenerateInline(header, out string indexStackName);
            string valueBody = Value.GenerateInline(header, out string valueStackName);
            if(!String.IsNullOrEmpty(indexBody) && !String.IsNullOrEmpty(valueBody)) {
                stackName = $"/* Set index: {indexStackName} */ VarRawSet({valueStackName}, {indexStackName}, {value})";
                return $"{indexBody}\n{valueBody}";
            }else{
                string resultHolder = UniqueValueName("set");
                stackName = resultHolder;
                StringBuilder sb = new StringBuilder();
                sb.AppendLine($"// Set index: {Identifier}");
                sb.AppendLine(valueBody);
                sb.AppendLine(indexBody);
                sb.AppendLine($"Var* {resultHolder} = VarRawSet({valueStackName}, {indexStackName}, {value});");
                return sb.ToString();
            }
        }else{
            throw new Exception("Invalid indexing");
        }
    }
}