using System.ComponentModel;
using System.Text;
public class ListLiteral : Expression { 
    public List<ListEntry> Entries {get; set;}
    public ListLiteral(List<ListEntry> entries) {
        Entries = entries;
    }

    public static (ListLiteral?, int) TryParse(List<Token> tokens, int index){
        int i = index;
        if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "[") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;

        var entries = new List<ListEntry>();
        while(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "]") {
            var entry = ListEntry.TryParse(tokens, i);
            if(entry.listEntry == null) {
                return (null, index);
            }
            entries.Add(entry.listEntry);
            i = entry.index;

            // Skip a ,
            if(tokens[i].Type == TokenType.Punctuation && tokens[i].Value == ",") {
                i++;
            }
        }
        Parser.RegisterFurthest(i);
        i++;

        return (new ListLiteral(entries), i);
    }

    public override string GenerateInline(StreamWriter header, out string stackName){
        StringBuilder sb = new();
        string newListName = UniqueValueName("newList");
        string listIndexName = UniqueValueName("listIndex");
        stackName = newListName;

        sb.AppendLine($"// ListLiteral");
        sb.AppendLine($"\tVar* {newListName} = VarNewList();");
        sb.AppendLine($"\tint {listIndexName} = 0;");
        
        int i = 0;
        foreach(var entry in Entries){
            if(entry is EntryExpression ee){
                string eeBody = ee.Value.GenerateInline(header, out string valueStackName);
                if(!String.IsNullOrEmpty(eeBody)) {
                    sb.AppendLine($"\t// Expression {i}");
                    sb.AppendLine(Tabbed(Tabbed(eeBody)));
                }
                sb.AppendLine($"\tVarRawSet({newListName}, VarNewNumber({listIndexName}++), {valueStackName});");
            }else if(entry is EntryNamed en){
                string enBody = en.Value.GenerateInline(header, out string valueStackName);
                if(!String.IsNullOrEmpty(enBody)) {
                    sb.AppendLine($"\t// Named {i}");
                    sb.AppendLine(Tabbed(Tabbed(enBody)));
                }
                sb.AppendLine($"\tVarRawSet({newListName}, VarNewString(\"{en.Name}\"), {valueStackName});");
            }else if(entry is EntrySplat es){
                string esBody = es.Value.GenerateInline(header, out string valueStackName);
                if(!String.IsNullOrEmpty(esBody)) {
                    sb.AppendLine($"\t// Splat {i}");
                    sb.AppendLine(Tabbed(Tabbed(esBody)));
                }
                sb.AppendLine($"\tVarCopyListIntoOffset({valueStackName}, {newListName}, &{listIndexName});");
            }
            i++;
        }
        return sb.ToString();
    }
}

public abstract class ListEntry {
    public static (ListEntry? listEntry, int index) TryParse(List<Token> tokens, int index){
        // Try and get an EntryNamed
        var named = EntryNamed.TryParse(tokens, index);
        if(named.entryNamed != null) {
            return named;
        }

        // Try an EntrySplat
        var splat = EntrySplat.TryParse(tokens, index);
        if(splat.entrySplat != null) {
            return splat;
        }

        // Try an EntryExpression
        var expression = EntryExpression.TryParse(tokens, index);
        if(expression.entryExpression != null) {
            return expression;
        }

        return (null, index);
    }
}

public class EntryNamed : ListEntry {
    public string Name {get; set;}
    public Expression Value {get; set;}
    public EntryNamed(string name, Expression value) {
        Name = name;
        Value = value;
    }

    public static new (EntryNamed? entryNamed, int index) TryParse(List<Token> tokens, int index){
        int i = index;
        if(tokens[i].Type != TokenType.Identifier) {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;

        if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "=") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i++;

        var value = Expression.TryParseAny(tokens, i);
        if(value.expression == null) {
            return (null, index);
        }
        i = value.index;

        return (new EntryNamed(tokens[index].Value, value.expression), i);
    }
}

// name...
public class EntrySplat : ListEntry {
    public Expression Value {get; set;}
    public EntrySplat(Expression value) {
        Value = value;
    }

    public static new (EntrySplat? entrySplat, int index) TryParse(List<Token> tokens, int index){
        int i = index;
        // Get any expression
        var value = Expression.TryParseAny(tokens, index);
        if(value.expression == null) {
            return (null, index);
        }
        i = value.index;
        // Check for a ., then a ., then a .
        if(tokens[i].Type != TokenType.Punctuation || tokens[i].Value != "." || tokens[i+1].Type != TokenType.Punctuation || tokens[i+1].Value != "." || tokens[i+2].Type != TokenType.Punctuation || tokens[i+2].Value != ".") {
            return (null, index);
        }
        Parser.RegisterFurthest(i);
        i += 3;

        return (new EntrySplat(value.expression), i);
    }
}

// Just a regular expression
public class EntryExpression : ListEntry {
    public Expression Value {get; set;}
    public EntryExpression(Expression value) {
        Value = value;
    }

    public static new (EntryExpression? entryExpression, int index) TryParse(List<Token> tokens, int index){
        // Get any expression
        var value = Expression.TryParseAny(tokens, index);
        if(value.expression == null) {
            return (null, index);
        }
        return (new EntryExpression(value.expression), value.index);
    }
}