using System.Text;
public class Call : Expression, ILeftProvider
{
    public Expression Method { get; set; }
    public List<ListEntry> Arguments { get; set; }

    public Call(Expression method, List<ListEntry> arguments)
    {
        Method = method;
        Arguments = arguments;
    }

    public static (Call?, int) TryParse(Expression left, List<Token> tokens, int index)
    {
        int i = index;
        if (tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "(")
        {
            // Try to get a String literal, or a DeOperator
            (StringLiteral?, int) result = StringLiteral.TryParse(tokens, i);
            if (result.Item1 != null)
            {
                return (new Call(left, new List<ListEntry>() { new EntryExpression(result.Item1) }), result.Item2);
            }
            (InterpolatedStringLiteral? interpolated, int) result3 = InterpolatedStringLiteral.TryParse(tokens, i);
            if (result3.interpolated != null)
            {
                return (new Call(left, new List<ListEntry>() { new EntryExpression(result3.interpolated) }), result3.Item2);
            }
            (DeOperator?, int) result2 = DeOperator.TryParse(tokens, i);
            if (result2.Item1 != null)
            {
                return (new Call(left, new List<ListEntry>() { new EntryExpression(result2.Item1) }), result2.Item2);
            }
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;
        var arguments = new List<ListEntry>();
        while (tokens[i].Type != TokenType.Punctuation || tokens[i].Value != ")")
        {
            Parser.RegisterFurthest(i);
            var result = ListEntry.TryParse(tokens, i);
            if (result.listEntry != null)
            {
                i = result.index;
                arguments.Add(result.listEntry);
            }
            else
            {
                return (null, index);
            }
            // Check for a comma
            if (tokens[i].Type == TokenType.Punctuation && tokens[i].Value == ",")
            {
                i++;
            }
        }
        Parser.RegisterFurthest(i);
        i++;
        var call = new Call(left, arguments);
        return (call, i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName)
    {
        // Impossible to inline.
        string vArgs = UniqueValueName("args");
        string listIndexName = UniqueValueName("listIndex");
        StringBuilder sb = new StringBuilder();
        int? knownIndex = null;
        if (Arguments.Count > 0)
        {
            sb.AppendLine("// Call");
            sb.AppendLine($"\tVar* {vArgs} = VarNewList();");
            if (!Arguments.OfType<EntrySplat>().Any())
            {
                knownIndex = 0;
            }
            else
            {
                sb.AppendLine($"\tint {listIndexName} = 0;");
            }
        }
        else
        {
            vArgs = "VarNewList()";
        }

        // Assemble arguments
        int i = 0;
        foreach (var entry in Arguments)
        {
            /*string argBody = argument.GenerateInline(header, out string argName);
            if(!String.IsNullOrEmpty(argBody)) {
                sb.AppendLine($"\t// Arg {i}");
                sb.Append(Tabbed(Tabbed(argBody)));
                sb.AppendLine($"\t\tVarRawSet({vArgs}, VarNewNumber({i++}), {argName});");
            }else{
                //sb.AppendLine($"\t Arg {i} VarRawSet({vArgs}, VarNewNumber({i++}), {argName});");
            }*/
            if (entry is EntryExpression ee)
            {
                string eeBody = ee.Value.GenerateInline(header, out string valueStackName);
                if (!String.IsNullOrEmpty(eeBody))
                {
                    sb.AppendLine($"\t// Expression {i}");
                    sb.AppendLine(Tabbed(Tabbed(eeBody)));
                }
                var increName = knownIndex.HasValue ? knownIndex++.ToString() : $"{listIndexName}++";
                sb.AppendLine($"\tVarRawSet({vArgs}, VarNewNumber({increName}), {valueStackName});");
            }
            else if (entry is EntryNamed en)
            {
                string enBody = en.Value.GenerateInline(header, out string valueStackName);
                if (!String.IsNullOrEmpty(enBody))
                {
                    sb.AppendLine($"\t// Named {i}");
                    sb.AppendLine(Tabbed(Tabbed(enBody)));
                }
                sb.AppendLine($"\tVarRawSet({vArgs}, VarNewString(\"{en.Name}\"), {valueStackName});");
            }
            else if (entry is EntrySplat es)
            {
                string esBody = es.Value.GenerateInline(header, out string valueStackName);
                if (!String.IsNullOrEmpty(esBody))
                {
                    sb.AppendLine($"\t// Splat {i}");
                    sb.AppendLine(Tabbed(Tabbed(esBody)));
                }
                sb.AppendLine($"\tVarCopyListIntoOffset({valueStackName}, {vArgs}, &{listIndexName});");
            }
            else if (entry is EntryUnpack eu)
            {
                string enBody = eu.Value.GenerateInline(header, out string valueStackName);
                sb.AppendLine($"\t// Unpack {i}");
                if (!String.IsNullOrEmpty(enBody))
                {
                    sb.AppendLine(Tabbed(Tabbed(enBody)));
                }
                var BodyStore = UniqueValueName("unpack");
                sb.AppendLine($"\tVar* {BodyStore} = {valueStackName};");
                foreach (string s in eu.Names)
                {
                    sb.AppendLine($"\tVarRawSet({vArgs}, VarNewString(\"{s}\"), VarGet({BodyStore}, VarNewString(\"{s}\")));");
                }
            }
        }
        string methodBody = Method.GenerateInline(header, out string vMethod);
        if (!String.IsNullOrEmpty(methodBody))
        {
            sb.Append(Tabbed(methodBody));
        }
        // Call method
        stackName = $"VarFunctionCall({vMethod}, {vArgs})";
        return sb.ToString();
    }

    public Expression? GetLeft()
    {
        return Method;
    }

    public void SetLeft(Expression? e)
    {
        Method = e!;
    }

    public int GetPrecedence()
    {
        return IExpressionProvider.CALL_PRECEDENCE;
    }
}