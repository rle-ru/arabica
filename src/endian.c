int reverse_endian_32(int n)
{
	int ret = 0;
	ret |= (n & 0xff) << 24;
	ret |= (n & 0xff00) << 8;
	ret |= (n & 0xff0000) >> 8;
	ret |= (n & 0xff000000) >> 24;
	return (ret);
}