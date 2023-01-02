using System.Text;
public class DeOperator : Expression, IRightProvider
{
    public BinaryOperator? BinaryOp {get; set;}
    public UnaryOperator? UnaryOp {get; set;}
    public Expression? Expr {get; set;}
    public bool indexed = false;
    public string Code {get; set;}

    public DeOperator(UnaryOperator? unaryOperator, BinaryOperator? binaryOperator, string code){
        BinaryOp = binaryOperator;
        UnaryOp = unaryOperator;
        Code = code;
    }
    public DeOperator(Expression expression, string code){
        Expr = expression;
        Code = code;
    }

    public static (DeOperator? deop, int index) TryParse(List<Token> tokens, int index){
        int i = index;
        if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "@"){
            return (null, index);
        }

        Parser.RegisterFurthest(i);
        i++;

        // Get a unary operator
        var unaryOperator = UnaryOperator.TryParse(tokens, i);
        // Get a binary operator
        var binaryOperator = BinaryOperator.TryParse(tokens, i);
        int start = tokens[index].Index;
        // If we got both a unary, and a binary.
        if(binaryOperator.Item1!=null && unaryOperator.Item1 != null){
            if(binaryOperator.Item2 == unaryOperator.Item2){
                int end = tokens[binaryOperator.Item2-1].Index + tokens[binaryOperator.Item2-1].Length;
                string code = Compiler.CurrentCode[start..end];
                return (new DeOperator(unaryOperator.Item1, binaryOperator.Item1, code), binaryOperator.Item2);
            }
            else if(binaryOperator.Item2 > unaryOperator.Item2){
                int end = tokens[binaryOperator.Item2-1].Index + tokens[binaryOperator.Item2-1].Length;
                string code = Compiler.CurrentCode[start..end];
                return (new DeOperator(null, binaryOperator.Item1, code), binaryOperator.Item2);
            }else{
                int end = tokens[unaryOperator.Item2-1].Index + tokens[unaryOperator.Item2-1].Length;
                string code = Compiler.CurrentCode[start..end];
                return (new DeOperator(unaryOperator.Item1, null, code), unaryOperator.Item2);
            }
        }
        if(binaryOperator.Item1 != null){
            int end = tokens[binaryOperator.Item2-1].Index + tokens[binaryOperator.Item2-1].Length;
            string code = Compiler.CurrentCode[start..end];
            return (new DeOperator(null, binaryOperator.Item1, code), binaryOperator.Item2);
        }
        if(unaryOperator.Item1 != null){
            int end = tokens[unaryOperator.Item2-1].Index + tokens[unaryOperator.Item2-1].Length;
            string code = Compiler.CurrentCode[start..end];
            return (new DeOperator(unaryOperator.Item1, null, code), unaryOperator.Item2);
        }
        // Try get get an INDEX expression
        Variable doepHolder = new DeopVar();
        var indexExpr = RightParse(IndexVariable.TryParse(doepHolder, tokens, i), tokens);
        if(indexExpr.expression != null){
            int end = tokens[indexExpr.index-1].Index + tokens[indexExpr.index-1].Length;
            string code = Compiler.CurrentCode[start..end];
            return (new DeOperator(indexExpr.expression, code){indexed = true}, indexExpr.index);
        }

