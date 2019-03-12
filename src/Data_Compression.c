/*
 ============================================================================
 Name        : TESTC.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#define length(x) ((int)log10(x)+1)

/* Node of the huffman tree */
struct node{
    int value;
    char letter;
    struct node *left,*right;
};

typedef struct node Node;

int count_total_characters(FILE *input);
int Small(Node *array[], int another);
void count_characters(int a[],FILE *input);
void HuffmanTree(Node **tree,FILE *input,int count[]);
void Record_codes(int BitTable[],int codeTable[],int codeTable2[],Node *tree,int Code,int Code2,int bit);
void invertCodes(int codeTable[],int InvertcodeTable[]);
void Compress(FILE *input, FILE *output, int codeTable[],int codeTable2[],int BitTable[]);
void Decompress(FILE *input,FILE *output,Node *tree,int End);
void print_code(Node *tree,int arr[],int count[],int top);

int main()
{
    FILE *input, *output, *input1;
    Node *tree;
    int BitTable[123],codeTable[123],codeTable2[123],IcodeTable[123],IcodeTable2[123],count[256]={0},array[100];
    int compress;
    int i,total;
    char filename[20];
    printf("%d",length(1000));

    input = fopen("input.txt", "rb");

    total=count_total_characters(input);

    HuffmanTree(&tree,input,count);

    Record_codes(BitTable,codeTable,codeTable2,tree,0,0,0);

    print_code(tree,array,count,0);

    invertCodes(codeTable,IcodeTable);
    invertCodes(codeTable2,IcodeTable2);

    printf("Type 1 to compress, 2 to decompress, other keys to exit:");
    scanf("%d",&compress);

    for(;compress==1||compress==2;){
        if (compress==1){
            output = fopen("output.txt","wb");
            Compress(input,output,IcodeTable,IcodeTable2,BitTable);
            fclose(output);
        }
        else{
            output = fopen("output.txt","rb");
            input1= fopen("input1.txt", "wb");
            Decompress (output,input1,tree,total);
            fclose(output);
            fclose(input1);
        }
        printf("Type 1 to compress, 2 to decompress, other keys to exit:");
        scanf("%d",&compress);
    }

    fclose(input);

    return 0;
}

/*Count total characters*/
int count_total_characters(FILE *input){
    int i=0;
    char c;
    while ((c=fgetc(input))!=EOF)
        i++;

    return i;
}

/*Return the small subtree*/
int Small(Node *array[], int another){
    int small;
    int i = 0;

    while (array[i]->value==-1)
        i++;
        small=i;
    if (i==another){
        i++;
        while (array[i]->value==-1)
            i++;
        small=i;
    }

    for (i=1;i<91;i++){
        if (array[i]->value==-1)
            continue;
        if (i==another)
            continue;
        if (array[i]->value<array[small]->value)
            small = i;
    }

    return small;
}

/*Count the number of each characters*/
void count_characters(int a[],FILE *input) {

    int chk,temp;
    int i;

    if ( NULL == input ){
        printf( "Open failure" );
        return;
    }
    else{
        while( feof(input) == 0 )
        {
            temp=fgetc(input);
            for(chk=32;chk<123;chk++)
                if (temp==chk)
                    a[chk]++;
        }
        for(i=32;i<123;i++)
        printf("%c:%d\n",i,a[i]);
       }

}

/*build the huffman tree*/
void HuffmanTree(Node **tree,FILE *input,int count[]){
    Node *temp;
    Node *arr[91];
    int i, leaf = 91;
    int small_1,small_2;

    fseek(input,0,0);

    count_characters(count,input);

    for (i=0;i<91;i++){
        arr[i] = malloc(sizeof(Node));
        arr[i]->value = count[i+32];
        arr[i]->letter = i+32;
        arr[i]->left = NULL;
        arr[i]->right = NULL;
    }


    while (leaf>1){
        small_1=Small(arr,-1);
        small_2=Small(arr,small_1);
        temp = arr[small_1];
        arr[small_1] = malloc(sizeof(Node));
        arr[small_1]->value=temp->value+arr[small_2]->value;
        arr[small_1]->letter=127;
        arr[small_1]->left=arr[small_2];
        arr[small_1]->right=temp;
        arr[small_2]->value=-1;
        temp->value=-1;
        leaf--;
    }


    *tree = arr[small_1];

return;
}

