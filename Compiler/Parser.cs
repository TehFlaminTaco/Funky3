public static class Parser {
    
    public static void Parse(List<Token> tokens) {
        int index = 0;
        while(index < tokens.Count) {
            (Expression? expression, int index) result = Expression.TryParseAny(tokens, index);
            if(result.expression != null) {
                index = result.index;
                // Skip semicolons
                if(tokens[index].Type == TokenType.Punctuation && tokens[index].Value == ";") {
                    index++;
                }
            }else{
                throw new Exception($"Unexpected token {tokens[index]} at line {tokens[index].Line} column {tokens[index].Column}");
            }
        }
    }
}