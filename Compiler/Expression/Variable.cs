using System.ComponentModel;
using System.Text;

public class Variable : Expression
{
    public string? Name { get; set; }
    public bool IsLocal { get; set; }

    public Variable(string name, bool isLocal)
    {
        Name = name;
        IsLocal = isLocal;
    }

    public Variable()
    {
    }

    public static (Variable?, int) TryParse(List<Token> tokens, int index)
    {
        int i = index;
        bool isLocal = false;
        if (tokens[index].Type == TokenType.Punctuation && tokens[index].Value == "(")
        {
            return UnpackVariable.TryParse(tokens, index);
        }
        if (tokens[index].Type == TokenType.Keyword)
        {
            if (tokens[index].Value == "var" || tokens[index].Value == "local")
            {
                Parser.RegisterFurthest(i);
                i++;
                isLocal = true;
            }
            else
            {
                return (null, index);
            }
        }
        if (tokens[i].Type != TokenType.Identifier)
        {
            return (null, index);
        }
        var variable = new Variable(tokens[i].Value, isLocal);
        i++;
        return (variable, i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName)
    {
        if (IsLocal)
        {
            StringBuilder sb = new StringBuilder();
            string varHolder = UniqueValueName($"var_{Name}");
            sb.AppendLine($"// Get local:{Name}");
            sb.AppendLine($"Var* {varHolder} = VarRawGet(scope, VarNewConstString(\"{Name}\"));");
            sb.AppendLine($"if(ISUNDEFINED({varHolder})) {{");
            sb.AppendLine($"\t{varHolder} = VarRawSet(scope, VarNewConstString(\"{Name}\"), &NIL);");
            sb.AppendLine($"}}");
            Block.SetFromScope(Name!, varHolder);
            stackName = varHolder;
            return sb.ToString();
        }
        else
        {
            // Can always be inlined.
            if (Block.GetFromScope(Name!, out var s))
                stackName = $"/* Get {Name} */ {s}";
            else
                stackName = $"/* Get {Name} */ VarGet(scope, VarNewConstString(\"{Name}\"))";
            return "";
        }
    }

    public virtual string GenerateSetterInline(StreamWriter header, out string stackName, string value)
    {
        // Can always be inlined.
        // The setter never needs to make the default check, because it always sets a value.
        if (IsLocal)
        {
            if (Block.GetFromScope(Name!, out var s))
            {
                stackName = $"/* Set local:{Name} */ {s} = VarRawSet(scope, VarNewConstString(\"{Name}\"), {value})";
                return "";
            }
            StringBuilder sb = new StringBuilder();
            string varHolder = UniqueValueName($"var_{Name}");
            sb.AppendLine($"// Set local:{Name}");
            sb.AppendLine($"Var* {varHolder} = VarRawSet(scope, VarNewConstString(\"{Name}\"), {value});");
            stackName = varHolder;
            Block.SetFromScope(Name!, varHolder);
            return sb.ToString();
        }
        else
        {
            if (Block.GetFromScope(Name!, out var s))
            {
                stackName = $"/* Set {Name} */ {s} = VarSet(scope, VarNewConstString(\"{Name}\"), {value})";
            }
            else
            {
                stackName = $"/* Set {Name} */ VarSet(scope, VarNewConstString(\"{Name}\"), {value})";
            }
        }
        return "";
    }
}

public class IndexVariable : Variable, ILeftProvider
{
    public Expression? Index { get; set; }
    public string? Identifier { get; set; }
    public Expression Value { get; set; }
    public bool IsCurry { get; set; }

    public IndexVariable(Expression index, Expression value) : base()
    {
        Index = index;
        Value = value;
        IsCurry = false;
    }

    public IndexVariable(string identifier, Expression value, bool isCurry = false) : base()
    {
        Identifier = identifier;
        Value = value;
        IsCurry = isCurry;
    }


    // Indexing, one of `[index]`, `.name`, or `:name`
    public static (Variable?, int) TryParse(Expression left, List<Token> tokens, int index)
    {
        int i = index;
        if (tokens[i].Type != TokenType.Punctuation)
        {
            return (null, index);
        }
        if (tokens[i].Value == "[")
        {
            Parser.RegisterFurthest(i);
            i++;
            {
                (Expression? expression, int index) result = Expression.TryParseAny(tokens, i);
                if (result.expression == null)
                {
                    return (null, index);
                }
                i = result.index;
                // Skip a right bracket
                if (tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "]")
                {
                    return (null, index);
                }
                Parser.RegisterFurthest(i);
                i++;
                var indexVariable = new IndexVariable(result.expression, left);
                return (indexVariable, i);
            }
        }
        if (tokens[i].Value == ".")
        {
            Parser.RegisterFurthest(i);
            i++;
            if (tokens[i].Type != TokenType.Identifier)
            {
                return (null, index);
            }
            var indexVariable = new IndexVariable(tokens[i].Value, left);
            Parser.RegisterFurthest(i);
            i++;
            return (indexVariable, i);
        }
        if (tokens[i].Value == ":")
        {
            Parser.RegisterFurthest(i);
            i++;
            if (tokens[i].Type != TokenType.Identifier)
            {
                return (null, index);
            }
            var indexVariable = new IndexVariable(tokens[i].Value, left, true);
            Parser.RegisterFurthest(i);
            i++;
            return (indexVariable, i);
        }
        return (null, index);
    }

