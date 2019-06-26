#include"filesystemheader.h"

int main()
{
	struct file *filehead = NULL;
	char *command = (char*)malloc(sizeof(char*) * 1000);
	char **tokens = (char**)malloc(sizeof(char) * 6);
	for (int index = 0; index < 6; index++)
		tokens[index] = (char*)malloc(sizeof(char) * 100);
	int no_of_tokens = 0;
	int no_of_files = 0;
	unsigned seek_point = 244;
	FILE *fstream = fopen("harddisk.hdd", "rb+");
	if (fstream)
	{
		if (!fseek(fstream, 0, SEEK_SET))
		{
			if (fread(&no_of_files, 4, 1, fstream))
				filehead = get_seekpoint_table_from_disk(no_of_files, fstream, &seek_point);
			else
			{
				printf("FILE NOT READ\n");
				_getch();
				fclose(fstream);
				return 0;
			}
			fclose(fstream);
		}
		else
			printf("UNSUCCESFULL\n");
	}
	else
	{
		printf("FILE NOT OPENED\n");
		_getch();
		return 0;
	}
	while (1)
	{
		gets(command);
		tokens = divide_into_tokens(command,&no_of_tokens);
		if (no_of_tokens == 1 && !strcmp(tokens[0], "format"))
		{
			filehead = NULL;
			seek_point = 244;
			format_the_disk(&no_of_files);
		}
		else if (no_of_tokens == 1 && !strcmp(tokens[0], "list"))
		{
			list_all_the_files(filehead);
		}
		else if (no_of_tokens == 1 && !strcmp(tokens[0], "debug"))
		{
			debug(filehead);
		}
		else if (no_of_tokens == 1 && !strcmp(tokens[0], "clearmemory"))
		{
			filehead = NULL;
			seek_point = 244;
			format_the_disk(&no_of_files);
			FILE *fstream = fopen("harddisk.hdd", "w");
			if (fstream)
			{
				if (!fseek(fstream, 0, SEEK_SET))
				{
					for (int i = 0; i < 104857600; i++)
						fprintf(fstream, "%c", '\0');
					fclose(fstream);
				}
				else
					printf("UNSUCCESFULL\n");
			}
			else
				printf("FILE NOT OPENED\n");
		}
		else if (no_of_tokens == 3 && !strcmp(tokens[0], "copytodisk"))
		{
			filehead=copy_file_to_disk(filehead,tokens[1],tokens[2],&no_of_files,&seek_point);
		}
		else if (no_of_tokens == 3 && !strcmp(tokens[0], "copyfromdisk"))
		{
			filehead=copy_file_from_disk(filehead,tokens[1], tokens[2]);
		}
		else if (no_of_tokens == 1 && !strcmp(tokens[0], "exit"))
			break;
	}
	return 0;
}
