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

    public override string Generate(string stackName, StreamWriter header) {
        return $"// Get {Name}\n\t{stackName} = VarGet(scope, VarNewString(\"{Name}\"));\n";
    }

    public virtual string GenerateSetterTabbed(string stackName, string stackValue, StreamWriter header) {
        return Lines.Replace(GenerateSetter(stackName, stackValue, header),  "\t$0");
    }

    public virtual string GenerateSetter(string stackName, string stackValue, StreamWriter header) {
        return $"// Set {Name}\n\t{stackName} = VarSet(scope, VarNewString(\"{Name}\"), {stackValue});\n";
    }
}