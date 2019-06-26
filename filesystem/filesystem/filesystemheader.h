#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>

struct file
{
	char *filename;
	unsigned pointer;
	int size;
	struct file *next;
};

void debug(struct file* filehead)
{
	struct file *start = filehead;
	while (start)
	{
		printf("%s %u %d", start->filename,start->pointer,start->size);
		start = start->next;
	}
}

void format_the_disk(int *no_of_files)
{
	*no_of_files = 0;
	FILE *fstream = fopen("harddisk.hdd", "rb+");
	if (fstream)
	{
		if (!fwrite(no_of_files, 4, 1, fstream))
			printf("FILE NOT WRITTEN\n");
		fclose(fstream);
	}
	else
	{
		printf("FILE NOT OPENED\n");
	}
}

void list_all_the_files(struct file* filehead)
{
	struct file *start = filehead;
	while (start)
	{
		printf("%s\n", start->filename);
		start = start->next;
	}
}

struct file* get_seekpoint_table_from_disk(int no_of_files,FILE *fstream,unsigned *seekpoint)
{
	struct file *start = NULL;
	struct file *last = NULL;
	unsigned point;
	int size;
	if (!no_of_files)
		return start;
	while (no_of_files--)
	{
		struct file *new_file = (struct file*)malloc(sizeof(struct file));
		new_file->filename = (char*)malloc(sizeof(char) * 16);
		if(!fread(new_file->filename, 1, 16, fstream))
			printf("FILE NOT READ\n");
		if(!fread(&point, 4, 1, fstream))
			printf("FILE NOT READ\n");
		if(!fread(&size, 4, 1, fstream))
			printf("FILE NOT READ\n");
		new_file->pointer = point;
		new_file->size = size;
		new_file->next = NULL;
		if (start == NULL)
			start = new_file;
		else
			last->next = new_file;
		last = new_file;
	}
	*seekpoint = last->pointer + last->size;
	return start;
}

void insert_seek_point_table_into_disk(struct file *filehead,int *no_of_files)
{
	struct file *temp = filehead;
	FILE *fstream = fopen("harddisk.hdd", "rb+");
	if (fstream)
	{
		fseek(fstream, 4, SEEK_SET);
		while (temp)
		{
			if (!fwrite(temp->filename, 1, 16, fstream))
				printf("FILE NOT WRITTEN\n");
			if (!fwrite(&temp->pointer, 4, 1, fstream))
				printf("FILE NOT WRITTEN\n");
			if (!fwrite(&temp->size, 4, 1, fstream))
				printf("FILE NOT WRITTEN\n");
			temp = temp->next;
		}
		fseek(fstream, 0, SEEK_SET);
		if (!fwrite(no_of_files, 4, 1, fstream))
			printf("FILE NOT WRITTEN\n");
		fclose(fstream);
	}
	else
	{
		printf("FILE NOT OPENED\n");
	}
}

struct file * update_seekpoint_table(struct file *filehead,int *no_of_files,char *filename,int size,unsigned pointer)
{
	struct file *temp = filehead;
	struct file *new_file = (struct file*)malloc(sizeof(struct file));
	new_file->filename = (char*)malloc(sizeof(char)*16);
	strcpy(new_file->filename,filename);
	new_file->size = size;
	new_file->pointer = pointer;
	new_file->next = NULL;
	*no_of_files+=1;
	if (temp == NULL)
		filehead = new_file;
	else
	{
		while (temp->next)
			temp = temp->next;
		temp->next = new_file;
	}
	insert_seek_point_table_into_disk(filehead,no_of_files);
	return filehead;
}

char ** divide_into_tokens(char *command,int *no_of_tokens)
{
	char **tokens = (char**)malloc(sizeof(char) * 6);
	for (int index = 0; index < 6; index++)
		tokens[index] = (char*)malloc(sizeof(char) * 100);
	int token_index = 0;
	int token_point = 0;
	for (int index = 0; command[index] != '\0'; index++)
	{
		if (command[index] == 32)
		{
			tokens[token_index][token_point] = '\0';
			token_index++;
			token_point = 0;
		}
		else
		{
			tokens[token_index][token_point++] = command[index];
		}
	}
	tokens[token_index][token_point] = '\0';
	*no_of_tokens = token_index + 1;
	return tokens;
}

struct file* copy_file_to_disk(struct file *filehead,char *source,char *destination,int *no_of_files,unsigned *seek_point)
{
	char ch;
	unsigned pointer;
	int size;
	char *buffer = (char*)malloc(sizeof(char) * 100000);
	FILE *fstream = fopen("harddisk.hdd","rb+");
	if (fstream)
	{
		FILE *source_stream = fopen(source, "rb+");
		if (source_stream)
		{
			fseek(source_stream, 0, SEEK_END);
			pointer=ftell(source_stream);
			size = pointer;
			fseek(source_stream, 0, SEEK_SET);
			fseek(fstream, *seek_point, SEEK_SET);
			while (pointer--)
			{
				ch = fgetc(source_stream);
				fputc(ch,fstream);
			}
			fclose(source_stream);
			filehead=update_seekpoint_table(filehead, no_of_files, destination, size, *seek_point);
			*seek_point = *seek_point + size;
		}
		else
			printf("FILE NOT OPENED\n");
		fclose(fstream);
	}
	else
		printf("FILE NOT OPENED\n");
	return filehead;
}
struct file* copy_file_from_disk(struct file *filehead,char *source, char *destination)
{
	struct file *temp = filehead;
	int size;
	char ch;
	unsigned pointer;
	int flag = 0;
	FILE *fstream = fopen("harddisk.hdd", "rb+");
	if (fstream)
	{
		FILE *destination_stream = fopen(destination, "wb");
		if (destination_stream)
		{
			while (temp)
			{
				if (!strcmp(temp->filename, source))
				{
					flag = 1;
					size = temp->size;
					fseek(fstream, temp->pointer, SEEK_SET);
					fseek(destination_stream, 0, SEEK_SET);
					while (size--)
					{
						ch = fgetc(fstream);
						fputc(ch, destination_stream);
					}
					break;
				}
				temp = temp->next;
			}
			if (!flag)
				printf("FILE NOT FOUND INSIDE THE DISK");
			fclose(destination_stream);
		}
		else
			printf("FILE NOT OPENED\n");
		fclose(fstream);
	}
	else
		printf("FILE NOT OPENED\n");
	return filehead;
}

