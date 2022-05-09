using System.Text;
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
                StringBuilder sb = new();
                sb.AppendLine($"// &&");
                sb.AppendLine($"\tVar* {leftHolder} = &NIL);");
                sb.Append("\t"+Left!.GenerateTabbed(leftHolder, header));
                sb.AppendLine($"\tif(!VarTruthy({leftHolder})){{");
                sb.AppendLine($"\t\t{stackName} = {leftHolder};");
                sb.AppendLine($"\t}}else{{");
                string rightHolder = UniqueValueName("right");
                sb.AppendLine($"\tVar* {rightHolder} = &NIL;");
                sb.Append("\t"+Right.GenerateTabbed(rightHolder, header));
                sb.AppendLine($"\t\t{stackName} = {rightHolder};");
                sb.AppendLine($"\t}}");
                return sb.ToString();
            }else if(OperatorBinary.Operator == "||") {
                string leftHolder = UniqueValueName("left");
                StringBuilder sb = new();
                sb.AppendLine($"// ||");
                sb.AppendLine($"\tVar* {leftHolder} = &NIL);");
                sb.Append("\t"+Left!.GenerateTabbed(leftHolder, header));
                sb.AppendLine($"\tif(VarTruthy({leftHolder})){{");
                sb.AppendLine($"\t\t{stackName} = {leftHolder};");
                sb.AppendLine($"\t}}else{{");
                string rightHolder = UniqueValueName("right");
                sb.AppendLine($"\tVar* {rightHolder} = &NIL;");
                sb.Append("\t"+Right.GenerateTabbed(rightHolder, header));
                sb.AppendLine($"\t\t{stackName} = {rightHolder};");
                sb.AppendLine($"\t}}");
                return sb.ToString();
            }else{
                string leftHolder = UniqueValueName("left");
                string rightHolder = UniqueValueName("right");
                StringBuilder sb = new();
                sb.AppendLine($"// {OperatorBinary.Operator}");
                sb.AppendLine($"\tVar* {leftHolder} = &NIL);");
                sb.Append("\t"+Left!.GenerateTabbed(leftHolder, header));
                sb.AppendLine($"\tVar* {rightHolder} = &NIL;");
                sb.Append("\t"+Right.GenerateTabbed(rightHolder, header));
                sb.AppendLine($"\t{stackName} = {leftHolder};");
                string metamethodHolder = UniqueValueName("metamethod");
                sb.AppendLine($"\tVar* {metamethodHolder} = VarGetMeta({leftHolder}, \"{BinaryOperator.OperatorMetamethods[OperatorBinary.Operator]}\");");            
                string argsHolder = UniqueValueName("args");
                sb.AppendLine($"\tVar* {argsHolder} = &NIL;");
                sb.AppendLine($"\t{argsHolder} = VarNewList({rightHolder});");
                sb.AppendLine($"\tVarRawSet({argsHolder}, VarNewNumber(0), {leftHolder});");
                sb.AppendLine($"\tVarRawSet({argsHolder}, VarNewString(\"left\"), {leftHolder});");
                sb.AppendLine($"\tVarRawSet({argsHolder}, VarNewNumber(1), {rightHolder});");
                sb.AppendLine($"\tVarRawSet({argsHolder}, VarNewString(\"right\"), {rightHolder});");
                sb.AppendLine($"\t{stackName} = VarFunctionCall({metamethodHolder}, {argsHolder});");
                return sb.ToString();
            }
        }else{
            throw new Exception($"Unary math not implemented {OperatorUnary!.Operator}");
        }
    }
}
