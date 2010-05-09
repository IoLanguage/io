Anchor := Element clone do(
    tag := "a"
    
    action := method(value,
        attributes atPut("href", value)
        return self
    )
)
