public interface IExpressionProvider {
    public const int CALL_PRECEDENCE = 9000;
    public const int NO_PRECEDENCE = 0;
    public int GetPrecedence();
    public virtual bool IsRightLeftAssociative(){
        return false;
    }

    public static Expression Balance (IRightProvider left) {
        if (left is not Expression le){ // Illogical.
            throw new Exception("Left is not an Expression.");
        }
        if(left.GetRight() is not ILeftProvider right || right is not Expression re) {
            return le;
        }
        if(right.GetLeft() is null){
            return re;
        }
        int rPrec = right.GetPrecedence();
        int lPrec = left.GetPrecedence();
        if(rPrec > lPrec || (!left.IsRightLeftAssociative() && rPrec == lPrec)) { // If the right is a higher or equal precedence, leave it as is.
            return le;
        }
        
        if(!left.AcceptRight(right.GetLeft()) || !right.AcceptLeft(left.GetRight())) {
            return le;
        }
        // Get the RIGHT's LEFT expression, and make it LEFT's RIGHT expression.
        left.SetRight(right.GetLeft());
        // SET the RIGHT's LEFT expression to our LEFT expression.
        right.SetLeft(le);
        return re;
    }

    public static Expression Balance(ILeftProvider right){
        if (right is not Expression re){ // Illogical.
            throw new Exception("Left is not an Expression.");
        }
        if(right.GetLeft() is not IRightProvider left || left is not Expression le) {
            return re;
        }
        if(left.GetRight() is null){
            return re;
        }
        int rPrec = right.GetPrecedence();
        int lPrec = left.GetPrecedence();
        if(lPrec > rPrec || (left.IsRightLeftAssociative() && rPrec == lPrec)) { // If the right is a higher or equal precedence, leave it as is.
            return re;
        }
        // Get the LEFT's RIGHT expression, and make it RIGHT's LEFT expression.
        right.SetLeft(left.GetRight());
        // SET the LEFT's RIGHT expression to our RIGHT expression.
        left.SetRight(re);
        return le;
    }

    public static Expression BalanceEither(IExpressionProvider leftRight){
        if(leftRight is not Expression e){
            throw new Exception("LeftRight is not an Expression.");
        }
        if(leftRight is ILeftProvider left){
            var outP = Balance(left);

            if(outP is IExpressionProvider iep){
                if(iep != e){
                    return BalanceEither(iep);
                }
                leftRight = iep;
            }else{
                return outP;
            }
        }
        if(leftRight is IRightProvider right){
            var outP = Balance(right);

            if(outP is IExpressionProvider iep){
                if(iep != e){
                    return BalanceEither(iep);
                }
            }else{
                return outP;
            }
        }
        return e;
    }
}

public interface ILeftProvider : IExpressionProvider {
    public Expression? GetLeft();
    public void SetLeft(Expression? e);
    public virtual bool AcceptLeft(Expression? e){
        return true;
    }
}

public interface IRightProvider : IExpressionProvider {
    public Expression? GetRight();
    public void SetRight(Expression? e);
    public virtual bool AcceptRight(Expression? e){
        return true;
    }
}