    public override string GenerateInline(StreamWriter header, out string stackName)
    {
        // VarGet(Value, Index)
        if (Identifier != null)
        {
            string valueBody = Value.GenerateInline(header, out string valueStackName);
            if (!String.IsNullOrEmpty(valueBody))
            {
                string resultHolder = UniqueValueName("get");
                stackName = resultHolder;
                StringBuilder sb = new StringBuilder();
                sb.AppendLine($"// Get index: {Identifier}");
                sb.Append(valueBody);
                // Use VarCurryGet if curried
                if (IsCurry)
                {
                    sb.AppendLine($"Var* {resultHolder} = VarCurryGet({valueStackName}, VarNewConstString(\"{Identifier}\"));");
                }
                else
                {
                    sb.AppendLine($"Var* {resultHolder} = VarGet({valueStackName}, VarNewConstString(\"{Identifier}\"));");
                }
                return sb.ToString();
            }
            else
            {
                // Use VarCurryGet if curried
                if (IsCurry)
                {
                    stackName = $"/* Get index: {Identifier} */ VarCurryGet({valueStackName}, VarNewConstString(\"{Identifier}\"))";
                }
                else
                {
                    stackName = $"/* Get index: {Identifier} */ VarGet({valueStackName}, VarNewConstString(\"{Identifier}\"))";
                }
                return "";
            }
        }
        else if (Index != null)
        {
            string indexBody = Index.GenerateInline(header, out string indexStackName);
            string valueBody = Value.GenerateInline(header, out string valueStackName);
            if (!String.IsNullOrEmpty(indexBody) && !String.IsNullOrEmpty(valueBody))
            {
                stackName = $"/* Get index: {indexStackName} */ VarGet({valueStackName}, {indexStackName})";
                return $"{indexBody}\n{valueBody}";
            }
            string resultHolder = UniqueValueName("get");
            stackName = resultHolder;
            StringBuilder sb = new StringBuilder();
            sb.AppendLine($"// Get index: {Identifier}");
            sb.Append(valueBody);
            sb.Append(indexBody);
            sb.AppendLine($"Var* {resultHolder} = VarGet({valueStackName}, {indexStackName});");
            return sb.ToString();
        }
        else
        {
            throw new Exception("Invalid indexing");
        }
    }

    public override string GenerateSetterInline(StreamWriter header, out string stackName, string value)
    {
        if (Identifier != null)
        {
            string valueBody = Value.GenerateInline(header, out string valueStackName);
            if (!String.IsNullOrEmpty(valueBody))
            {
                string resultHolder = UniqueValueName("set");
                stackName = resultHolder;
                StringBuilder sb = new StringBuilder();
                sb.AppendLine($"// Set index: {Identifier}");
                sb.Append(valueBody);
                // Use VarCurrySet if curried
                if (IsCurry)
                {
                    sb.AppendLine($"Var* {resultHolder} = VarCurrySet({valueStackName}, VarNewConstString(\"{Identifier}\"), {value});");
                }
                else
                {
                    sb.AppendLine($"Var* {resultHolder} = VarSet({valueStackName}, VarNewConstString(\"{Identifier}\"), {value});");
                }
                return sb.ToString();
            }
            else
            {
                // Use VarCurrySet if curried
                if (IsCurry)
                {
                    stackName = $"/* Set index: {Identifier} */ VarCurrySet({valueStackName}, VarNewConstString(\"{Identifier}\"), {value})";
                }
                else
                {
                    stackName = $"/* Set index: {Identifier} */ VarSet({valueStackName}, VarNewConstString(\"{Identifier}\"), {value})";
                }
                return "";
            }
        }
        else if (Index != null)
        {
            string indexBody = Index.GenerateInline(header, out string indexStackName);
            string valueBody = Value.GenerateInline(header, out string valueStackName);
            if (!String.IsNullOrEmpty(indexBody) && !String.IsNullOrEmpty(valueBody))
            {
                stackName = $"/* Set index: {indexStackName} */ VarSet({valueStackName}, {indexStackName}, {value})";
                StringBuilder sb = new();
                if (!String.IsNullOrEmpty(indexBody))
                {
                    sb.Append(indexBody);
                }
                if (!String.IsNullOrEmpty(valueBody))
                {
                    sb.Append(valueBody);
                }
                return sb.ToString();
            }
            else
            {
                string resultHolder = UniqueValueName("set");
                stackName = resultHolder;
                StringBuilder sb = new();
                sb.AppendLine($"// Set index: {Identifier}");
                if (String.IsNullOrEmpty(valueBody))
                {
                    sb.Append(valueBody);
                }
                if (String.IsNullOrEmpty(indexBody))
                {
                    sb.Append(indexBody);
                }
                sb.AppendLine($"Var* {resultHolder} = VarSet({valueStackName}, {indexStackName}, {value});");
                return sb.ToString();
            }
        }
        else
        {
            throw new Exception("Invalid indexing");
        }
    }

