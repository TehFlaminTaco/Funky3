using System.Text;
public class With : Expression {
    public Expression Expression { get; set; }
    public List<Expression> Body { get; set; }

    public With(Expression expression, List<Expression> body) {
        Expression = expression;
        Body = body;
    }

    public static (With?, int) TryParse(List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.Keyword || tokens[i].Value != "with") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;

        // Get an expression
        var (valueExpression, j) = Expression.TryParseAny(tokens, i);
        if(valueExpression == null) {
            return (null, index);
        }
        i = j;

        // Check for a block
        List<Expression> body;
        if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "{") {
            // Single expression
            (var expression, j) = Expression.TryParseAny(tokens, i);
            if(expression == null) {
                return (null, index);
            }
            i = j;
            body = new List<Expression> { expression };
            return (new With(valueExpression, body), i);
        }

        // Block
        Parser.RegisterFurthest(i);
        i++;
        body = new List<Expression>();
        while(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "}") {
            (var expression, j) = Expression.TryParseAny(tokens, i);
            if(expression == null) {
                return (null, index);
            }
            i = j;
            body.Add(expression);

            // Skip a semicolon
            if(tokens[i].Type == TokenType.Punctuation && tokens[i].Value == ";") {
                Parser.RegisterFurthest(i);
                i++;
            }
        }
        
        // Check for final brace
        if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "}") {
            return (null, index);
        }

        Parser.RegisterFurthest(i);
        i++;

        return (new With(valueExpression, body), i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName) {
        StringBuilder sb = new StringBuilder();
        sb.AppendLine("// With");
        string BlockReturnValue = UniqueValueName("blockReturnValue");
        sb.AppendLine($"Var* {BlockReturnValue} = &NIL;");
        sb.AppendLine("{");
        Block.PushCurrentScope();
        stackName = BlockReturnValue;
        string expressionBody = Expression.GenerateInline(header, out string expressionHolder);
        if(!String.IsNullOrEmpty(expressionBody)) {
            sb.Append(expressionBody);
        }
        sb.AppendLine($"\tscope = VarWithScope(scope, {expressionHolder});");
        foreach(var expression in Body) {
            string retVal;
            string lineBody = expression.GenerateInline(header, out retVal);
            if(!string.IsNullOrEmpty(lineBody)) {
                sb.AppendLine("\t"+lineBody);
            };
            sb.AppendLine($"\t{BlockReturnValue} = {retVal};");
        }
        Block.PopCurrentScope();
        sb.AppendLine("}");
        return sb.ToString();
    }
}