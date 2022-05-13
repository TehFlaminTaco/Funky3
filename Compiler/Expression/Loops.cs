using System.Text;
public abstract class Loop : Expression{
    public static Stack<(string breakLabel, string breakHolder)> BreakValues = new();
}

public class While : Loop {
    public Expression Condition {get; set;}
    public Expression Body {get; set;}

    public While(Expression condition, Expression body) {
        Condition = condition;
        Body = body;
    }

    public static (While?, int) TryParse(List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.Keyword || tokens[i].Value != "while") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;

        var condition = Expression.TryParseAny(tokens, i);
        if(condition.expression == null) {
            return (null, index);
        }
        i = condition.index;

        var body = Expression.TryParseAny(tokens, i);
        if(body.expression == null) {
            return (null, index);
        }
        i = body.index;

        return (new While(condition.expression, body.expression), i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName){
        StringBuilder sb = new();
        string labelEnd = UniqueValueName("whileEnd");
        string labelStart = UniqueValueName("whileStart");
        string result = UniqueValueName("whileResult");
        BreakValues.Push((labelEnd, result));
        sb.AppendLine($"// While");
        sb.AppendLine($"\tVar* {result} = &NIL;");
        sb.AppendLine($"\t{labelStart}:");
        // First condition iteration
        string conditionBody = Condition.GenerateInline(header, out string conditionHolder);
        if(!String.IsNullOrEmpty(conditionBody)) {
            sb.Append(Tabbed(conditionBody));
        }
        sb.AppendLine($"\tif(!VarTruthy({conditionHolder})) {{");
        sb.AppendLine($"\t\tgoto {labelEnd};");
        sb.AppendLine($"\t}}");
        // Body
        string bodyBody = Body.GenerateInline(header, out string bodyHolder);
        if(!String.IsNullOrEmpty(bodyBody)) {
            sb.Append(Tabbed(bodyBody));
        }
        sb.AppendLine($"\t{result} = {bodyHolder};");
        sb.AppendLine($"\tgoto {labelStart};");
        sb.AppendLine($"\t{labelEnd}:");
        BreakValues.Pop();
        stackName = result;
        return sb.ToString();
    }
}