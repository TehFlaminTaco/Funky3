define('ace/mode/funky3', function(require, exports, module) {

var oop = require("ace/lib/oop");
var TextMode = require("ace/mode/text").Mode;
var Funky3HighlightRules = require("ace/mode/funky3_highlight_rules").Funky3HighlightRules;

var Mode = function() {
    this.HighlightRules = Funky3HighlightRules;
};
oop.inherits(Mode, TextMode);

(function() {
    // Extra logic goes here. (see below)
}).call(Mode.prototype);

exports.Mode = Mode;
});

define('ace/mode/funky3_highlight_rules', function(require, exports, module) {

var oop = require("ace/lib/oop");
var TextHighlightRules = require("ace/mode/text_highlight_rules").TextHighlightRules;

var Funky3HighlightRules = function() {

    var keywords = (
        "for|if|else|function|with|while|for|in|"+
        "var|local|"+
        "return|break|"+
        "and|or|not|"
    );

    var builtinConstants = ("nil|undefined|_G");

    var functions = (
      // builtinFunctions
        "pairs|values|print|write|getMeta|setMeta|rawGet|rawSet|toList|"+
        "list|string|math|draw|os|"+
        "where|map|fold|reduce|cumulate|reverse|insert|push|enqueue|remove|pop|dequeue|clone|sort|sum|product|max|min|"+
        "abs|round|floor|ceil|min|max|clamp|sqrt|rad|deg|random|randomSeed|pi|e|huge|inf|nan|epsilon|sin|cos|tan|asin|acos|atan|"+
        "clock|"+
        "match|matches|replace|find|sub|reverse|upper|lower|"+
        "beginPath|clear|moveTo|lineTo|stroke|fill|strokeStyle|fillStyle|frame"
    );

    var stdLibaries = ("list|string|math|draw|os");

    var keywordMapper = this.createKeywordMapper({
        "keyword": keywords,
        "support.function": functions,
        "constant.library": stdLibaries,
        "constant.language": builtinConstants,
        "variable.language": "self"
    }, "identifier");

    this.$rules = {
        "start" : [{
            stateName: "bracketedComment",
            onMatch : function(value, currentState, stack){
                stack.unshift(this.next, value.length - 2, currentState);
                return "comment";
            },
            regex : /\-\-\[=*\[/,
            next  : [
                {
                    onMatch : function(value, currentState, stack) {
                        if (value.length == stack[1]) {
                            stack.shift();
                            stack.shift();
                            this.next = stack.shift();
                        } else {
                            this.next = "";
                        }
                        return "comment";
                    },
                    regex : /\]=*\]/,
                    next  : "start"
                }, {
                    defaultToken : "comment"
                }
            ]
        },

        {
            token : "comment",
            regex : "\\$.*$"
        },
        {
            token : "string",           // " string
            regex : '"(?:[^\\\\]|\\\\.)*?"'
        }, {
            token : "string",           // ' string
            regex : "'(?:[^\\\\]|\\\\.)*?'"
        }, {
            token : "constant.numeric", // float
            regex : "((0(?:x([0-9A-Fa-f]+)|b([01]+)))|(?:((\\d*)\\.(\\d+))|(\\d+))(?:e(-?\\d+))?)"
        }, {
            token : keywordMapper,
            regex : "[a-zA-Z_$][a-zA-Z0-9_$]*\\b"
        }, {
            token : "keyword.operator",
            regex : "\\+|\\-|\\*|\\/|%|\\#|\\^|~|<|>|<=|=>|==|~=|=|\\:|\\.\\.\\.|\\.\\.|\\!|@"
        }, {
            token : "paren.lparen",
            regex : "[\\[\\(\\{]"
        }, {
            token : "paren.rparen",
            regex : "[\\]\\)\\}]"
        }, {
            token : "text",
            regex : "\\s+|\\w+"
        } ]
    };
    
    this.normalizeRules();
};

oop.inherits(Funky3HighlightRules, TextHighlightRules);

exports.Funky3HighlightRules = Funky3HighlightRules;
});