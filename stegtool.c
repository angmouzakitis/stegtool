/*
Copyright (c) 2014, Angelos Mouzakitis (aka Gr33kR3volV3R)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

struct headers {
	uint32_t fullsize;
	uint32_t rawsize;
	uint8_t *offset;
	uint32_t height;
	uint32_t width;
	uint32_t compression;
} header;
int8_t filepath[260];

void save_file(uint8_t *);
void set_headers(uint8_t *);
void encrypt_bits(uint8_t *, int8_t[]);
void char2bit(int8_t);
void usage();
int8_t *makeXOR(int8_t*);
uint8_t *copy_to_mem(int8_t[]);
uint8_t *GetNextCharacter(uint8_t[],int8_t);
int8_t bit2char(int8_t[]);
int8_t decrypt_bits(int);
int power(int);

void usage()
{
	printf("Give some arguments\nUSAGE: stegtool.exe <--encrypt|--decrypt> <TextToCrypt>\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	uint8_t *ptr;
	int8_t *action, ch[10000], encryption[8]; //argument to hide
	int i = 0, j;
	int8_t *decryptptr;

	decryptptr = (int8_t *) malloc(sizeof(int8_t) * 10000);

	if (argc == 1) 
		usage();

	printf("What encryption method ? plain/xor\n");
	scanf("%s", encryption);

	ptr = copy_to_mem(argv[1]);
	set_headers(ptr);

	switch (argv[1][2]) {
	case 'e' : // encrypt
		printf("MAIN : Text to hide : %s\n\n", argv[2]);
		printf("______________________________________________________\n\n\n");
		switch (encryption[0]) {
		case 'x' : 
			argv[2] = makeXOR(argv[2]);
		case 'p' : 
			encrypt_bits(ptr, argv[2]);
			break;
		}
		save_file(ptr);
		break;
	case 'd' : // decrypt
		switch (encryption[0]) {
		case 'p' : //PLAIN ENCRYPTION
			for (i = 1; ch[i] = decrypt_bits(i); i++)
				printf("MAIN : To ch einai %d kai %c\n", ch[i], ch[i]);
			break;
		case 'x' : //XOR ENCRYPTION
			for (i = 1; ch[i] = decrypt_bits(i); i++)
				printf("MAIN : To ch einai %d kai %c\n", ch[i], ch[i]);
			decryptptr = &ch[1];
			decryptptr = makeXOR(decryptptr);
			break;

		default:
			printf("ERROR SWITCH DEFAULT , NO SUCH ENCRYPTION METHOD ... QUITING");
			return 0;
		}
		printf("\n\nHIDDEN MESSAGE FOUND : ");
		for (j = 1; j <= i; j++)
			printf("%c", ch[j]);
		printf("\n");
	}
	return 0;
}

int8_t *makeXOR(int8_t *buf) 
{	
	int i, j = 0;
	int8_t pass[20];

	printf("\nGive xor passshare: ");
		scanf("%s", pass);
	for (i = 0; buf[i]; i++) {	
		if(buf[i] == 2)
			buf[i] = 0;
		buf[i] ^= pass[j++]; 
		if (!(pass[j]))
			j = 0;
		if (buf[i] == 2)
			buf[i] = 0;
	}
	return buf;
}

void encrypt_bits(uint8_t *ptr, int8_t TextToCrypt[])
{
	int i = 0;
	uint8_t *bytes, poschar = 0;

	bytes = (uint8_t *) malloc(8);
	ptr = header.offset;
	while (TextToCrypt[poschar]) {
		//printf("encrypt_bits : TextToCrypt[%d] = %c : \n", poschar, TextToCrypt[poschar]);
		bytes = GetNextCharacter(bytes, TextToCrypt[poschar]);	
		poschar++;
		for (i = 0; i < 8; i++) {
			if (bytes[i] == 0)	
				ptr[i] &=0xFE;
			else			
				ptr[i] |=0x01;
		//	printf("encrypt_bits : Changed byte is %x at %p\n", *(ptr+i), ptr+i);
		}
		ptr += 8;//Go 8 bytes after
	}
	//SET THE NEXT 8 BYTES TO ZERO TO KNOW WHERE THE DECRYPT STOPS
	for (i = 0; i < 8; i++)
		ptr[i] &= 0xFE;
	printf("\nCHANGEBITS : Changed successfully %d bits\n", poschar);
}

int8_t decrypt_bits(int i)
{
	uint8_t *ptr, ret;

	ptr = header.offset;
	ptr += (i-1) * 8;
	ret = bit2char(ptr);
	return ret;
}

void char2bit(int8_t c)
{
	int8_t i, bytes[8];

	for (i = 7; i >= 0; i--) {
		bytes[i] = c%2;
		c = c >> 1;
	}
	putchar('\n');
	printf("einai %d\n", bit2char(bytes));
}

int8_t bit2char(int8_t *ptr)
{
	int i, y = 7;
	int8_t ret = 0;

	for (i = 0;i < 8; i++) {
		if (ptr[y] & 1)
			ret += power(i);
		y--;
	}
	return ret;
}

int power(int n)
{
	int i, p = 1;

	for (i = 0; i < n; i++) p *= 2;
	return p;
}

uint8_t *GetNextCharacter(uint8_t text[], int8_t ch)
{
	int i;

	printf("GETNEXTCHARACTER : Character %c ", ch);
	for (i = 7;i >= 0;i--) {
		text[i] = ch % 2;
		ch /= 2;
	}
	for (i = 0; i < 8; i++)
		printf("%d", text[i]);
	putchar('\n');
	return text;
}

uint8_t *copy_to_mem(int8_t *action)
{
	FILE *fp;
	long long i = 0;
	uint8_t *map;
	int8_t temp;

	map = (uint8_t *) malloc(32000000); //32MB

	if (!map) {
		printf("Cannot allocate memory for map\n");
		return 0;
	}
	printf("Enter absolute path filename of image to {en/de}crypt : ");
	scanf("%s", filepath);
	fp = fopen(filepath, "r+b");
	if (!fp) {
		perror("exiting ... ");
		exit(10);
	}
	for (i = 0; fscanf(fp, "%c", &map[i]) != EOF; i++);		
	header.fullsize = i;
	fclose(fp);
	printf("copy_to_mem : Size is %d bytes \n\n", header.fullsize);
	return map;
}

void set_headers(uint8_t *ptr)
{
	//header.fullsize Allready defined in copy_to_mem
	//header.compression = &ptr[0x1e];
	header.rawsize = header.fullsize - *ptr + 0x0a;
	header.height = ptr[0x16];
	header.width = ptr[0x12];
	header.offset = &ptr[ptr[0x0a]];
	//printf("set_headers : Rawsize is %d bytes\n", header.rawsize);
	//printf("set_headers : Fullsize is %d bytes\n", header.fullsize);
	//printf("set_headers : Image heitght is %d pixels\n", header.height);
	//printf("set_headers : Image width is %d pixels\n", header.width);
	//printf("set_headers : The header offset points to %p with content of : %x \n\n", header.offset, *(header.offset));
}

void save_file(uint8_t *ptr)
{
	FILE *fp;

	fp = fopen( "steg.bmp", "w+b" );
	fwrite(ptr, 1, header.fullsize, fp);
	fclose(fp);
	printf("\nFile steg.bmp successfully generated !\n");
}
