using System.Text;

public class Using : Expression
{
    public Expression Expression { get; set; }
    public Expression Body { get; set; }

    public Using(Expression expression, Expression body)
    {
        Expression = expression;
        Body = body;
    }

    public static (Using?, int) TryParse(List<Token> tokens, int index)
    {
        int i = index;
        if (tokens[i].Type != TokenType.Keyword || tokens[i].Value != "using")
        {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;

        // Get an expression
        var (valueExpression, j) = Expression.TryParseAny(tokens, i);
        if (valueExpression == null)
        {
            return (null, index);
        }
        i = j;

        // Single expression
        (var expression, j) = Expression.TryParseAny(tokens, i);
        if (expression == null)
        {
            return (null, index);
        }
        i = j;
        return (new Using(valueExpression, expression!), i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName)
    {
        StringBuilder sb = new StringBuilder();
        sb.AppendLine("// Using");
        var expressionBody = Expression.GenerateInline(header, out string expressionValue);
        if (!String.IsNullOrEmpty(expressionBody))
            sb.Append(Tabbed(expressionBody));
        var usingValue = UniqueValueName("usingValue");
        sb.AppendLine($"\tVar* {usingValue} = {expressionValue};");
        var bodyBody = Body.GenerateInline(header, out string bodyValue);
        if (!String.IsNullOrEmpty(bodyBody))
            sb.Append(Tabbed(bodyBody));
        var usingResult = UniqueValueName("usingResult");
        sb.AppendLine($"\tVar* {usingResult} = {bodyValue};"); // Ensure it evaluates BEFORE we dispose...
        var args = UniqueValueName("args");
        sb.AppendLine($"\tVar* {args} = VarNewList();");
        sb.AppendLine($"\tVarRawSet({args}, VarNewNumber(0), {usingValue});");
        sb.AppendLine($"\tVarFunctionCall(VarGetMeta({usingValue}, \"dispose\"), {args});");
        stackName = usingResult;
        return sb.ToString();
    }
}