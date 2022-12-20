using System.Text;
public class Block : Expression
{
    public List<Expression> Expressions { get; set; }

    public Block(List<Expression> expressions)
    {
        Expressions = expressions;
    }

    public static List<Dictionary<string, string>?> CurrentVariables = new(new[]{
        new Dictionary<string, string>()
    });

    public static void PushCurrentScope()
    {
        CurrentVariables.Insert(0, new());
    }
    public static void PushBadScope()
    {
        CurrentVariables.Insert(0, null);
    }
    public static void PopCurrentScope()
    {
        CurrentVariables.RemoveAt(0);
    }
    public static void ResetScopes()
    {
        CurrentVariables.Clear();
        PushCurrentScope();
    }

    public static bool GetFromScope(string name, out string cName)
    {
        for (int i = 0; i < CurrentVariables.Count(); i++)
        {
            var scope = CurrentVariables[i];
            if (scope is null) break;
            if (scope.ContainsKey(name))
            {
                cName = scope[name];
                return true;
            }
        }
        cName = "";
        return false;
    }

    public static void SetFromScope(string name, string cName)
    {
        CurrentVariables[0]![name] = cName;
    }

    public static (Block?, int) TryParse(List<Token> tokens, int index)
    {
        int i = index;
        if (tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "{")
        {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;
        var expressions = new List<Expression>();
        while (tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "}")
        {
            var expression = Expression.TryParseAny(tokens, i);
            if (expression.expression == null)
            {
                return (null, index);
            }
            expressions.Add(expression.expression);
            i = expression.index;
            // Skip over a semicolon.
            if (tokens[i].Type == TokenType.Punctuation && tokens[i].Value == ";")
            {
                Parser.RegisterFurthest(i);
                i++;
            }
        }
        // Check for final brace
        if (tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "}")
        {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;
        return (new Block(expressions), i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName)
    {
        // Never inline. :(
        string BlockReturnValue = UniqueValueName("blockReturnValue");
        stackName = BlockReturnValue;

        var sb = new StringBuilder();
        sb.AppendLine("// Block");
        sb.AppendLine($"Var* {BlockReturnValue} = &NIL;");
        sb.AppendLine("{");
        PushCurrentScope();
        sb.AppendLine("\tVar* oldScope = scope;");
        sb.AppendLine("\tVar* scope = VarSubScope(oldScope);");
        foreach (var expression in Expressions)
        {
            string retVal;
            string lineBody = expression.GenerateInline(header, out retVal);
            if (!string.IsNullOrEmpty(lineBody))
            {
                sb.AppendLine("\t" + lineBody);
            };
            sb.AppendLine($"\t{BlockReturnValue} = {retVal};");
        }
        PopCurrentScope();
        sb.AppendLine("}");
        return sb.ToString();
    }
}