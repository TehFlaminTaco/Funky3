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

    public override string GenerateInline(StreamWriter header, out string stackName) {
        StringBuilder sb = new();
        string resultName = UniqueValueName("result");
        stackName = resultName;
        if(OperatorBinary != null) {
            sb.AppendLine($"// {OperatorBinary.Operator}");
            sb.AppendLine($"\tVar* {resultName} = &NIL;");
            // Custom behaviour for && and ||
            if(OperatorBinary.Operator == "&&") {
                string leftHolder;
                string leftBody = Left!.GenerateInline(header, out leftHolder);
                if(!String.IsNullOrEmpty(leftBody)) {
                    sb.AppendLine(Tabbed(leftBody));
                }else{
                    string leftName = UniqueValueName("left");
                    sb.AppendLine($"\tVar* {leftName} = {leftHolder};");
                    leftHolder = leftName;
                }
                sb.AppendLine($"\tif(!VarTruthy({leftHolder})){{");
                sb.AppendLine($"\t\t{resultName} = {leftHolder};");
                sb.AppendLine($"\t}}else{{");
                string rightHolder;
                string rightBody = Right!.GenerateInline(header, out rightHolder);
                if(!String.IsNullOrEmpty(rightBody)) {
                    sb.AppendLine(Tabbed(rightBody));
                }else{
                    string rightName = UniqueValueName("right");
                    sb.AppendLine($"\tVar* {rightName} = {rightHolder};");
                    rightHolder = rightName;
                }
                sb.AppendLine($"\t\t{resultName} = {rightHolder};");
                sb.AppendLine($"\t}}");
            }else if(OperatorBinary.Operator == "||") {
                string leftHolder;
                string leftBody = Left!.GenerateInline(header, out leftHolder);
                if(!String.IsNullOrEmpty(leftBody)) {
                    sb.AppendLine(Tabbed(leftBody));
                }else{
                    string leftName = UniqueValueName("left");
                    sb.AppendLine($"\tVar* {leftName} = {leftHolder};");
                    leftHolder = leftName;
                }
                sb.AppendLine($"\tif(VarTruthy({leftHolder})){{");
                sb.AppendLine($"\t\t{resultName} = {leftHolder};");
                sb.AppendLine($"\t}}else{{");
                string rightHolder;
                string rightBody = Right!.GenerateInline(header, out rightHolder);
                if(!String.IsNullOrEmpty(rightBody)) {
                    sb.AppendLine(rightBody);
                }else{
                    string rightName = UniqueValueName("right");
                    sb.AppendLine($"\tVar* {rightName} = {rightHolder};");
                    rightHolder = rightName;
                }
                sb.AppendLine($"\t\t{resultName} = {rightHolder};");
                sb.AppendLine($"\t}}");
            }else{
                string leftHolder;
                sb.AppendLine("\t// Left");
                string leftBody = Left!.GenerateInline(header, out leftHolder);
                if(!String.IsNullOrEmpty(leftBody)) {
                    sb.AppendLine(Tabbed(leftBody));
                }else{
                    string leftName = UniqueValueName("left");
                    sb.AppendLine($"\t\tVar* {leftName} = {leftHolder};");
                    leftHolder = leftName;
                }
                string rightHolder;
                string rightBody = Right!.GenerateInline(header, out rightHolder);
                sb.AppendLine("\t// Right");
                if(!String.IsNullOrEmpty(rightBody)) {
                    sb.AppendLine(Tabbed(rightBody));
                }else{
                    string rightName = UniqueValueName("right");
                    sb.AppendLine($"\t\tVar* {rightName} = {rightHolder};");
                    rightHolder = rightName;
                }
                string metamethodHolder = $"VarGetMeta({leftHolder}, \"{BinaryOperator.OperatorMetamethods[OperatorBinary.Operator]}\")";
                string argsHolder = UniqueValueName("args");
                sb.AppendLine($"\tVar* {argsHolder} = &NIL;");
                sb.AppendLine($"\t{argsHolder} = VarNewList({rightHolder});");
                sb.AppendLine($"\tVarRawSet({argsHolder}, VarNewNumber(0), {leftHolder});");
                sb.AppendLine($"\tVarRawSet({argsHolder}, VarNewString(\"left\"), {leftHolder});");
                sb.AppendLine($"\tVarRawSet({argsHolder}, VarNewNumber(1), {rightHolder});");
                sb.AppendLine($"\tVarRawSet({argsHolder}, VarNewString(\"right\"), {rightHolder});");
                sb.AppendLine($"\t{resultName} = VarFunctionCall({metamethodHolder}, {argsHolder});");
            }
            return sb.ToString();
        }else if(OperatorUnary != null) {
            throw new Exception($"Unary math not implemented {OperatorUnary!.Operator}");
        }
        return "";  
    }
}
