using System.Text;
// v++, v--
// ++v, --v
public class Crementor : Expression {
    public Variable Variable {get; set;}
    public bool IsDecrement {get; set;}
    public bool IsPrefix {get; set;}

    public Crementor(Variable variable, bool isDecrement, bool isPrefix) {
        Variable = variable;
        IsDecrement = isDecrement;
        IsPrefix = isPrefix;
    }

    public static (Crementor?, int) TryParse(List<Token> tokens, int index) {
        // Match two + tokens or two - tokens
        int i = index;
        if(tokens[i].Type != TokenType.Punctuation) {
            return (null, index);
        }
        bool isDecrement;
        if(tokens[i].Value == "+") {
            Parser.RegisterFurthest(i);
            i++;
            if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "+") {
                return (null, index);
            }
            Parser.RegisterFurthest(i);
            i++;
            isDecrement = false;
        } else if(tokens[i].Value == "-") {
            Parser.RegisterFurthest(i);
            i++;
            if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "-") {
                return (null, index);
            }
            Parser.RegisterFurthest(i);
            i++;
            isDecrement = true;
        } else {
            return (null, index);
        }

        var expression = Expression.TryParseAny(tokens, i);
        if(expression.expression is not Variable variable){
            return (null, index);
        }
        i = expression.index;

        return (new Crementor(variable, isDecrement, true), i);
    }
    
    public static (Crementor?, int) TryParse(Variable variable, List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.Punctuation) {
            return (null, index);
        }
        bool isDecrement;
        if(tokens[i].Value == "+") {
            Parser.RegisterFurthest(i);
            i++;
            if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "+") {
                return (null, index);
            }
            Parser.RegisterFurthest(i);
            i++;
            isDecrement = false;
        } else if(tokens[i].Value == "-") {
            Parser.RegisterFurthest(i);
            i++;
            if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "-") {
                return (null, index);
            }
            Parser.RegisterFurthest(i);
            i++;
            isDecrement = true;
        } else {
            return (null, index);
        }

        return (new Crementor(variable, isDecrement, false), i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName){
        StringBuilder sb = new();
        if(IsPrefix)
            sb.AppendLine($"// {(IsDecrement ? "--": "++")}var");
        else
            sb.AppendLine($"// var{(IsDecrement ? "--": "++")}");
        string varBody = Variable.GenerateInline(header, out string varHolder);
        if(!String.IsNullOrEmpty(varBody)) {
            sb.Append(Tabbed(varBody));
        }
        if(true){
            string valueHolder = UniqueValueName("value");
            sb.AppendLine($"\tVar* {valueHolder} = {varHolder};");
            varHolder = valueHolder;
        }

        // Call the add or sub metamethods
        string metaMethod = $"VarGetMeta({varHolder}, \"{(IsDecrement ? "sub": "add")}\")";
        string argHolder = UniqueValueName("arg");
        sb.AppendLine($"\tVar* {argHolder} = VarNewList();");
        sb.AppendLine($"\tArgVarSet({argHolder}, 0, \"left\", {varHolder});");
        sb.AppendLine($"\tArgVarSet({argHolder}, 1, \"right\", VarNewNumber(1));");
        string setterBody = Variable.GenerateSetterInline(header, out string setterHolder, $"VarFunctionCall({metaMethod}, {argHolder})");
        if(!String.IsNullOrEmpty(setterBody)) {
            sb.Append(Tabbed(setterBody));
        }
        if(IsPrefix){
            stackName = setterHolder;
        }else{
            stackName = varHolder;
            sb.AppendLine($"\t_ = {setterHolder};");
        }
        return sb.ToString();
    }
}