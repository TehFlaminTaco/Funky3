using System.Text;

public class Variable : Expression {
    public string Name { get; set; }
    public bool IsLocal { get; set; }

    public Variable(string name, bool isLocal) {
        Name = name;
        IsLocal = isLocal;
    }

    public static (Variable?, int) TryParse(List<Token> tokens, int index) {
        int i = index;
        bool isLocal = false;
        if(tokens[index].Type == TokenType.Keyword) {
            if(tokens[index].Value == "var" || tokens[index].Value == "local") {
                Parser.RegisterFurthest(i);
                i++;
                isLocal = true;
            }else{
                return (null, index);
            }
        }
        if(tokens[i].Type != TokenType.Identifier) {
            return (null, index);
        }
        var variable = new Variable(tokens[i].Value, isLocal);
        i++;
        return (variable, i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName) {
        // Can always be inlined.
        stackName = $"/*Get {Name}*/ VarGet(scope, VarNewString(\"{Name}\"))";
        return "";
    }

    public virtual string GenerateSetterInline(StreamWriter header, out string stackName, string value) {
        // Can always be inlined.
        stackName = $"/*Set {Name}*/ VarSet(scope, VarNewString(\"{Name}\"), {value})";
        return "";
    }
}