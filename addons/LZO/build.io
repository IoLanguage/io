AddonBuilder clone do(
    if(optionallyDependsOnLib("lzo2"),
        addDefine("USE_LIBLZO")
    ,
        addDefine("USE_MINILZO")
    )
)
