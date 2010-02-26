
BigNum repeat := method(
	bignum := self >> 31
	remainder := (self % 2 ** 31) asNumber
	if(bignum != 0, bignum repeat(call delegateTo(bignum)))
	call delegateTo(remainder)
)
