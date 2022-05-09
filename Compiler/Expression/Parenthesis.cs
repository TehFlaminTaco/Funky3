using System.Linq.Expressions;
public static class Parenthesis {
    public static (Expression?, int) TryParse(List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "(") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;
        var expression = Expression.TryParseAny(tokens, i);
        if(expression.expression == null) {
            return (null, index);
        }
        i = expression.index;
        if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != ")") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;
        return (expression.expression, i);
    }
}