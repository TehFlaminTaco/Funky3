using System.Text;
public abstract class Loop : Expression{
    public static Stack<(string breakLabel, string breakHolder)> BreakValues = new();
}

public class While : Loop {
    public Expression Condition {get; set;}
    public Expression Body {get; set;}

    public While(Expression condition, Expression body) {
        Condition = condition;
        Body = body;
    }

    public static (While?, int) TryParse(List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.Keyword || tokens[i].Value != "while") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;

        var condition = Expression.TryParseAny(tokens, i);
        if(condition.expression == null) {
            return (null, index);
        }
        i = condition.index;

        var body = Expression.TryParseAny(tokens, i);
        if(body.expression == null) {
            return (null, index);
        }
        i = body.index;

        return (new While(condition.expression, body.expression), i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName){
        StringBuilder sb = new();
        string labelEnd = UniqueValueName("whileEnd");
        string labelStart = UniqueValueName("whileStart");
        string result = UniqueValueName("whileResult");
        BreakValues.Push((labelEnd, result));
        sb.AppendLine($"// While");
        sb.AppendLine($"\tVar* {result} = &NIL;");
        sb.AppendLine($"\t{labelStart}:");
        // First condition iteration
        string conditionBody = Condition.GenerateInline(header, out string conditionHolder);
        if(!String.IsNullOrEmpty(conditionBody)) {
            sb.Append(Tabbed(conditionBody));
        }
        sb.AppendLine($"\tif(!VarTruthy({conditionHolder})) {{");
        sb.AppendLine($"\t\tgoto {labelEnd};");
        sb.AppendLine($"\t}}");
        // Body
        string bodyBody = Body.GenerateInline(header, out string bodyHolder);
        if(!String.IsNullOrEmpty(bodyBody)) {
            sb.Append(Tabbed(bodyBody));
        }
        sb.AppendLine($"\t{result} = {bodyHolder};");
        sb.AppendLine($"\tgoto {labelStart};");
        sb.AppendLine($"\t{labelEnd}:");
        BreakValues.Pop();
        stackName = result;
        return sb.ToString();
    }
}

public class For : Loop {
    public Expression? Initializer { get; set; }
    public Expression? Condition { get; set; }
    public Expression? Increment { get; set; }
    public Expression Body { get; set; }

    public For(Expression? initializer, Expression? condition, Expression? increment, Expression body) {
        Initializer = initializer;
        Condition = condition;
        Increment = increment;
        Body = body;
    }

    public static (For?, int) TryParse(List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.Keyword || tokens[i].Value != "for") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;

