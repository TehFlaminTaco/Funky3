public static class Parser {
    public static int FurthestToken = 0;
    public static void Parse(List<Token> tokens, StreamWriter body, StreamWriter header) {
        int index = 0;
        while(index < tokens.Count) {
            (Expression? expression, int index) result = Expression.TryParseAny(tokens, index);
            if(result.expression != null) {
                body.WriteLine(result.expression.Generate("_", header));
                index = result.index;
                // Skip semicolons
                if(tokens[index].Type == TokenType.Punctuation && tokens[index].Value == ";") {
                    index++;
                }
                FurthestToken = index;
            }else{
                throw new Exception($"Unexpected token {tokens[FurthestToken]} at line {tokens[FurthestToken].Line} column {tokens[FurthestToken].Column}");
            }
        }
    }
    public static void RegisterFurthest(int f){
        if(f > FurthestToken) {
            FurthestToken = f;
        }
    }
}