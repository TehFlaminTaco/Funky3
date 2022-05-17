using System.Linq.Expressions;
public class Parentheses : Expression {
    public Expression Expression { get; set; }
    public Parentheses(Expression expression) {
        Expression = expression;
    }

    public static (Parentheses?, int) TryParse(List<Token> tokens, int index) {
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
        return (new Parentheses(expression.expression), i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName){
        return Expression.GenerateInline(header, out stackName);
    }
}