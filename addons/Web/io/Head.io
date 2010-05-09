Head := Element clone do(
    tag := "head"
    
    title := method(value,
        children append(Title clone with(value))
        return self
    )
   
)
