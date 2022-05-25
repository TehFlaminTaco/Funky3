using System.Text;

public class Cache : Expression, ILeftProvider {
    Expression Left;

    public Cache(Expression left) {
        Left = left;
    }


    public static (Cache? cache, int index) TryParse(Expression left, List<Token> tokens, int index) {
        int i = index;
        if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "!") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;
        return (new Cache(left), i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName){
        var cacheName = UniqueValueName("cache");
        header.WriteLine($"Var* {cacheName} = &UNDEFINED;");

        StringBuilder sb = new();
        sb.AppendLine("// Cache");
        sb.AppendLine($"\tif(ISUNDEFINED({cacheName})){{");
        string valueBody = Left.GenerateInline(header, out string valueHolder);
        if(!String.IsNullOrEmpty(valueBody)){
            sb.Append(Tabbed(Tabbed(valueBody)));
        }
        sb.AppendLine($"\t\t{cacheName} = {valueHolder};");
        sb.AppendLine($"\t\ttgc_set_flags(&gc, {cacheName}, TGC_ROOT);"); // Make it garbage collection immune... ;-;
        sb.AppendLine("\t}");
        stackName = cacheName;
        return sb.ToString();
    }

    public Expression? GetLeft(){
        return Left;
    }

    public int GetPrecedence(){
        return IExpressionProvider.CALL_PRECEDENCE;
    }

    public void SetLeft(Expression? e){
        Left = e!;
    }
}