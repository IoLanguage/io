
struct md5_context
{
	u_int32_t buf[4];
	u_int32_t bits[2];
	uint8_t in[64];
};