        // Get an expression
        var expression = Expression.TryParseAny(tokens, i);
        if(expression.expression != null){
            int end = tokens[expression.index-1].Index + tokens[expression.index-1].Length;
            string code = Compiler.CurrentCode[start..end];
            return (new DeOperator(expression.expression, code), expression.index);
        }
        return (null, index);
    }

    public static Dictionary<string, string> KnownOperators = new();
    public override string GenerateInline(StreamWriter header, out string stackName){
        StringBuilder sb = new();
        sb.Append('\"');
        foreach(char c in Code) {
            switch(c){
                case '\a':
                    sb.Append("\\a"); break;
                case '\b':
                    sb.Append("\\b"); break;
                case '\x1B':
                    sb.Append("\\e"); break;
                case '\f':
                    sb.Append("\\f"); break;
                case '\n':
                    sb.Append("\\n"); break;
                case '\r':
                    sb.Append("\\r"); break;
                case '\t':
                    sb.Append("\\t"); break;
                case '\v':
                    sb.Append("\\v"); break;
                case '\\':
                    sb.Append("\\\\"); break;
                case '\'':
                    sb.Append("\\'"); break;
                case '\"':
                    sb.Append("\\\""); break;
                default:
                    if(c < 32 || c > 126){
                        sb.Append($"\\x{c:X2}");
                    }else{
                        sb.Append(c);
                    }
                    break;
            }
        }
        sb.Append('\"');

        if(Expr != null){
            // Generate a method that takes no arguments and runs the code.
            var methodName = UniqueValueName("deop");
            StringBuilder headerSB = new();
            headerSB.AppendLine($"// Deoperator Method");
            Block.PushBadScope();
            Block.PushCurrentScope();
            headerSB.AppendLine($"Var* {methodName}(Var* scope, Var* args){{");
            headerSB.AppendLine($"\tVar* _ = &NIL;");
            if(indexed){
                headerSB.AppendLine($"\tVar* this = ArgVarGet(args, 0, \"this\");");
            }
            headerSB.AppendLine($"\t// Expression");
            var expressionBody = Expr.GenerateInline(header, out string expressionStackName);
            if(!String.IsNullOrEmpty(expressionBody)){
                headerSB.AppendLine(Tabbed(Tabbed(expressionBody)));
            }
            headerSB.AppendLine($"\treturn {expressionStackName};");
            headerSB.AppendLine($"}}");
            header.Write(headerSB.ToString());
            Block.PopCurrentScope();
            Block.PopCurrentScope();

            stackName = $"VarNewFunctionWithScope({methodName}, scope, {sb})";
            return "";
        }else if(BinaryOp != null || UnaryOp != null){
            var opName = BinaryOp?.Operator ?? UnaryOp!.Operator;
            var opMeta = BinaryOp != null ? BinaryOperator.OperatorMetamethods[opName] : UnaryOperator.OperatorMetamethods[opName];
            if( KnownOperators.ContainsKey(BinaryOp?.Operator ?? UnaryOp!.Operator) ){
                var oldMethod = KnownOperators[BinaryOp?.Operator ?? UnaryOp!.Operator];
                stackName = $"VarNewFunctionWithScope({oldMethod}, scope, {sb})";
                return "";
            }
            var methodName = UniqueValueName("deop_"+opMeta);
            StringBuilder headerSB = new();
            headerSB.AppendLine($"// Deoperator {BinaryOp?.Operator ?? UnaryOp!.Operator}");
            headerSB.AppendLine($"Var* {methodName}(Var* scope, Var* args){{");
            Block.PushBadScope();
            Block.PushCurrentScope();
            headerSB.AppendLine($"\tVar* _ = &NIL;");
            headerSB.AppendLine($"\tscope = VarSubScope(scope);");
            headerSB.AppendLine($"\tVar* left = ArgVarGet(args, 0, \"left\");");
            headerSB.AppendLine($"\tVar* right = ArgVarGet(args, 1, \"right\");");
            headerSB.AppendLine($"\tif(ISUNDEFINED(left)){{");
            headerSB.AppendLine($"\t\treturn &NIL;");
            headerSB.AppendLine($"\t}}");
            headerSB.AppendLine($"\tif(ISUNDEFINED(right)){{");
            if(UnaryOp != null){
                // Execute as if a unary operator. !var
                string metamethodHolder = $"VarGetMeta(left, \"{UnaryOperator.OperatorMetamethods[UnaryOp.Operator]}\")";
                string argsHolder = UniqueValueName("args");
                headerSB.AppendLine($"\t\tVar* {argsHolder} = VarNewList();");
                headerSB.AppendLine($"\t\tArgVarSet({argsHolder}, 0, \"value\", left);");
                headerSB.AppendLine($"\t\treturn VarFunctionCall({metamethodHolder}, {argsHolder});");
            }else{
                headerSB.AppendLine($"\t\treturn &NIL;");
            }
            if(BinaryOp != null){
                headerSB.AppendLine($"\t}}else{{");
                // Execute as if a binary operator. var*var
                string metamethodHolder = $"VarGetMeta(left, \"{BinaryOperator.OperatorMetamethods[BinaryOp.Operator]}\")";
                string argsHolder = UniqueValueName("args");
                headerSB.AppendLine($"\t\tVar* {argsHolder} = VarNewList();");
                headerSB.AppendLine($"\t\tArgVarSet({argsHolder}, 0, \"left\", left);");
                headerSB.AppendLine($"\t\tArgVarSet({argsHolder}, 1, \"right\", right);");
                headerSB.AppendLine($"\t\treturn VarFunctionCall({metamethodHolder}, {argsHolder});");
            }
            headerSB.AppendLine($"\t}}");
            headerSB.AppendLine($"\treturn &NIL;");
            Block.PopCurrentScope();
            Block.PopCurrentScope();
            headerSB.AppendLine($"}}");
            header.Write(headerSB.ToString());

            stackName = $"VarNewFunctionWithScope({methodName}, scope, {sb})";
            KnownOperators[BinaryOp?.Operator ?? UnaryOp!.Operator] = methodName;
            return "";
        }
        throw new Exception("Could not generate inline code for deoperator.");
    }

    public Expression? GetRight(){
        return Expr;
    }

    public void SetRight(Expression? e){
        Expr = e;
    }

    public int GetPrecedence(){
        return IExpressionProvider.NO_PRECEDENCE;
    }

    // Stands for the "this" variable.
    private class DeopVar : Variable{
        public override string GenerateInline(StreamWriter header, out string stackName){
            stackName = $"this";
            return "";
        }

        public override string GenerateSetterInline(StreamWriter header, out string stackName, string value)
        {
            stackName = $"(this = {value})";
            return "";
        }
    }
}