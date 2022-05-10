using System.Text;
public class Assignment : Expression {
    public Variable Name { get; set; }
    public Expression Value { get; set; }
    public Expression? Operator { get; set; }

    public Assignment(Variable name, Expression value, Expression? operator_ = null) {
        Name = name;
        Value = value;
        Operator = operator_;
    }

    public static (Assignment?, int) TryParse(Expression left, List<Token> tokens, int index) {
        int i = index;
        if(left is not Variable variable) {
            return (null, index);
        }
        if(tokens[i].Type != TokenType.Punctuation){
            return (null, index);
        }

        Expression? operator_ = null;
        if(tokens[i].Value != "=") { // Try to get an operator.
            (Expression? expression, int index) result = Expression.TryParseAny(tokens, i);
            if(result.expression != null) {
                i = result.index;
                operator_ = result.expression;
            }else{
                return (null, index);
            }
        }
        Parser.RegisterFurthest(i);
        if(tokens[i].Value != "=") { // Check for an assignment operator.
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;
        {
            (Expression? expression, int index) result = Expression.TryParseAny(tokens, i);
            if(result.expression == null) {
                return (null, index);
            }
            i = result.index;
            var assignment = new Assignment(variable, result.expression, operator_);
            return (assignment, i);
        }
    }

    public override string GenerateInline(StreamWriter header, out string stackName){
        string valueHolder;
        string valueBody = Value.GenerateInline(header, out valueHolder);
        Name.GenerateSetterInline(header, out stackName, valueHolder); // KNOWN to always be inline
        return valueBody;
    }
}