        // Get up to three statements, surrounded by parentheses
        List<Expression> statements = new();
        if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "(") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;
        while(statements.Count < 3 && (tokens[i].Type != TokenType.Punctuation || tokens[i].Value != ")")) {
            (var statement, int j) = Expression.TryParseAny(tokens, i);
            if(statement == null) {
                return (null, index);
            }
            i = j;
            statements.Add(statement);

            // Skip a semicolon
            if(tokens[i].Type == TokenType.Punctuation && tokens[i].Value == ";") {
                Parser.RegisterFurthest(i);
                i++;
            }
        }
        if(statements.Count < 3 || tokens[i].Type != TokenType.Punctuation || tokens[i].Value != ")") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;

        // Get an expression
        (var expression, int k) = Expression.TryParseAny(tokens, i);
        if(expression == null) {
            return (null, index);
        }
        i = k;
        
        // Return the statement
        return (new For(statements.Count > 0 ? statements[0] : null, statements.Count > 1 ? statements[1] : null, statements.Count > 2 ? statements[2] : null, expression), i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName){
        StringBuilder sb = new();
        string labelEnd = UniqueValueName("forEnd");
        string labelStart = UniqueValueName("forStart");
        string result = UniqueValueName("forResult");
        BreakValues.Push((labelEnd, result));
        sb.AppendLine($"// For");
        sb.AppendLine($"\tVar* {result} = &NIL;");
        // Initializer
        if(Initializer != null) {
            string initializerBody = Initializer.GenerateInline(header, out string initializerHolder);
            if(!String.IsNullOrEmpty(initializerBody)) {
                sb.Append(Tabbed(initializerBody));
            }
            sb.AppendLine($"\t_ = {initializerHolder};");
        }
        sb.AppendLine($"\t{labelStart}:");
        // Condition
        if(Condition != null) {
            string conditionBody = Condition.GenerateInline(header, out string conditionHolder);
            if(!String.IsNullOrEmpty(conditionBody)) {
                sb.Append(Tabbed(conditionBody));
            }
            sb.AppendLine($"\tif(!VarTruthy({conditionHolder})) {{");
            sb.AppendLine($"\t\tgoto {labelEnd};");
            sb.AppendLine($"\t}}");
        }
        // Body
        string bodyBody = Body.GenerateInline(header, out string bodyHolder);
        if(!String.IsNullOrEmpty(bodyBody)) {
            sb.Append(Tabbed(bodyBody));
        }
        sb.AppendLine($"\t{result} = {bodyHolder};");
        // Increment
        if(Increment != null) {
            string incrementBody = Increment.GenerateInline(header, out string incrementHolder);
            if(!String.IsNullOrEmpty(incrementBody)) {
                sb.Append(Tabbed(incrementBody));
            }
            sb.AppendLine($"\t_ = {incrementHolder};");
        }
        sb.AppendLine($"\tgoto {labelStart};");
        sb.AppendLine($"\t{labelEnd}:");
        BreakValues.Pop();
        stackName = result;
        return sb.ToString();
    }
}

public class ForIn : Loop {
    public Variable? Variable { get; set; }
    public List<Variable>? Variables { get; set; }
    public Expression Collection { get; set; }
    public Expression Body { get; set; }

    public ForIn(Variable variable, Expression collection, Expression body) {
        Variable = variable;
        Collection = collection;
        Body = body;
    }

    public ForIn(List<Variable> variables, Expression collection, Expression body) {
        Variables = variables;
        Collection = collection;
        Body = body;
    }

    // for var in collection { body }
    // for (var in collection) { body }
    public static (ForIn?, int) TryParse(List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.Keyword || tokens[i].Value != "for") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;

        Variable? variable = null;
        List<Variable>? variables = null;

        // Unpack List into variables.
        if(tokens[i].Type == TokenType.Punctuation && tokens[i].Value == "(") {
            Parser.RegisterFurthest(i);
            i++;

            // Get a bunch of variables, seperated optionally by commas.
            variables = new();
            while(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != ")") {
                (var v, int j) = Variable.TryParse(tokens, i);
                if(v == null) {
                    return (null, index);
                }
                i = j;
                variables.Add(v);

                // Skip a comma
                if(tokens[i].Type == TokenType.Punctuation && tokens[i].Value == ",") {
                    Parser.RegisterFurthest(i);
                    i++;
                }
            }

            // Get the right parenthesis
            if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != ")") {
                return (null, index);
            }
            Parser.RegisterFurthest(i);
            i++;


