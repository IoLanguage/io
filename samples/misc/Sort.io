
words := List clone

words append("video ")
words append("killed ")
words append("the ")
words append("radio ")
words append("star ")

writeln("original: ", words)

words = words sortBy(method(a, b, a < b))

writeln("sortBy:   ", words)
