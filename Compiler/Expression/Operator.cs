public class BinaryOperator {
    public string Operator { get; set; }

    public BinaryOperator(string operator_) {
        Operator = operator_;
    }

    public static char[] Operators = new char[] {
        '+',
        '-',
        '*',
        '/',
        '%',
        '^',
        '&',
        '|',
        '~',
        '<',
        '>',
    };

    public static Dictionary<string, string> OperatorMetamethods = new() {
        { "+",  "add" },
        { "-",  "sub" },
        { "*",  "mul" },
        { "/",  "div" },
        { "//", "intdiv" },
        { "%",  "mod" },
        { "^",  "pow" },
        { "&",  "band" },
        { "|",  "bor" },
        { "~",  "bnot" },
        { "<",  "lt" },
        { ">",  "gt" },
        { "==", "eq" },
        { "!=", "ne" },
        { "<=", "le" },
        { ">=", "ge" },
        { "<<", "bshl" },
        { ">>", "bshr" },
        { "..", "concat" },
    };



    public static (BinaryOperator?, int) TryParse(List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.Punctuation) {
            if(tokens[i].Type == TokenType.Keyword) {
                // Check for and, or or
                if(tokens[i].Value == "and") {
                    return (new BinaryOperator("&&"), i + 1);
                }
                if(tokens[i].Value == "or") {
                    return (new BinaryOperator("||"), i + 1);
                }
            }
            return (null, index);
        }
        // Check for special cases: "..", "!="
        if(tokens[i].Value == "." && tokens[i+1].Type == TokenType.Punctuation && tokens[i + 1].Value == ".") {
            return (new BinaryOperator(".."), i + 1);
        }
        if(tokens[i].Value == "!" && tokens[i+1].Type == TokenType.Punctuation && tokens[i + 1].Value == "=") {
            return (new BinaryOperator("!="), i + 1);
        }
        // Check the operator is a binary operator.
        if(!Operators.Contains(tokens[i].Value[0])) {
            return (null, index);
        }
        // Check for "Double character" operators
        string firstChar = tokens[i].Value;
        Parser.RegisterFurthest(i);
        i++;
        if(i >= tokens.Count) {
            return (new BinaryOperator(firstChar), i);
        }
        if(tokens[i].Type != TokenType.Punctuation) {
            return (new BinaryOperator(firstChar), i);
        }
        switch(firstChar){
            case "/":
                if(tokens[i].Value == "/") return (new BinaryOperator("//"), i + 1);
                break;
            case "<":
                if(tokens[i].Value == "=") {
                    return (new BinaryOperator("<="), i + 1);
                }
                if(tokens[i].Value == "<") return (new BinaryOperator("<<"), i + 1);
                break;
            case ">":
                if(tokens[i].Value == "=") return (new BinaryOperator(">="), i + 1);
                if(tokens[i].Value == ">") return (new BinaryOperator(">>"), i + 1);
                break;
            case "=":
                if(tokens[i].Value == "=") return (new BinaryOperator("=="), i + 1);
                break;
            case "&":
                if(tokens[i].Value == "&") return (new BinaryOperator("&&"), i + 1);
                break;
            case "|":
                if(tokens[i].Value == "|") return (new BinaryOperator("||"), i + 1);
                break;
        }
        return (new BinaryOperator(firstChar), i);
    }
}

public class UnaryOperator {
    public string Operator { get; set; }
    
    public UnaryOperator(string operator_) {
        Operator = operator_;
    }

    public static char[] Operators = new char[] {
        '+',
        '-',
        '!',
        '~',
        '#',
    };

    public static Dictionary<string, string> OperatorMetamethods = new() {
        {"+", "unp"},
        {"-", "unm"},
        {"!", "not"},
        {"~", "bnot"},
        {"#", "len"},
    };

    public static (UnaryOperator?, int) TryParse(List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.Punctuation) {
            if(tokens[i].Type == TokenType.Keyword && tokens[i].Value == "not") {
                return (new UnaryOperator("!"), i + 1);
            }
            return (null, index);
        }
        // Check that this is an operator
        if(!Operators.Contains(tokens[i].Value[0])) {
            return (null, index);
        }
        return (new UnaryOperator(tokens[i].Value), i + 1);
    }
}