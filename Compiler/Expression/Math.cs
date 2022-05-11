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
        if(OperatorBinary != null) {
            sb.AppendLine($"// {OperatorBinary.Operator}");
            // Custom behaviour for && and ||
            if(OperatorBinary.Operator == "&&") {
                string resultName = UniqueValueName("result");
                stackName = resultName;
                string leftBody = Left!.GenerateInline(header, out string leftHolder);
                if(!String.IsNullOrEmpty(leftBody)) {
                    sb.AppendLine(Tabbed(leftBody));
                }
                sb.AppendLine($"\tVar* {resultName} = {leftHolder};");
                sb.AppendLine($"\tif(VarTruthy({resultName})){{");
                string rightBody = Right!.GenerateInline(header, out string rightHolder);
                if(!String.IsNullOrEmpty(rightBody)) {
                    sb.AppendLine(Tabbed(rightBody));
                }
                sb.AppendLine($"\t\t{resultName} = {rightHolder};");
                sb.AppendLine($"\t}}");
            }else if(OperatorBinary.Operator == "||") {
                string resultName = UniqueValueName("result");
                stackName = resultName;
                string leftBody = Left!.GenerateInline(header, out string leftHolder);
                if(!String.IsNullOrEmpty(leftBody)) {
                    sb.AppendLine(Tabbed(leftBody));
                }
                sb.AppendLine($"\tVar* {resultName} = {leftHolder};");
                sb.AppendLine($"\tif(!VarTruthy({resultName})){{");
                string rightBody = Right!.GenerateInline(header, out string rightHolder);
                if(!String.IsNullOrEmpty(rightBody)) {
                    sb.AppendLine(Tabbed(rightBody));
                }
                sb.AppendLine($"\t\t{resultName} = {rightHolder};");
                sb.AppendLine($"\t}}");
            }else{
                string leftHolder;
                string leftBody = Left!.GenerateInline(header, out leftHolder);
                if(!String.IsNullOrEmpty(leftBody)) {
                    sb.AppendLine("\t// Left");
                    sb.AppendLine(Tabbed(leftBody));
                }
                {
                    string tempHolder = UniqueValueName("left");
                    sb.AppendLine($"\tVar* {tempHolder} = {leftHolder};");
                    leftHolder = tempHolder;
                }
                string rightHolder;
                string rightBody = Right!.GenerateInline(header, out rightHolder);
                if(!String.IsNullOrEmpty(rightBody)) {
                    sb.AppendLine("\t// Right");
                    sb.AppendLine(Tabbed(rightBody));
                }
                string metamethodHolder = $"VarGetMeta({leftHolder}, \"{BinaryOperator.OperatorMetamethods[OperatorBinary.Operator]}\")";
                string argsHolder = UniqueValueName("args");
                sb.AppendLine($"\tVar* {argsHolder} = VarNewList({rightHolder});");
                sb.AppendLine($"\tArgVarSet({argsHolder}, 0, \"left\", {leftHolder});");
                sb.AppendLine($"\tArgVarSet({argsHolder}, 1, \"right\", {rightHolder});");
                stackName = $"VarFunctionCall({metamethodHolder}, {argsHolder})";
            }
            return sb.ToString();
        }else if(OperatorUnary != null) {
            throw new Exception($"Unary math not implemented {OperatorUnary!.Operator}");
        }
        stackName = "&NIL";
        return "";  
    }
}
