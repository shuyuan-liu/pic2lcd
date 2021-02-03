unsigned char reverse_byte (unsigned char a)
{
    a = (a & 0b11110000) >> 4 | (a & 0b00001111) << 4;
    a = (a & 0b11001100) >> 2 | (a & 0b00110011) << 2;
    a = (a & 0b10101010) >> 1 | (a & 0b01010101) << 1;
    return a;
}