        // Just one variable.
        }else{
            // Get the variable
            (variable, int j) = Variable.TryParse(tokens, i);
            if(variable == null) {
                return (null, index);
            }
            i = j;
        }

        // Get the in
        if(tokens[i].Type != TokenType.Keyword || tokens[i].Value != "in") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;

        // Get the collection
        (var collection, int k) = Expression.TryParseAny(tokens, i);
        if(collection == null) {
            return (null, index);
        }
        i = k;

        (var body, int l) = Expression.TryParseAny(tokens, i);
        if(body == null) {
            return (null, index);
        }
        i = l;

        // Return the statement
        if(variable is not null)
            return (new ForIn(variable, collection, body), i);
        else
            return (new ForIn(variables!, collection, body), i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName){
        StringBuilder sb = new();
        string labelEnd = UniqueValueName("forInEnd");
        string labelStart = UniqueValueName("forInStart");
        string result = UniqueValueName("forInResult");
        string nextHolder = UniqueValueName("next");
        BreakValues.Push((labelEnd, result));
        sb.AppendLine($"// ForIn");
        sb.AppendLine($"\tVar* {result} = &NIL;");
        sb.AppendLine($"\tVar* {nextHolder} = &NIL;");
        // Get the collection
        string collectionBody = Collection.GenerateInline(header, out string collectionHolder);
        if(!String.IsNullOrEmpty(collectionBody)) {
            sb.Append(Tabbed(collectionBody));
        }
        if(true){
            string tempHolder = UniqueValueName("collection");
            sb.AppendLine($"\tVar* {tempHolder} = {collectionHolder};");
            collectionHolder = tempHolder;
        }
        // Get the iterator
        string iterHolder = UniqueValueName("iter");
        // Call the collection's iter metamethod to get it's iterator
        string argHolder = UniqueValueName("arg");
        sb.AppendLine($"\tVar* {iterHolder} = {collectionHolder};");
        sb.AppendLine($"\tVar* {argHolder} = VarNewList();");
        sb.AppendLine($"\tif({iterHolder}->type != VAR_FUNCTION) {{");
        sb.AppendLine($"\t\tArgVarSet({argHolder}, 0, \"obj\", {collectionHolder});");
        sb.AppendLine($"\t\t{iterHolder} = VarAsFunction(VarFunctionCall(VarGetMeta({collectionHolder}, \"iter\"), {argHolder}));");
        sb.AppendLine($"\t}}");
        sb.AppendLine($"\tif({iterHolder}->type != VAR_FUNCTION) {{");
        sb.AppendLine($"\t\tgoto {labelEnd};");
        sb.AppendLine($"\t}}");

        sb.AppendLine($"\t{labelStart}:");
        // Get the next item
        sb.AppendLine($"\t{argHolder} = VarNewList();");
        sb.AppendLine($"\tArgVarSet({argHolder}, 0, \"obj\", {collectionHolder});");
        sb.AppendLine($"\tArgVarSet({argHolder}, 1, \"last\", {nextHolder});");
        sb.AppendLine($"\t{nextHolder} = VarFunctionCall({iterHolder}, {argHolder});");

        // Singular Variable
        if(Variable is not null){
            sb.AppendLine($"\tif(ISUNDEFINED({nextHolder})) {{");
            sb.AppendLine($"\t\tgoto {labelEnd};");
            sb.AppendLine($"\t}}");
            var variableBody = Variable.GenerateSetterInline(header, out string variableHolder, nextHolder);
            if(!String.IsNullOrEmpty(variableBody)) {
                sb.Append(Tabbed(variableBody));
            }
            sb.AppendLine($"\t_ = {variableHolder};");
        }else{
            sb.AppendLine($"\tif({nextHolder} -> type != VAR_LIST) {{");
            sb.AppendLine($"\t\tgoto {labelEnd};");
            sb.AppendLine($"\t}}");
            int j = 0;
            foreach(var variable in Variables!){
                var variableBody = variable.GenerateSetterInline(header, out string variableHolder, $"VarRawGet({nextHolder}, VarNewNumber({j++}))");
                if(!String.IsNullOrEmpty(variableBody)) {
                    sb.Append(Tabbed(variableBody));
                }
                sb.AppendLine($"\t_ = {variableHolder};");
            }
        }
        // Call the body
        string bodyBody = Body.GenerateInline(header, out string bodyHolder);
        if(!String.IsNullOrEmpty(bodyBody)) {
            sb.Append(Tabbed(bodyBody));
        }
        sb.AppendLine($"\t{result} = {bodyHolder};");
        sb.AppendLine($"\tgoto {labelStart};");
        sb.AppendLine($"\t{labelEnd}:");
        BreakValues.Pop();
        stackName = result;
        return sb.ToString();
    }
}