using System.Text;
public class Assignment : Expression, IRightProvider {
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

        BinaryOperator? operator_ = null;
        if(tokens[i].Value != "=" || tokens[i].Type != TokenType.Punctuation) { // Try to get an operator.
            var op = BinaryOperator.TryParse(tokens, i);
            if(op.Item1 is not BinaryOperator binaryOperator) {
                return (null, index);
            }
            operator_ = binaryOperator;
            i = op.Item2;
        }
        if(tokens[i].Type != TokenType.Punctuation){
            return (null, index);
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
        StringBuilder sb = new();
        if(Operator is not null){
            // If it's AND or OR, use short-circuiting.
            if(Operator.Operator == "&&" || Operator.Operator == "||"){
                // Calculate the currently stored value.
                string storedValueBody = Name.GenerateInline(header, out string storedValueHolder);
                if(!String.IsNullOrEmpty(storedValueBody)){
                    sb.Append(Tabbed(storedValueBody));
                }
                if(true){ // Ensure single use promise.
                    string valueHolder2 = UniqueValueName("value");
                    sb.AppendLine($"\tVar* {valueHolder2} = {storedValueHolder};");
                    storedValueHolder = valueHolder2;
                }
                // If it's an AND, the value should be set to whatever's on the right ONLY if this is truthy
                // If it's an OR, the value should be set to whatever's on the right ONLY if this is falsy
                sb.AppendLine($"\tif ({(Operator.Operator == "||" ? "!" : "")}VarTruthy({storedValueHolder})) {{");
                // Calculate the right side.
                string valueBody = Value.GenerateInline(header, out string valueHolder);
                if(!String.IsNullOrEmpty(valueBody)){
                    sb.Append(Tabbed(Tabbed(valueBody)));
                }
                // Run the setter
                string setterBody = Name.GenerateSetterInline(header, out string setterHolder, valueHolder);
                if(!String.IsNullOrEmpty(setterBody)){
                    sb.Append(Tabbed(setterBody));
                }
                // Ensure the setter is called here
                sb.AppendLine($"\t\t{storedValueHolder} = {setterHolder};");
                stackName = storedValueHolder;
                sb.AppendLine($"\t}}");
                return sb.ToString();
            
            // Otherwise, Handle as math.
            }else{
                // Calculate the currently stored value.
                string storedValueBody = Name.GenerateInline(header, out string storedValueHolder);
                if(!String.IsNullOrEmpty(storedValueBody)){
                    sb.Append(Tabbed(storedValueBody));
                }
                if(true){ // Ensure single use promise.
                    string valueHolder2 = UniqueValueName("value");
                    sb.AppendLine($"\tVar* {valueHolder2} = {storedValueHolder};");
                    storedValueHolder = valueHolder2;
                }
                // Calculate the right side.
                string valueBody = Value.GenerateInline(header, out string valueHolder);
                if(!String.IsNullOrEmpty(valueBody)){
                    sb.Append(Tabbed(Tabbed(valueBody)));
                }
                string metaMethod = $"VarGetMeta({storedValueHolder}, \"{BinaryOperator.OperatorMetamethods[Operator.Operator]}\")";
                string argHolder = UniqueValueName("arg");
                sb.AppendLine($"\tVar* {argHolder} = VarNewList();");
                sb.AppendLine($"\tArgVarSet({argHolder}, 0, \"left\", {storedValueHolder});");
                sb.AppendLine($"\tArgVarSet({argHolder}, 1, \"right\", {valueHolder});");
                // Call the setter with the outcome of the math
                string setterBody = Name.GenerateSetterInline(header, out string setterHolder, $"VarFunctionCall({metaMethod}, {argHolder})");
                if(!String.IsNullOrEmpty(setterBody)){
                    sb.Append(Tabbed(setterBody));
                }
                stackName = setterHolder;
                return sb.ToString();
            }
        // Otherwise, ignore what's stored and just set it to the right side.
        }else{
            string valueBody = Value.GenerateInline(header, out string valueHolder);
            if(!String.IsNullOrEmpty(valueBody)){
                sb.Append(Tabbed(valueBody));
            }
            string setterBody = Name.GenerateSetterInline(header, out string setterHolder, valueHolder);
            if(!String.IsNullOrEmpty(setterBody)){
                sb.Append(Tabbed(setterBody));
            }
            stackName = setterHolder;
            return sb.ToString();
        }
    }

    public Expression? GetRight(){
        return Value;
    }

    public void SetRight(Expression? e){
        Value = e!;
    }

    public int GetPrecedence(){
        return IExpressionProvider.NO_PRECEDENCE;
    }
}