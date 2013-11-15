#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct headers{
	unsigned int fullsize;
	unsigned int rawsize;
	unsigned char *offset;
	unsigned int height;
	unsigned int width;
	unsigned int compression;
}header;
unsigned char* GetNextCharacter(unsigned char[],char);
char* makeXOR(char*);
void EncryptBits(unsigned char*,char[]);
char DecryptBits(int);
unsigned char* CopyToMemory(char[]);
void CopyToFile(unsigned char*);
void SetHeaders(unsigned char*);
int power(int);
char AssembleBitsToChar(char[]);
void CharToBits(char);

int main(int argc, char **argv)
{
	unsigned char *ptr;
	char *TextToCrypt,*action,ch[10000],encryption[10]; //argument to hide
	int i=0,j;
	char *decryptptr;
	decryptptr = (char*) malloc(sizeof(char) * 10000);

	if(argc==1)
	{
		printf("Give some arguments\nUSAGE: stegtool.exe <--encrypt|--decrypt> <TextToCrypt>\n");
		system("PAUSE");return 0;
	}
	TextToCrypt=argv[2];
	/*fp = fopen("C:/file","r");
	for(i=0;fscanf(fp,"%c", TextToCrypt+i) != EOF; i++);
	fclose(fp);
	printf("%s",TextToCrypt);*/
	action = argv[1];
	ptr=CopyToMemory(action);
	printf("\nWhat encryption method ? plain/xor\n");
	gets_s(encryption,6);
	if(ptr==0)
	{
		putchar('\n');system("PAUSE");return 0;
	}
	SetHeaders(ptr);
	//printf("MAIN : 8 bits of offset ");
	//for(i=0;i<8;i++)
	//	printf("%x ",*(header.offset+i));

	//////////ENCRYPT CONTENT INTO IMAGE////////////////
	if(argv[1][2]=='e') /// Not check whole string ... checked into CopyToMemory
	{
		printf("MAIN : Text to hide : %s\n\n",TextToCrypt);
		printf("______________________________________________________\n\n\n");
		switch (encryption[0])
		{
		case 'p' : 
			EncryptBits(ptr,TextToCrypt);
			break;
		case 'x' : 
			
			TextToCrypt = makeXOR(TextToCrypt);
			EncryptBits(ptr,TextToCrypt);
			break;

		default:
			printf("ERROR SWITCH DEFAULT , NO SUCH ENCRYPTION METHOD ... QUITING");
			return 0;
		}
		CopyToFile(ptr);
	}
	//////////END OF ENCRYPT CONTENT///////////////////

	/////////DECRYPT BITS SHOW THE HIDDEN MSG !!! /////
	else
	{
		switch (encryption[0])
		{
			case 'p' : //PLAIN ENCRYPTION
				for(i=1;(ch[i] = DecryptBits(i)) != 0 ;i++)
					printf("MAIN : To ch einai %d kai %c\n",ch[i],ch[i]);
			break;
			case 'x' : //XOR ENCRYPTION
				for(i=1;(ch[i] = DecryptBits(i)) != 0 ;i++)
					printf("MAIN : To ch einai %d kai %c\n",ch[i],ch[i]);
				decryptptr = &ch[1];
				decryptptr = makeXOR(decryptptr);
			break;

			default:
				printf("ERROR SWITCH DEFAULT , NO SUCH ENCRYPTION METHOD ... QUITING");
				return 0;
		}
		printf("\n\nHIDDEN MESSAGE FOUND : ");
				for(j=1;j<=i;j++)
					printf("%c",ch[j]);
	}
	/////////////////END OF DECRYPT////////////////////
putchar('\n');system("PAUSE");
return 0;
}
char* makeXOR(char*TextToCrypt)
{	
	int i,passcount=0;
	char *passshare;
	passshare = (char *) malloc(sizeof(char) * 20);
	printf("\nGive xor passshare: ");
		scanf("%s",passshare);
	for(i=0;*(TextToCrypt+i)!=0;i++)
	{
		if(*(TextToCrypt+i)==2)
			*(TextToCrypt+i)=0x0;
		*(TextToCrypt+i) = *(TextToCrypt+i) ^ (*(passshare+passcount++));
		if(*(passshare+passcount)=='\x0')
			passcount=0;
		if(*(TextToCrypt+i)==0)
			*(TextToCrypt+i)|=0x2;
	}
	return TextToCrypt;
}
void EncryptBits(unsigned char*ptr,char TextToCrypt[])
{
	int i=0;
	unsigned char *bytes,poschar=0;
	bytes = (unsigned char*) malloc(8);
	ptr = header.offset;
	while (*(TextToCrypt+poschar)!=0)
	{
		//printf("ENCRYPTBITS : TextToCrypt[%d] = %c : \n",poschar,TextToCrypt[poschar]);
		bytes=GetNextCharacter(bytes,TextToCrypt[poschar]);	
		poschar++;
		for(i=0;i<8;i++)
		{
			if(bytes[i]==0)	
				*(ptr+i)&=0xFE;
			else			
				*(ptr+i)|=0x01;
		//	printf("ENCRYPTBITS : Changed byte is %x at %p\n",*(ptr+i),ptr+i);
		}
		ptr+=8;//Go 8 bytes after
		//printf("\n");
	}
	//SET THE NEXT 8 BYTES TO ZERO(0) TO KNOW WHERE THE DECRYPT STOPS
	for(i=0;i<8;i++)
		*(ptr+i) &= 0xFE;
	printf("\nCHANGEBITS : Changed successfully %d bits\n",poschar);
}
char DecryptBits(int countofbytes)
{
	//int i;
	unsigned char *ptr,newchar;
	ptr = header.offset;
	//Export 8 bits to an ASCII character//
	ptr+=(countofbytes-1)*8;
	//printf("\nDECRYPTBITS : 8 bits of *ptr ");
	//for(i=0;i<8;i++)
		//printf("%x ",*(ptr+i));
	newchar = AssembleBitsToChar(ptr);
	//printf("DECRYPTBITS new char %d and character %c\n",newchar,newchar);
	
	return newchar;
}
void CharToBits(char sequence)
{
	char i,bytes[8];
	for(i=7;i>=0;i--)
	{
		bytes[i]=sequence%2;
		sequence/=2;
	}
	putchar('\n');
	printf("einai %d\n",AssembleBitsToChar(bytes));
}
char AssembleBitsToChar(char bytes[])
{
	int i,y=7;
	char newchar=0;
	for(i=0;i<8;i++)
	{
		if((bytes[y] & 1) == 0x01)
			newchar += power(i);
		y--;
	}
	return(newchar);
}
int power(int n)
{
	int i,powered=1;
	for(i=0;i<n;i++)
		powered*=2;
	return powered;
}
unsigned char* GetNextCharacter(unsigned char bytes[],char poschar)
{
	int i;
	printf("GETNEXTCHARACTER : Character %c ",poschar);
	for(i=7;i>=0;i--)
	{
		bytes[i]=poschar%2;
		poschar/=2;
	}
	for(i=0;i<8;i++)
		printf("%d",bytes[i]);
	putchar('\n');
	return(bytes);
}
unsigned char* CopyToMemory(char argv[])
{
	FILE *text;
	long long i=0;
	static  char *filepointer;
	char openfile[50],temp;
	filepointer = ( char *) malloc(16000000); //16MB
	if(filepointer==NULL)
	{
		printf("Cannot allocate memory for filepointer\n");
		return 0;
	}
	if(strcmp("--encrypt",argv)==0)
	{
		printf("Enter absolute path filename of image to encrypt : ");
		gets(openfile);

		text = fopen(openfile,"r+b");
		if(text==0)
		{
			printf("Problem with the file ... QUITING ....\n");
			return 0;
		}
	}
	else 
	{
		if(strcmp("--decrypt",argv)==0)
		{
			text = fopen("C:/steg.bmp","r+b");
		}
		else
		{
			printf("Kapia lathos exeis kanei stin synta3i ...\nUSAGE: stegtool.exe <--encrypt|--decrypt> <TextToCrypt> ");
			return 0;
		}
	}
	for(i=0;fscanf(text,"%c", &filepointer[i]) != EOF; i++);		
	header.fullsize = i;
	printf("COPYTOMEMORY : Full size is %d bytes \n\n",header.fullsize);
	fclose(text);
return filepointer;
}
void SetHeaders(unsigned char*ptr)
{
	//header.fullsize Allready defined in CopyToMemory
	//header.compression=&ptr[0x1e];
	header.rawsize = header.fullsize - *ptr+0x0a;
	header.height=ptr[0x16];
	header.width=ptr[0x12];
	header.offset = &ptr[ptr[0x0a]];
	//printf("SETHEADERS : Rawsize is %d bytes\n",header.rawsize);
	//printf("SETHEADERS : Fullsize is %d bytes\n",header.fullsize);
	//printf("SETHEADERS : Image heitght is %d pixels\n",header.height);
	//printf("SETHEADERS : Image width is %d pixels\n",header.width);
	//printf("SETHEADERS : The header offset points to %p with content of : %x \n\n",header.offset,*(header.offset));
}
void CopyToFile(unsigned char*ptr)
{
	FILE *stegtext;
	stegtext = fopen( "C:/steg.bmp", "w+b" );
	fwrite(ptr, 1, header.fullsize , stegtext);
	fclose(stegtext);
	printf("\nFile steg.bmp successfully generated !\n");
}
