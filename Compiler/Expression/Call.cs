using System.Text;
public class Call : Expression, ILeftProvider {
    public Expression Method { get; set; }
    public List<ListEntry> Arguments { get; set; }

    public Call(Expression method, List<ListEntry> arguments) {
        Method = method;
        Arguments = arguments;
    }

    public static (Call?, int) TryParse(Expression left, List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "(") {
            // Try to get a String literal, or a DeOperator
            (StringLiteral?, int) result = StringLiteral.TryParse(tokens, i);
            if(result.Item1 != null) {
                return (new Call(left, new List<ListEntry>(){new EntryExpression(result.Item1)}), result.Item2);
            }
            (DeOperator?, int) result2 = DeOperator.TryParse(tokens, i);
            if(result2.Item1 != null) {
                return (new Call(left, new List<ListEntry>(){new EntryExpression(result2.Item1)}), result2.Item2);
            }
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;
        var arguments = new List<ListEntry>();
        while(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != ")") {
            Parser.RegisterFurthest(i);
            var result = ListEntry.TryParse(tokens, i);
            if(result.listEntry != null) {
                i = result.index;
                arguments.Add(result.listEntry);
            }else{
                return (null, index);
            }
            // Check for a comma
            if(tokens[i].Type == TokenType.Punctuation && tokens[i].Value == ",") {
                i++;
            }
        }
        Parser.RegisterFurthest(i);
        i++;
        var call = new Call(left, arguments);
        return (call, i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName) {
        // Impossible to inline.
        string retValue = UniqueValueName("retValue");
        string vArgs = UniqueValueName("args");
        string listIndexName = UniqueValueName("listIndex");
        StringBuilder sb = new StringBuilder();
        sb.AppendLine("// Call");
        sb.AppendLine($"\tVar* {retValue};");
        sb.AppendLine($"\tVar* {vArgs} = VarNewList();");
        sb.AppendLine($"\tint {listIndexName} = 0;");
        // Assemble arguments
        int i = 0;
        foreach(var entry in Arguments) {
            /*string argBody = argument.GenerateInline(header, out string argName);
            if(!String.IsNullOrEmpty(argBody)) {
                sb.AppendLine($"\t// Arg {i}");
                sb.Append(Tabbed(Tabbed(argBody)));
                sb.AppendLine($"\t\tVarRawSet({vArgs}, VarNewNumber({i++}), {argName});");
            }else{
                //sb.AppendLine($"\t Arg {i} VarRawSet({vArgs}, VarNewNumber({i++}), {argName});");
            }*/
            if(entry is EntryExpression ee){
                string eeBody = ee.Value.GenerateInline(header, out string valueStackName);
                if(!String.IsNullOrEmpty(eeBody)) {
                    sb.AppendLine($"\t// Expression {i}");
                    sb.AppendLine(Tabbed(Tabbed(eeBody)));
                }
                sb.AppendLine($"\tVarRawSet({vArgs}, VarNewNumber({listIndexName}++), {valueStackName});");
            }else if(entry is EntryNamed en){
                string enBody = en.Value.GenerateInline(header, out string valueStackName);
                if(!String.IsNullOrEmpty(enBody)) {
                    sb.AppendLine($"\t// Named {i}");
                    sb.AppendLine(Tabbed(Tabbed(enBody)));
                }
                sb.AppendLine($"\tVarRawSet({vArgs}, VarNewString(\"{en.Name}\"), {valueStackName});");
            }else if(entry is EntrySplat es){
                string esBody = es.Value.GenerateInline(header, out string valueStackName);
                if(!String.IsNullOrEmpty(esBody)) {
                    sb.AppendLine($"\t// Splat {i}");
                    sb.AppendLine(Tabbed(Tabbed(esBody)));
                }
                sb.AppendLine($"\tVarCopyListIntoOffset({valueStackName}, {vArgs}, &{listIndexName});");
            }
        }
        string methodBody = Method.GenerateInline(header, out string vMethod);
        if(!String.IsNullOrEmpty(methodBody)) {
            sb.Append(Tabbed(methodBody));
        }
        // Call method
        sb.AppendLine($"\t{retValue} = VarFunctionCall({vMethod}, {vArgs});");
        stackName = retValue;
        return sb.ToString();
    }

    public Expression? GetLeft(){
        return Method;
    }

    public void SetLeft(Expression? e){
        Method = e!;
    }

    public int GetPrecedence(){
        return IExpressionProvider.CALL_PRECEDENCE;
    }
}