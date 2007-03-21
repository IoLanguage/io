IPAddress do(
    asSimpleString := method(
        ip .. ":" .. port
    )

    asString := getSlot("asSimpleString")

    slotDescriptionMap := method(
         resend atPut("ip", ip) atPut("port", port asString)
    )
)
