public class Math : Expression {
    public Expression? Left { get; set; }
    public Expression Right { get; set; }
    public BinaryOperator? OperatorBinary { get; set; }
    public UnaryOperator? OperatorUnary { get; set; }

    public Math(Expression? left, Expression right, BinaryOperator? operatorBinary, UnaryOperator? operatorUnary) {
        Left = left;
        Right = right;
        OperatorBinary = operatorBinary;
        OperatorUnary = operatorUnary;
    }

    public static (Math?, int) TryParse(List<Token> tokens, int index) {
        int i = index;
        // Try to get a Unary math operation
        var (operatorUnary, j) = UnaryOperator.TryParse(tokens, i);
        if(operatorUnary == null) {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i = j;
        // And then an expression
        (var expression, j) = Expression.TryParseAny(tokens, i);
        if(expression == null) {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i = j;
        // Build and return
        return (new Math(null, expression, null, operatorUnary), i);
    }

    public static (Math?, int) TryParse(Expression left, List<Token> tokens, int index) {
        int i = index;
        // Try to get a Binary math operation
        var (operatorBinary, j) = BinaryOperator.TryParse(tokens, i);
        if(operatorBinary == null) {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i = j;
        // And then an expression
        (var expression, j) = Expression.TryParseAny(tokens, i);
        if(expression == null) {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i = j;
        // Build and return
        return (new Math(left, expression, operatorBinary, null), i);
    }

    public override string Generate(string stackName, StreamWriter header) {
        // Binary math
        if(OperatorBinary != null) {
            // Custom behaviour for && and ||
            if(OperatorBinary.Operator == "&&") {
                string leftHolder = UniqueValueName("left");
                string s = $"Var* {leftHolder} = &NIL;\n";
                s += Left!.Generate(leftHolder, header);
                s += $"if(!VarTruthy({leftHolder})){{";
                s += $"{stackName} = {leftHolder};\n";
                s += $"}}else{{\n";
                string rightHolder = UniqueValueName("right");
                s += $"Var* {rightHolder} = &NIL;\n";
                s += Right.Generate(rightHolder, header);
                s += $"{stackName} = {rightHolder};\n";
                s += $"}}\n";
                return s;
            }else if(OperatorBinary.Operator == "||") {
                string leftHolder = UniqueValueName("left");
                string s = $"Var* {leftHolder} = &NIL;\n";
                s += Left!.Generate(leftHolder, header);
                s += $"if(VarTruthy({leftHolder})){{\n";
                s += $"{stackName} = {leftHolder};\n";
                s += $"}}else{{\n";
                string rightHolder = UniqueValueName("right");
                s += $"Var* {rightHolder} = &NIL;\n";
                s += Right.Generate(rightHolder, header);
                s += $"{stackName} = {rightHolder};\n";
                s += $"}}\n";
                return s;
            }else{
                string leftHolder = UniqueValueName("left");
                string rightHolder = UniqueValueName("right");
                string s = $"Var* {leftHolder} = &NIL;\n";
                s += Left.Generate(leftHolder, header);
                s += $"Var* {rightHolder} = &NIL;\n";
                s += Right.Generate(rightHolder, header);
                s += $"{stackName} = {leftHolder};\n";
                string metamethodHolder = UniqueValueName("metamethod");
                s += $"Var* {metamethodHolder} = VarGetMeta({leftHolder}, \"{BinaryOperator.OperatorMetamethods[OperatorBinary.Operator]}\");\n";            
                string argsHolder = UniqueValueName("args");
                s += $"Var* {argsHolder} = &NIL;\n";
                s += $"{argsHolder} = VarNewList({rightHolder});\n";
                s += $"VarRawSet({argsHolder}, VarNewNumber(0), {leftHolder});\n";
                s += $"VarRawSet({argsHolder}, VarNewString(\"left\"), {leftHolder});\n";
                s += $"VarRawSet({argsHolder}, VarNewNumber(1), {rightHolder});\n";
                s += $"VarRawSet({argsHolder}, VarNewString(\"right\"), {rightHolder});\n";
                s += $"{stackName} = VarFunctionCall({metamethodHolder}, {argsHolder});\n";
                return s;
            }
        }else{
            throw new Exception($"Unary math not implemented {OperatorUnary!.Operator}");
        }
        return "";
    }
}