    public Expression? GetLeft()
    {
        return Value;
    }

    public void SetLeft(Expression? e)
    {
        Value = e!;
    }

    public int GetPrecedence()
    {
        return IExpressionProvider.CALL_PRECEDENCE;
    }
}

// (a, b...) = list
// A getter for this is illogical.
public class UnpackVariable : Variable
{
    List<(Variable var, bool isSplat)> Variables;
    public UnpackVariable(List<(Variable var, bool isSplat)> variables)
    {
        Variables = variables;
    }

    public static new(Variable?, int) TryParse(List<Token> tokens, int index)
    {
        int i = index;
        if (tokens[index].Type != TokenType.Punctuation || tokens[index].Value != "(")
        {
            return (null, index);
        }
        Parser.RegisterFurthest(index);
        i++;
        List<(Variable var, bool isSplat)> variables = new();

        while (tokens[i].Type != TokenType.Punctuation || tokens[i].Value != ")")
        {
            (Variable var, bool isSplat) variable;
            (var e, i) = Expression.TryParseAny(tokens, i);
            if (e is not Variable v)
            {
                return (null, index);
            }
            if (tokens[i].Type == TokenType.Punctuation && tokens[i].Value == "." && tokens[i + 1].Type == TokenType.Punctuation && tokens[i + 1].Value == "." && tokens[i + 2].Type == TokenType.Punctuation && tokens[i + 2].Value == ".")
            {
                variable = (v, true);
                i += 3;
            }
            else
            {
                variable = (v, false);
            }
            variables.Add(variable);
            if (tokens[i].Type == TokenType.Punctuation && tokens[i].Value == ",")
            {
                Parser.RegisterFurthest(i);
                i++;
            }
        }
        Parser.RegisterFurthest(i);
        i++;
        return (new UnpackVariable(variables), i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName)
    {
        // In case you're trying to do x = (y) and it thinks it's an unpack for some reason.
        if (Variables.Count == 1)
        {
            return Variables[0].var.GenerateInline(header, out stackName);
        }
        throw new Exception("No getter for an unpack statement.");
    }

    public override string GenerateSetterInline(StreamWriter header, out string stackName, string value)
    {
        StringBuilder sb = new();
        sb.AppendLine($"// Unpack");
        string valueHolder = UniqueValueName("value");
        sb.AppendLine($"\tVar* {valueHolder} = {value};");
        stackName = valueHolder;
        // TODO: Maybe some unpack operator? Maybe not.
        sb.AppendLine($"\tif({valueHolder} -> type == VAR_LIST) {{");
        Block.PushCurrentScope();
        // Basically the same thing as a function defintion. Ish.
        string argIndex = UniqueValueName("argIndex");
        sb.AppendLine($"\t\tint {argIndex} = 0;");
        for (int i = 0; i < Variables.Count; i++)
        {
            string argName = UniqueValueName($"arg{i}");
            sb.AppendLine($"\t\t// Variable: {i} ");
            // If it's splatted, Dump the rest of the arguments into this in particular.
            if (Variables[i].isSplat)
            {
                sb.AppendLine($"\t\t\tVar* {argName} = VarListCopyLShifted({valueHolder}, {argIndex});");
                // Remove all arguments by name from the list.
                for (int j = 0; j < Variables.Count; j++)
                {
                    if (Variables[j].var.Name is not null)
                    {
                        sb.AppendLine($"\t\t\tVarRawSet({argName}, VarNewConstString(\"{Variables[j].var.Name}\"), &UNDEFINED);");
                    }
                }
            }
            else
            {
                if (Variables[i].var.Name is not null)
                {
                    sb.AppendLine($"\t\t\tVar* {argName} = VarRawGet({valueHolder}, VarNewConstString(\"{Variables[i].var.Name}\"));");
                    sb.AppendLine($"\t\t\tif(ISUNDEFINED({argName})) {{");
                    sb.AppendLine($"\t\t\t\t{argName} = VarRawGet({valueHolder}, VarNewNumber({argIndex}++));");
                    sb.AppendLine($"\t\t\t}}");
                }
                else
                {
                    sb.AppendLine($"\t\t\tVar* {argName} = VarRawGet({valueHolder}, VarNewNumber({argIndex}++));");
                }
                sb.AppendLine($"\t\t\tif(ISUNDEFINED({argName})) {{");
                sb.AppendLine($"\t\t\t\t{argName} = &NIL;");
                sb.AppendLine("\t\t\t}");
            }
            string varBody = Variables[i].var.GenerateSetterInline(header, out string varStackName, argName);
            if (!String.IsNullOrEmpty(varBody))
            {
                sb.Append(Tabbed(Tabbed(Tabbed(varBody))));
            }
            sb.AppendLine($"\t\t\t_ = {varStackName};");
        }
        Block.PopCurrentScope();
        sb.AppendLine("\t}");
        return sb.ToString();
    }
}