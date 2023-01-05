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
    
    var c_cppHighlightRules = require("./c_cpp_highlight_rules").c_cppHighlightRules;
    
    var Funky3HighlightRules = function() {
    
        var keywords = (
            "for|if|else|function|with|using|while|for|in|"+
            "var|local|"+
            "return|break|"+
            "and|or|not|"
        );
    
        var builtinConstants = ("nil|undefined|_G");
    
        var functions = (
          // builtinFunctions
            "pairs|values|print|write|rawGet|rawSet|toList|"+
            "list|string|math|draw|os|meta|"+
            "where|map|fold|reduce|cumulate|reverse|insert|push|enqueue|remove|pop|dequeue|clone|sort|sum|product|max|min|"+
            "abs|round|floor|ceil|min|max|clamp|sqrt|rad|deg|random|randomSeed|pi|e|huge|inf|nan|epsilon|sin|cos|tan|asin|acos|atan|"+
            "clock|"+
            "match|matches|replace|find|sub|reverse|upper|lower|"+
            "box|boxOutline|cameraToScreen|circle|circleOutline|clear|getColor|getFont|getLineWidth|pop|print|printOutline|push|pushCamera|pushViewport|rotate|scale|screenHeight|screenSize|screenToCamera|screenToViewport|screenWidth|setColor|setFont|setLineWidth|translate|viewportToScreen|"+
            "beginPath|clear|moveTo|lineTo|stroke|fill|strokeStyle|fillStyle|strokeText|fillText|font|textAlign|textBaseline|fillRect|strokeRect|arc|arcTo|bezierCurveTo|quadraticCurveTo|rect|closePath|clip|isPointInPath|isPointInStroke|scale|rotate|translate|transform|setTransform|resetTransform|drawImage|drawImage2|save|restore|reset|roundRect|measureText|lineWidth|"+
            "base|function|number|string|list|null|methods|"+
          // metamethods
            "add|sub|mul|div|intdiv|mod|pow|band|bor|bnot|bshl|bshr|lt|gt|le|ge|eq|ne|concat|unp|unm|not|bnot|len|iter|tostring|truthy|get|set|call|len|dispose"
        );
    
        var stdLibaries = ("list|string|math|draw|os");
        
        var preprocs = "define|undef|ifdef|ifndef|elif|endif"
    
        var keywordMapper = this.createKeywordMapper({
            "keyword": keywords,
            "support.function": functions,
            "constant.library": stdLibaries,
            "constant.language": builtinConstants,
            "variable.language": "self"
        }, "identifier");
    
        this.$rules = {
            "start" : [
            {
              token: "keyword.preproc",
              regex: /\$cInlineStart|\$cHeaderStart/,
              next: "c-start"
            },
            { 
                token: "keyword.preproc",
                regex: /\$define\//,
                next: "regex"
            },
            {
                token: "keyword.preproc",
                regex: `\\\$(${preprocs})`,
            },
            { 
                token: "comment.multiline",
                regex: /\$\*/,
                next:"comment"
            },
            {
                token: "comment",
                regex: /\$\$.*$/
            },
            {
                token : "invalid",
                regex : "\\$.*$"
            },
            {
                token: "string.interpolated",
                regex: /`/,
                next: "interpolated"
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
            },
            {
                token: "paren.lparen",
                regex: /\[/,
                onMatch: function(value, currentState, stack){
                   if(stack.length > 0){
                       stack[0]++;
                   }
                   return "paren.lparen";
               }
            },
            {
                token: "paren.rparen",
                regex: /\]/,
                next: "start",
                onMatch: function(value, currentState, stack){
                    if(stack.length > 0){
                        if(stack[0] <= 0){
                            this.next = "interpolated";
                            stack.shift();
                            return "string.interpolated";
                        }
                        stack[0]--;
                    }
                    this.next = "start";
                    return "paren.rparen";
               }
            },
            {
                token : "paren.lparen",
                regex : "[\\(\\{]"
            }, {
                token : "paren.rparen",
                regex : "[\\)\\}]"
            }, {
                token : "text",
                regex : "\\s+|\\w+"
            } ],
            "regex": [
                {
                    // escapes
                    token: "regexp.keyword.operator",
                    regex: "\\\\(?:u[\\da-fA-F]{4}|x[\\da-fA-F]{2}|.)"
                }, {
                    // flag
                    token: "string.regexp",
                    regex: "/",
                    next: "regex_end"
                }, {
                    // invalid operators
                    token : "invalid",
                    regex: /\{\d+\b,?\d*\}[+*]|[+*$^?][+*]|[$^][?]|\?{3,}/
                }, {
                    // operators
                    token : "constant.language.escape",
                    regex: /\(\?[:=!]|\)|\{\d+\b,?\d*\}|[+*]\?|[()$^+*?.]/
                }, {
                    token : "constant.language.delimiter",
                    regex: /\|/
                }, {
                    token: "constant.language.escape",
                    regex: /\[\^?/,
                    next: "regex_character_class"
                }, {
                    token: "empty",
                    regex: "$",
                    next: "regex_end"
                }, {
                    defaultToken: "string.regexp"
                }
            ],
            "regex_character_class": [
                {
                    token: "regexp.charclass.keyword.operator",
                    regex: "\\\\(?:u[\\da-fA-F]{4}|x[\\da-fA-F]{2}|.)"
                }, {
                    token: "constant.language.escape",
                    regex: "]",
                    next: "regex"
                }, {
                    token: "constant.language.escape",
                    regex: "-"
                }, {
                    token: "empty",
                    regex: "$",
                    next: "regex_end"
                }, {
                    defaultToken: "string.regexp.characterclass"
                }
            ],
            "regex_end": [
                {
                    token: "string.substitute",
                    regex: /$/,
                    next: "start"
                },
                {defaultToken: "string.substitute"}
            ],
            "comment": [
                {
                    token: "comment.multiline",
                    regex: /\*\$/,
                    next: "start"
                },
                {
                    defaultToken: "comment.multiline"
                }
            ],
            "interpolated": [
                {
                    token: "string.interpolated",
                    regex: /`/,
                    next: "start"
                },
                {
                   token: "string.interpolated",
                   regex: /\\./
                },
                {
                   token: "string.interpolated",
                   regex: /\[/,
                   next: "start",
                   onMatch: function(value, currentState, stack){
                       stack.unshift(0);
                       return "string.interpolated";
                   }
                },
                {
                    defaultToken: "string.interpolated"
                }]
        };
        this.embedRules(c_cppHighlightRules, "c-",
            [{
                token: "keyword.preproc",
                regex: /\$cInlineEnd|\$cHeaderEnd/,
                next: "start"
            }]);
        this.normalizeRules();
    };
    
    oop.inherits(Funky3HighlightRules, TextHighlightRules);
    
    exports.Funky3HighlightRules = Funky3HighlightRules;
});