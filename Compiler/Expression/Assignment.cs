using System.Text;
public class Assignment : Expression {
    public Variable Name { get; set; }
    public Expression Value { get; set; }
    public BinaryOperator? Operator { get; set; }

    public Assignment(Variable name, Expression value, BinaryOperator? operator_ = null) {
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

        BinaryOperator? operator_ = null;
        if(tokens[i].Value != "=") { // Try to get an operator.
            var op = BinaryOperator.TryParse(tokens, i);
            if(op.Item1 is not BinaryOperator binaryOperator) {
                return (null, index);
            }
            operator_ = binaryOperator;
            i = op.Item2;
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
        string valueBody = Value.GenerateInline(header, out string valueHolder);
        StringBuilder sb = new();
        if(!String.IsNullOrEmpty(valueBody)){
            sb.Append(Tabbed(valueBody));
        }
        if(Operator is not null){
            // Math. BAH.
            string argHolder = UniqueValueName("arg");
            sb.AppendLine($"// {Operator.Operator}=");
            sb.AppendLine($"\tVar* {argHolder} = VarNewList();");
            string storedValueBody = Name.GenerateInline(header, out string storedValueHolder);
            if(!String.IsNullOrEmpty(storedValueBody)){
                sb.Append(Tabbed(storedValueBody));
            }
            if(true){
                string valueHolder2 = UniqueValueName("value");
                sb.AppendLine($"\tVar* {valueHolder2} = {storedValueHolder};");
                storedValueHolder = valueHolder2;
            }
            string metaMethod = $"VarGetMeta({storedValueHolder}, \"{BinaryOperator.OperatorMetamethods[Operator.Operator]}\")";
            sb.AppendLine($"\tArgVarSet({argHolder}, 0, \"left\", {storedValueHolder});");
            sb.AppendLine($"\tArgVarSet({argHolder}, 1, \"right\", {valueHolder});");
            valueHolder = $"VarFunctionCall({metaMethod}, {argHolder})";
        }
        valueBody = Name.GenerateSetterInline(header, out stackName, valueHolder); // KNOWN to always be inline
        if(!String.IsNullOrEmpty(valueBody)){
            sb.Append(Tabbed(valueBody));
        }
        return sb.ToString();
    }
}