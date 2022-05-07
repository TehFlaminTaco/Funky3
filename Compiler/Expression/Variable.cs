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

    public override void Generate(StreamWriter body, StreamWriter header) {
        body.Write($"{Name}");
    }
}