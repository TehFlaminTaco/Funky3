using System.Text;

public class StringLiteral : Expression {
    public string Value { get; set; }
    public StringLiteral(string value) {
        Value = value;
    }

    public static (StringLiteral?, int) TryParse(List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.String) {
            return (null, index);
        }
        var literal = new StringLiteral(tokens[i].Value);
        i++;
        return (literal, i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName) {
        stackName = $"/* String: {Value.Replace('/', '_')} */ VarNewString({Value.Replace("\r", "\\r").Replace("\n", "\\n").Replace("\t", "\\t")})";
        return "";
    }
}

public class InterpolatedStringLiteral : Expression {
    public List<Expression> Expressions { get; set; }
    public List<string> Strings { get; set; }
    public InterpolatedStringLiteral(List<Expression> expressions, List<string> strings) {
        Expressions = expressions;
        Strings = strings;
        if(Expressions.Count != Strings.Count - 1) {
            throw new Exception("Interpolated string literal must have one more expression than string.");
        }
    }

    public static (InterpolatedStringLiteral?, int) TryParse(List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type == TokenType.InterpolatedStringTEXT){ // Text only.
            var literal = new InterpolatedStringLiteral(new List<Expression>(), new List<string>(){tokens[i].Value[1..^1]});
            i++;
            return (literal, i);
        }
        if(tokens[i].Type != TokenType.InterpolatedStringSTART) {
            return (null, index);
        }
        var expressions = new List<Expression>();
        var strings = new List<string>();
        strings.Add(tokens[i].Value[1..^1]);
        i++;
        // Grab an Expression, then a string, then an expression, then a string, etc.
        while(true) {
            var (expression, newIndex) = Expression.TryParseAny(tokens, i);
            if(expression == null) {
                return (null, index);
            }
            i = newIndex;
            expressions.Add(expression);
            if(tokens[i].Type == TokenType.InterpolatedStringEND) {
                strings.Add(tokens[i].Value[1..^1]);
                i++;
                return (new InterpolatedStringLiteral(expressions, strings), i);
            }
            if(tokens[i].Type != TokenType.InterpolatedStringTEXT) {
                return (null, index);
            }
            strings.Add(tokens[i].Value[1..^1]);
            i++;
        }
    }

    private static string ToDoubleQoute(string s){
        return "\"" + s.Replace("\"", "\\\"")
                .Replace("\r", "\\r")
                .Replace("\n", "\\n")
                .Replace("\t", "\\t") + "\"";
    }

    public override string GenerateInline(StreamWriter header, out string stackName)
    {
        // Single string case
        if(Expressions.Count == 0) {
            stackName = $"/* String: `{Strings[0].Replace('/', '_')}` */ VarNewString({ToDoubleQoute(Strings[0])})";
            return "";
        }
        // Multiple string case
        var sb = new StringBuilder();
        sb.AppendLine("/* Interpolated String */");
        var stringLengthHolder = UniqueValueName("stringLength");
        sb.AppendLine($"int {stringLengthHolder} = 0;");
        int stringIndex = 0;
        var stringHolders = new List<string>();
        var stringLengthHolders = new List<string>();
        for (var i = 0; i < Strings.Count; i++){
            // First, the string
            if (Strings[i].Length > 0)
            {
                stringHolders.Add(UniqueValueName("string" + stringIndex));
                stringLengthHolders.Add(UniqueValueName("stringLength" + stringIndex));
                sb.AppendLine($"char* {stringHolders[stringIndex]} = {ToDoubleQoute(Strings[i])};");
                sb.AppendLine($"int {stringLengthHolders[stringIndex]} = strlen({stringHolders[stringIndex]});");
                sb.AppendLine($"{stringLengthHolder} += {stringLengthHolders[stringIndex]};");
                stringIndex++;
            }
            // Then, if this isn't the LAST string, the expression
            if(i != Strings.Count - 1) {
                var s = Expressions[i].GenerateInline(header, out var expressionStack);
                if (s.Length > 0)
                {
                    sb.AppendLine("// Expression");
                    sb.AppendLine(Tabbed(s));
                }
                stringHolders.Add(UniqueValueName("string" + stringIndex));
                stringLengthHolders.Add(UniqueValueName("stringLength" + stringIndex));
                sb.AppendLine($"char* {stringHolders[stringIndex]} = (char*)(VarAsString({expressionStack}) -> value);");
                sb.AppendLine($"int {stringLengthHolders[stringIndex]} = strlen({stringHolders[stringIndex]});");
                sb.AppendLine($"{stringLengthHolder} += {stringLengthHolders[stringIndex]};");
                stringIndex++;
            }
        }
        var finalStringHolder = UniqueValueName("finalString");
        sb.AppendLine($"char* {finalStringHolder} = malloc({stringLengthHolder} + 1);");
        var stringIndexHolder = UniqueValueName("stringIndex");
        sb.AppendLine($"int {stringIndexHolder} = 0;");
        for (var j = 0; j < stringIndex; j++){
            sb.Append($"memcpy({finalStringHolder} + {stringIndexHolder}, {stringHolders[j]}, {stringLengthHolders[j]}); ");
            sb.AppendLine($"{stringIndexHolder} += {stringLengthHolders[j]};");
        }
        sb.AppendLine($"{finalStringHolder}[{stringLengthHolder}] = '\\0';");
        var outString = UniqueValueName("outString");
        sb.AppendLine($"Var* {outString} = VarNewString({finalStringHolder});");
        sb.AppendLine($"free({finalStringHolder});");
        stackName = outString;
        return sb.ToString();
    }
}