# Globals

The following globals are available in Funky 3:

## print
**Description:**\
Writes a message to the console.

**Arguments:**
* any* **message**: The values to print.

**Return Value:**\
string **message**: The message that was printed.

**Special Notes:**\
The message will be printed with a newline appended.\
Arguements are separated by a tab.\
Arguments are implicitly converted to strings.

```javascript
print("Hello,", "World!")
```