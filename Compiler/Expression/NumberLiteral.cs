public class NumberLiteral : Expression {
    public double Value { get; set; }

    public NumberLiteral(double value) {
        Value = value;
    }

    public static (NumberLiteral?, int) TryParse(List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.Number) {
            return (null, index);
        }
        var number = new NumberLiteral(double.Parse(tokens[i].Value));
        i++;
        return (number, i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName) {
        stackName = $"/* Number {Value} */ VarNewNumber({Value})";
        return "";
    }
}