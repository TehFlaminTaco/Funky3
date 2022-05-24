public class CodeChunk : Expression {
    public string Name { get; set; }

    public CodeChunk(string name) {
        Name = name;
    }

    public static (CodeChunk?, int) TryParse(List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.Keyword || !tokens[i].Value.StartsWith("$CodeChunk")) {
            return (null, index);
        }
        string name = tokens[i].Value["$CodeChunk".Length..];
        Parser.RegisterFurthest(i);
        i++;

        // Build the expression
        return (new CodeChunk(name), i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName) {
        stackName = "_";

        return CInline.InlineCodeChunks[Name];
    }
}