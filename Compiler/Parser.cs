public static class Parser {
    public static int FurthestToken = 0;
    public static void Parse(List<Token> tokens, StreamWriter body, StreamWriter header) {
        FurthestToken = 0;
        int index = 0;
        while(index < tokens.Count && tokens[index].Type != TokenType.None) {
            (Expression? expression, int index) result = Expression.TryParseAny(tokens, index);
            if(result.expression != null) {
                string lineBody = result.expression.GenerateInline(header, out string stackName);
                if(!String.IsNullOrEmpty(lineBody)) {
                    body.Write(lineBody);
                }
                body.WriteLine($"_ = {stackName};");
                index = result.index;
                if(index >= tokens.Count) {
                    break;
                }
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