using System.Text;
public class Return : Expression {
    public Expression? Value { get; set; }
    bool IsBreak { get; set; }
    public Return(Expression? value = null, bool isBreak = false) {
        Value = value;
        IsBreak = isBreak;
    }

    public static (Return?, int) TryParse(List<Token> tokens, int index) {
        int i = index;

        if(tokens[i].Type != TokenType.Keyword || (tokens[i].Value != "return" && tokens[i].Value != "break")) {
            return (null, index);
        }
        bool isBreak = tokens[i].Value == "break";
        Parser.RegisterFurthest(i);
        i++;
        
        var (expression, j) = Expression.TryParseAny(tokens, i);

        if(expression == null) {
            return (new Return(null, isBreak), i);
        }
        return (new Return(expression, isBreak), j);
    }

    public override string GenerateInline(StreamWriter header, out string stackName) {
        stackName = "_";
        StringBuilder sb = new();
        if(IsBreak){
            sb.AppendLine("// Break");
            sb.AppendLine("break;");
            return sb.ToString();
        }else{
            sb.AppendLine($"// Return");
            if(Value != null) {
                string valueBody = Value!.GenerateInline(header, out string valueHolder);
                if(!String.IsNullOrEmpty(valueBody)) {
                    sb.Append(valueBody);
                }
                sb.AppendLine($"return {valueHolder};");
                return sb.ToString(); 
            }else{
                sb.AppendLine("return;");
                return sb.ToString(); 
            }
        }
    }
}