/*make a table to record the codes of each characters*/
 void Record_codes(int BitTable[],int codeTable[],int codeTable2[],Node *tree,int Code,int Code2,int bit){

    if (tree->letter<123){
        BitTable[(int)tree->letter]=bit;
        codeTable[(int)tree->letter] = Code;
        codeTable2[(int)tree->letter]=Code2;
        }
    else{
        if(bit<9){
          bit++;
          Record_codes(BitTable,codeTable,codeTable2, tree->left, Code*10+1,Code2,bit);
          Record_codes(BitTable,codeTable,codeTable2, tree->right, Code*10+2,Code2,bit);
        }
        else if(bit>=9&&bit<=15){
          bit++;
          Record_codes(BitTable,codeTable,codeTable2, tree->left, Code,Code2*10+1,bit);
          Record_codes(BitTable,codeTable,codeTable2, tree->right, Code,Code2*10+2,bit);
        }
        else{
          bit++;
          Record_codes(BitTable,codeTable,codeTable2, tree->left, Code,Code2,bit);
          Record_codes(BitTable,codeTable,codeTable2, tree->right, Code,Code2,bit);
        }
    }
    return;
}

/*invert the codes so that they can be used in compression*/
void invertCodes(int codeTable[],int InvertcodeTable[]){
    int i, n, temp;

    for (i=32;i<123;i++){
        n = codeTable[i];
        temp = 0;
        while (n>0){
            temp = temp * 10 + n %10;
            n /= 10;
        }
        InvertcodeTable[i]=temp;
    }

return;
}

/*function to compress the input*/
void Compress(FILE *input, FILE *output, int codeTable[],int codeTable2[],int BitTable[])
{
    char bit, c, x = 0;
    int n,n1,n2,length,bitsLeft = 8;

    fseek(input,0,0);

    while ((c=fgetc(input))!=EOF){

        if (BitTable[c]<10){
            length = length(codeTable[c]);
            n = codeTable[c];
        }
        else{
            length=length(codeTable[c])+length(codeTable2[c]);
            if (length>=16)
                length=16;
            n1 = codeTable[c];
            n2 = codeTable2[c];
        }

        while (length>0){

            if(BitTable[c]<10){
                bit = n % 10 - 1;
                n /= 10;
            }
            else{
                if(n1>0){
                    bit=n1%10-1;
                    n1/=10;
                }
                else{
                    bit=n2%10-1;
                    n2/=10;
                }
            }

            x = x | bit;
            bitsLeft--;
            length--;
            if (bitsLeft==0){
                fputc(x,output);
                x = 0;
                bitsLeft = 8;
            }
            x = x << 1;
        }
    }

    if (bitsLeft!=8){
        x = x << (bitsLeft-1);
        fputc(x,output);
    }

    return;

}

/*function to decompress the input*/
void Decompress(FILE *input,FILE *output,Node *tree,int End){
    Node *current=tree;
    char c,bit;
    char mask=1<<7;
    int i,count=0;

    fseek(input,0,0);
    while(count<End){
        c=fgetc(input);
        for(i=0;i<8;i++){
            bit=c&mask;
            c=c<<1;
            if(bit==0){
                current=current->left;
                if(current->letter!=127){
                    fputc(current->letter,output);
                    current=tree;
                    count++;
                }
            }
            else{
                current=current->right;
                if(current->letter!=127){
                    fputc(current->letter,output);
                    current=tree;
                    count++;
                }
            }
        }
    }
    return;
}

/*print the code processed from Huffman tree*/
void print_code(Node *tree,int arr[],int count[],int top){

    int i;
    if(tree->left)
    {
        arr[top]=0;
        print_code(tree->left,arr,count,top+1);
    }
    if(tree->right)
    {
        arr[top]=1;
        print_code(tree->right,arr,count,top+1);
    }
    if((tree->left==NULL)&&(tree->right==NULL)){
        printf("%c(%d)",tree->letter,count[(int) tree->letter]);
        for(i=0;i<top;i++)
            printf("%d",arr[i]);
        printf("\n");
    }
}



