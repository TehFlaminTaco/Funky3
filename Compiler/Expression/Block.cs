using System.Text;
public class Block : Expression {
    public List<Expression> Expressions { get; set; }

    public Block(List<Expression> expressions) {
        Expressions = expressions;
    }

    public static (Block?, int) TryParse(List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "{") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;
        var expressions = new List<Expression>();
        while(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "}") {
            var expression = Expression.TryParseAny(tokens, i);
            if(expression.expression == null) {
                return (null, index);
            }
            expressions.Add(expression.expression);
            i = expression.index;
            // Skip over a semicolon.
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
        return (new Block(expressions), i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName) {
        // Never inline. :(
        string BlockReturnValue = UniqueValueName("blockReturnValue");
        stackName = BlockReturnValue;

        var sb = new StringBuilder();
        sb.AppendLine("// Block");
        sb.AppendLine($"Var* {BlockReturnValue} = &NIL;");
        sb.AppendLine("{");
        sb.AppendLine("\tVar* oldScope = scope;");
        sb.AppendLine("\tVar* scope = VarSubScope(oldScope);");
        foreach(var expression in Expressions) {
            string retVal;
            string lineBody = expression.GenerateInline(header, out retVal);
            if(!string.IsNullOrEmpty(lineBody)) {
                sb.AppendLine("\t"+lineBody);
            };
            sb.AppendLine($"\t{BlockReturnValue} = {retVal};");
        }
        sb.AppendLine("}");
        return sb.ToString();
    }
}