using System.Text;

public class If : Expression, ILeftProvider, IRightProvider
{
    public Expression Condition { get; set; }
    public Expression Then { get; set; }
    public Expression? Else { get; set; }
    public bool IsTernary { get; set; }

    public If(bool isTernary, Expression condition, Expression then, Expression? @else = null)
    {
        Condition = condition;
        Then = then;
        Else = @else;
        IsTernary = isTernary;
    }

    public static (If?, int) TryParse(List<Token> tokens, int index)
    {
        int i = index;
        if (tokens[i].Type != TokenType.Keyword || tokens[i].Value != "if")
        {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;
        var condition = Expression.TryParseAny(tokens, i);
        if (condition.expression == null)
        {
            return (null, index);
        }
        i = condition.index;

        var then = Expression.TryParseAny(tokens, i);
        if (then.expression == null)
        {
            return (null, index);
        }
        i = then.index;

        if (tokens[i].Type != TokenType.Keyword || tokens[i].Value != "else")
        {
            return (new If(false, condition.expression, then.expression), i);
        }
        Parser.RegisterFurthest(i);
        i++;

        var @else = Expression.TryParseAny(tokens, i);
        if (@else.expression == null)
        {
            return (null, index);
        }
        i = @else.index;

        return (new If(false, condition.expression, then.expression, @else.expression), i);
    }

    // Ternary statement. a ? b : c
    // Or a ? b
    public static (If?, int) TryParse(Expression left, List<Token> tokens, int index)
    {
        int i = index;
        if (tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "?")
        {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;

        Expression.PushBlock(new()
        {
            typeof(IndexVariable)
        });
        var then = Expression.TryParseAny(tokens, i);
        Expression.PopBlock();
        if (then.expression == null)
        {
            return (null, index);
        }
        i = then.index;

        if (tokens[i].Type != TokenType.Punctuation || tokens[i].Value != ":")
        {
            return (new If(true, left, then.expression), i);
        }
        Parser.RegisterFurthest(i);
        i++;

        var @else = Expression.TryParseAny(tokens, i);
        if (@else.expression == null)
        {
            return (null, index);
        }
        i = @else.index;

        return (new If(true, left, then.expression, @else.expression), i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName)
    {
        string ifResultHolder = UniqueValueName("ifResult");
        stackName = ifResultHolder;
        StringBuilder sb = new();
        sb.AppendLine($"// If");
        sb.AppendLine($"\tVar* {ifResultHolder} = &NIL;");
        string conditionBody = Condition.GenerateInline(header, out string conditionHolder);
        if (!String.IsNullOrEmpty(conditionBody))
        {
            sb.Append(conditionBody);
        }
        sb.AppendLine($"\tif(VarTruthy({conditionHolder})) {{");
        Block.PushCurrentScope();
        string thenBody = Then.GenerateInline(header, out string thenHolder);
        if (!String.IsNullOrEmpty(thenBody))
        {
            sb.Append(Tabbed(Tabbed(thenBody)));
        }
        sb.AppendLine($"\t{ifResultHolder} = {thenHolder};");
        if (Else != null)
        {
            Block.PopCurrentScope();
            sb.AppendLine($"\t}} else {{");
            Block.PushCurrentScope();
            string elseBody = Else!.GenerateInline(header, out string elseHolder);
            if (!String.IsNullOrEmpty(elseBody))
            {
                sb.Append(Tabbed(Tabbed(elseBody)));
            }
            sb.AppendLine($"\t{ifResultHolder} = {elseHolder};");
        }
        Block.PopCurrentScope();
        sb.AppendLine($"\t}}");
        return sb.ToString();
    }

    public Expression? GetLeft()
    {
        return IsTernary ? Condition : null;
    }

    public void SetLeft(Expression? e)
    {
        if (IsTernary) Condition = e!;
    }

    public int GetPrecedence()
    {
        return BinaryOperator.MATHMINPRECEDENCE + 8;
    }

    public Expression? GetRight()
    {
        return IsTernary ? (Else ?? Then) : null;
    }

    public void SetRight(Expression? e)
    {
        if (IsTernary)
        {
            if (Else == null)
            {
                Then = e!;
            }
            else
            {
                Else = e!;
            }
        }
    }
}
