public class StringLiteral : Expression {
    public string Value { get; set; }
    public StringLiteral(string value) {
        Value = value;
    }

    public static (StringLiteral?, int) TryParse(List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.String) {
            return (null, index);
        }
        var literal = new StringLiteral(tokens[i].Value);
        i++;
        return (literal, i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName) {
        stackName = $"/* String: {Value} */ VarNewString({Value})";
        return "";
    }
}