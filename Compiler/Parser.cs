public static class Parser {
    public static int FurthestToken = -1;
    public static void Parse(List<Token> tokens, StreamWriter body, StreamWriter header) {
        int index = 0;
        while(index < tokens.Count) {
            (Expression? expression, int index) result = Expression.TryParseAny(tokens, index);
            if(result.expression != null) {
                string lineBody = result.expression.GenerateInline(header, out string stackName);
                if(!String.IsNullOrEmpty(lineBody)) {
                    body.Write(lineBody);
                }
                body.WriteLine($"_ = {stackName};");
                index = result.index;
                // Skip semicolons
                if(tokens[index].Type == TokenType.Punctuation && tokens[index].Value == ";") {
                    index++;
                }
                FurthestToken = index;
            }else{
                throw new Exception($"Unexpected token {tokens[FurthestToken+1]} at line {tokens[FurthestToken+1].Line} column {tokens[FurthestToken+1].Column}");
            }
        }
    }
    public static void RegisterFurthest(int f){
        if(f > FurthestToken) {
            FurthestToken = f;
        }
    }
}