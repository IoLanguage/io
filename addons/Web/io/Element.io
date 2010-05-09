Element := Object clone do(
    tag := ""

    init := method(
        self attributes := Map clone
        self children := list clone
    )

    with := method(
        s := self clone
        call message arguments foreach(arg,
            s children append(call sender doMessage(arg))
        )
        return s
    )

    id := method(value,
        attributes atPut("id", value)
        return self
    )

    class := method(value,
        attributes atPut("class", value)
        return self
    )

    style := method(value,
        attributes atPut("style", value)
        return self
    )
   
    render := method(out,
        attrString := ""
        attributes foreach(key, value,
            attrString = attrString .. (" #{key}=\"#{value}\"" interpolate)
        )
        if(children size > 0) then(
            out = out .. ("<#{tag}#{attrString}>" interpolate)
            children foreach(i, child,
                e := try(out = child render(out))
                e catch(Exception, out := out .. child)
            )
            out = out .. "</#{tag}>" interpolate
        ) else(
            out = out .. "<#{tag} #{attrString}/>" interpolate
        )
        return out
    )
)
