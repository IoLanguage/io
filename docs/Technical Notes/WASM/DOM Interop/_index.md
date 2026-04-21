# DOM Interop

Io code running in the browser can query, create, and manipulate HTML elements through the `DOM` object and `Element` instances.

## Quick Start

```io
// Get the page body
body := DOM body
body tagName println   // ==> BODY

// Create and attach an element
div := DOM createElement("div")
div setTextContent("Hello from Io!")
div setStyle("color", "blue")
div addClassName("greeting")
body appendChild(div)

// Query existing elements
el := DOM getElementById("myId")
el setAttribute("data-count", "42")
el getAttribute("data-count") println  // ==> 42

// Chaining (setters return self)
DOM createElement("p") setTextContent("chained") setStyle("font-weight", "bold")
```

## DOM Object

The `DOM` singleton provides document-level queries and element creation.

| Method | Returns | Description |
|--------|---------|-------------|
| `querySelector(selector)` | Element or nil | First element matching CSS selector |
| `querySelectorAll(selector)` | List of Elements | All elements matching CSS selector |
| `getElementById(id)` | Element or nil | Element with given id attribute |
| `createElement(tagName)` | Element | Create a new detached element |
| `body` | Element | The document body |

```io
DOM querySelector("h1") textContent println
DOM querySelectorAll("li") foreach(el, el textContent println)
DOM getElementById("app") setInnerHTML("<p>replaced</p>")
```

## Element Object

Element wraps a DOM element handle. Methods fall into three categories: content, attributes, and tree manipulation.

### Content

| Method | Returns | Description |
|--------|---------|-------------|
| `tagName` | Sequence | Tag name (e.g. "DIV", "SPAN") |
| `textContent` | Sequence | Text content of element and descendants |
| `setTextContent(text)` | self | Set text content |
| `innerHTML` | Sequence | HTML markup of children |
| `setInnerHTML(html)` | self | Set inner HTML |

### Attributes & Style

| Method | Returns | Description |
|--------|---------|-------------|
| `getAttribute(name)` | Sequence or nil | Get attribute value |
| `setAttribute(name, value)` | self | Set attribute |
| `removeAttribute(name)` | self | Remove attribute |
| `getStyle(property)` | Sequence | Get inline style property |
| `setStyle(property, value)` | self | Set inline style property |
| `addClassName(name)` | self | Add CSS class |
| `removeClassName(name)` | self | Remove CSS class |
| `hasClassName(name)` | true/false | Check for CSS class |

```io
el := DOM createElement("div")
el setAttribute("id", "demo")
el setStyle("background-color", "#eee")
el addClassName("card")
el hasClassName("card") println  // ==> true
```

### Tree Manipulation

| Method | Returns | Description |
|--------|---------|-------------|
| `appendChild(child)` | self | Append a child element |
| `remove` | self | Remove element from its parent |
| `children` | List of Elements | Direct child elements |

```io
list := DOM createElement("ul")
3 repeat(i,
    li := DOM createElement("li")
    li setTextContent("Item " .. (i + 1) asString)
    list appendChild(li)
)
DOM body appendChild(list)
list children size println  // ==> 3
```

## Architecture

### Handle Table

JS maintains a `Map<int, Element>`. WASM C code only sees integer handles (ints stored as the Element object's data pointer). Handle 0 means null/not found and maps to `nil` in Io.

```
Io code           C (WASM)              JS
─────────        ──────────           ──────
DOM body    →    dom_getBody()    →   registerHandle(document.body) → 7
el tagName  →    dom_getTagName(7) →  handles.get(7).tagName → "BODY"
```

### String Passing

- **C→JS**: `(pointer, length)` pairs. JS reads UTF-8 from WASM linear memory.
- **JS→C**: JS writes into a C-exported 64KB buffer (`dom_buf`), null-terminates, returns length. C reads the buffer as a C string.

### GC Integration

When the Io garbage collector frees an Element object, the Element's `freeFunc` calls `dom_release(handle)`, which removes the entry from the JS handle map. This prevents the JS-side Map from growing without bound.

## Limitations

- **Max 256 results** from `querySelectorAll`, `children` (fixed-size buffer on stack)
- **Max 64KB** per string transfer (shared buffer size)
- **No events** — event listeners are deferred to a future phase
- **No fetch/async** — network requests are deferred to a future phase
- **Inline styles only** — `getStyle`/`setStyle` operate on `element.style`, not computed styles
