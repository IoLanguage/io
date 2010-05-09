ScreenHandler := Handler clone do(
    clone := method(
        return self
    )

    handle := method(msg,
        msg println
    )
)
